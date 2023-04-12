#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/native/juce_linux_EventLoopInternal.h>

#include <public.sdk/source/vst/vsteditcontroller.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace juce {

  // Implemented in juce_linux_Messaging.cpp
  bool dispatchNextMessageOnSystemQueue(bool returnIfNoPendingMessages);

  /** @internal */
  class MessageThread : public Thread
  {
  public:
    MessageThread() : Thread("JUCE Plugin Message Thread")
    {
      start();
    }

    ~MessageThread() override
    {
      MessageManager::getInstance()->stopDispatchLoop();
      stop();
    }

    void start()
    {
      startThread(Priority::high);

      // Wait for setCurrentThreadAsMessageThread() and getInstance to be executed
      // before leaving this method
      threadInitialised.wait(10000);
    }

    void stop()
    {
      signalThreadShouldExit();
      stopThread(-1);
    }

    bool isRunning() const noexcept { return isThreadRunning(); }

    void run() override
    {
      MessageManager::getInstance()->setCurrentThreadAsMessageThread();
      XWindowSystem::getInstance();

      threadInitialised.signal();

      while (!threadShouldExit())
      {
        if (!dispatchNextMessageOnSystemQueue(true))
          Thread::sleep(1);
      }
    }

  private:
    WaitableEvent threadInitialised;
    JUCE_DECLARE_NON_MOVEABLE(MessageThread)
      JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageThread)
  };

  //==============================================================================
  /** @internal */
  class HostDrivenEventLoop
  {
  public:
    HostDrivenEventLoop()
    {
      messageThread->stop();
      MessageManager::getInstance()->setCurrentThreadAsMessageThread();
    }

    void processPendingEvents()
    {
      MessageManager::getInstance()->setCurrentThreadAsMessageThread();

      for (;;)
        if (!dispatchNextMessageOnSystemQueue(true))
          return;
    }

    ~HostDrivenEventLoop()
    {
      messageThread->start();
    }

  private:
    SharedResourcePointer<MessageThread> messageThread;
  };

enum class HostMessageThreadAttached { no, yes };

class HostMessageThreadState
{
public:
  template <typename Callback>
  void setStateWithAction(HostMessageThreadAttached stateIn, Callback&& action)
  {
    const std::lock_guard<std::mutex> lock{ m };
    state = stateIn;
    action();
  }

  void assertHostMessageThread()
  {
    const std::lock_guard<std::mutex> lock{ m };

    if (state == HostMessageThreadAttached::no)
      return;

    JUCE_ASSERT_MESSAGE_THREAD
  }

private:
  HostMessageThreadAttached state = HostMessageThreadAttached::no;
  std::mutex m;
};

class EventHandler final : 
public Steinberg::Linux::IEventHandler,
private LinuxEventLoopInternal::Listener
{
public:
  EventHandler()
  {
    LinuxEventLoopInternal::registerLinuxEventLoopListener(*this);
  }

  ~EventHandler() override
  {
    jassert(hostRunLoops.empty());

    LinuxEventLoopInternal::deregisterLinuxEventLoopListener(*this);

    if (!messageThread->isRunning())
      hostMessageThreadState.setStateWithAction(HostMessageThreadAttached::no,
        [this]() { messageThread->start(); });
  }

    tresult PLUGIN_API queryInterface(const TUID targetIID, void** obj) override
  {
      QUERY_INTERFACE(targetIID, obj, IEventHandler::iid, IEventHandler)
        * obj = nullptr;
      return kNoInterface;
  }

  void PLUGIN_API onFDIsSet(Steinberg::Linux::FileDescriptor fd) override
  {
    updateCurrentMessageThread();
    LinuxEventLoopInternal::invokeEventLoopCallbackForFd(fd);
  }

  //==============================================================================
  void registerHandlerForFrame(IPlugFrame* plugFrame)
  {
    if (auto* runLoop = getRunLoopFromFrame(plugFrame))
    {
      refreshAttachedEventLoop([this, runLoop] { hostRunLoops.insert(runLoop); });
      updateCurrentMessageThread();
    }
  }

  void unregisterHandlerForFrame(IPlugFrame* plugFrame)
  {
    if (auto* runLoop = getRunLoopFromFrame(plugFrame))
      refreshAttachedEventLoop([this, runLoop] { hostRunLoops.erase(runLoop); });
  }

  /* Asserts if it can be established that the calling thread is different from the host's message
     thread.

     On Linux this can only be determined if the host has already registered its run loop. Until
     then JUCE messages are serviced by a background thread internal to the plugin.
  */
  static void assertHostMessageThread()
  {
    hostMessageThreadState.assertHostMessageThread();
  }

private:
  //==============================================================================
  /*  Connects all known FDs to a single host event loop instance. */
  class AttachedEventLoop
  {
  public:
    AttachedEventLoop() = default;

    AttachedEventLoop(Steinberg::Linux::IRunLoop* loopIn, Steinberg::Linux::IEventHandler* handlerIn)
      : loop(loopIn), handler(handlerIn)
    {
      for (auto& fd : LinuxEventLoopInternal::getRegisteredFds())
        loop->registerEventHandler(handler, fd);
    }

    AttachedEventLoop(AttachedEventLoop&& other) noexcept
    {
      swap(other);
    }

    AttachedEventLoop& operator= (AttachedEventLoop&& other) noexcept
    {
      swap(other);
      return *this;
    }

    AttachedEventLoop(const AttachedEventLoop&) = delete;
    AttachedEventLoop& operator= (const AttachedEventLoop&) = delete;

    ~AttachedEventLoop()
    {
      if (loop == nullptr)
        return;

      loop->unregisterEventHandler(handler);
    }

  private:
    void swap(AttachedEventLoop& other)
    {
      std::swap(other.loop, loop);
      std::swap(other.handler, handler);
    }

    Steinberg::Linux::IRunLoop* loop = nullptr;
    Steinberg::Linux::IEventHandler* handler = nullptr;
  };

  //==============================================================================
  static Steinberg::Linux::IRunLoop* getRunLoopFromFrame(IPlugFrame* plugFrame)
  {
    Steinberg::Linux::IRunLoop* runLoop = nullptr;

    if (plugFrame != nullptr)
      plugFrame->queryInterface(Steinberg::Linux::IRunLoop::iid, (void**)&runLoop);

    jassert(runLoop != nullptr);
    return runLoop;
  }

  void updateCurrentMessageThread()
  {
    if (!MessageManager::getInstance()->isThisTheMessageThread())
    {
      if (messageThread->isRunning())
        messageThread->stop();

      hostMessageThreadState.setStateWithAction(HostMessageThreadAttached::yes,
        [] { MessageManager::getInstance()->setCurrentThreadAsMessageThread(); });
    }
  }

  void fdCallbacksChanged() override
  {
    // The set of active FDs has changed, so deregister from the current event loop and then
    // re-register the current set of FDs.
    refreshAttachedEventLoop([] {});
  }

  /*  Deregisters from any attached event loop, updates the set of known event loops, and then
      attaches all FDs to the first known event loop.

      The same event loop instance is shared between all plugin instances. Every time an event
      loop is added or removed, this function should be called to register all FDs with a
      suitable event loop.

      Note that there's no API to deregister a single FD for a given event loop. Instead, we must
      deregister all FDs, and then register all known FDs again.
  */
  template <typename Callback>
  void refreshAttachedEventLoop(Callback&& modifyKnownRunLoops)
  {
    // Deregister the old event loop.
    // It's important to call the destructor from the old attached loop before calling the
    // constructor of the new attached loop.
    attachedEventLoop = AttachedEventLoop();

    modifyKnownRunLoops();

    // If we still know about an extant event loop, attach to it.
    if (hostRunLoops.begin() != hostRunLoops.end())
      attachedEventLoop = AttachedEventLoop(*hostRunLoops.begin(), this);
  }

  SharedResourcePointer<MessageThread> messageThread;

  std::atomic<int> refCount{ 1 };

  std::multiset<Steinberg::Linux::IRunLoop*> hostRunLoops;
  AttachedEventLoop attachedEventLoop;

  static HostMessageThreadState hostMessageThreadState;

  //==============================================================================
  //JUCE_DECLARE_NON_MOVEABLE(EventHandler)
  //  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EventHandler)
};

HostMessageThreadState EventHandler::hostMessageThreadState;

static void assertHostMessageThread()
{
#if JUCE_LINUX || JUCE_BSD
  EventHandler::assertHostMessageThread();
#else
  JUCE_ASSERT_MESSAGE_THREAD
#endif
}

}
