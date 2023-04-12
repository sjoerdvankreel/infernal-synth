/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

// inf added these
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/native/juce_linux_EventLoopInternal.h>
#include <juce_audio_plugin_client/utility/juce_LinuxMessageThread.h>
#include <public.sdk/source/vst/vsteditcontroller.h>
#include <mutex>
#include <set>

#define JUCE_DECLARE_VST3_COM_REF_METHODS \
Steinberg::uint32 PLUGIN_API addRef() override   { return (Steinberg::uint32) ++refCount; } \
Steinberg::uint32 PLUGIN_API release() override  { const int r = --refCount; if (r == 0) delete this; return (Steinberg::uint32) r; }
// inf trimmed everything else

namespace juce
{

using namespace Steinberg;

#if JUCE_LINUX || JUCE_BSD

enum class HostMessageThreadAttached { no, yes };

class HostMessageThreadState
{
public:
    template <typename Callback>
    void setStateWithAction (HostMessageThreadAttached stateIn, Callback&& action)
    {
        const std::lock_guard<std::mutex> lock { m };
        state = stateIn;
        action();
    }

    void assertHostMessageThread()
    {
        const std::lock_guard<std::mutex> lock { m };

        if (state == HostMessageThreadAttached::no)
            return;

        JUCE_ASSERT_MESSAGE_THREAD
    }

private:
    HostMessageThreadAttached state = HostMessageThreadAttached::no;
    std::mutex m;
};

class EventHandler final  : public Steinberg::Linux::IEventHandler,
                            private LinuxEventLoopInternal::Listener
{
public:
    EventHandler()
    {
        LinuxEventLoopInternal::registerLinuxEventLoopListener (*this);
    }

    ~EventHandler() override
    {
        jassert (hostRunLoops.empty());

        LinuxEventLoopInternal::deregisterLinuxEventLoopListener (*this);

        if (! messageThread->isRunning())
            hostMessageThreadState.setStateWithAction (HostMessageThreadAttached::no,
                                                       [this]() { messageThread->start(); });
    }

    JUCE_DECLARE_VST3_COM_REF_METHODS

    tresult PLUGIN_API queryInterface (const TUID targetIID, void** obj) override
    {
      // inf testFor pulls in a load of stuff
      QUERY_INTERFACE(targetIID, obj, IEventHandler::iid, IEventHandler)
      * obj = nullptr;
      return kNoInterface;
    }

    void PLUGIN_API onFDIsSet (Steinberg::Linux::FileDescriptor fd) override
    {
        updateCurrentMessageThread();
        LinuxEventLoopInternal::invokeEventLoopCallbackForFd (fd);
    }

    //==============================================================================
    void registerHandlerForFrame (IPlugFrame* plugFrame)
    {
        if (auto* runLoop = getRunLoopFromFrame (plugFrame))
        {
            refreshAttachedEventLoop ([this, runLoop] { hostRunLoops.insert (runLoop); });
            updateCurrentMessageThread();
        }
    }

    void unregisterHandlerForFrame (IPlugFrame* plugFrame)
    {
        if (auto* runLoop = getRunLoopFromFrame (plugFrame))
            refreshAttachedEventLoop ([this, runLoop] { hostRunLoops.erase (runLoop); });
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

        AttachedEventLoop (Steinberg::Linux::IRunLoop* loopIn, Steinberg::Linux::IEventHandler* handlerIn)
            : loop (loopIn), handler (handlerIn)
        {
            for (auto& fd : LinuxEventLoopInternal::getRegisteredFds())
                loop->registerEventHandler (handler, fd);
        }

        AttachedEventLoop (AttachedEventLoop&& other) noexcept
        {
            swap (other);
        }

        AttachedEventLoop& operator= (AttachedEventLoop&& other) noexcept
        {
            swap (other);
            return *this;
        }

        AttachedEventLoop (const AttachedEventLoop&) = delete;
        AttachedEventLoop& operator= (const AttachedEventLoop&) = delete;

        ~AttachedEventLoop()
        {
            if (loop == nullptr)
                return;

            loop->unregisterEventHandler (handler);
        }

    private:
        void swap (AttachedEventLoop& other)
        {
            std::swap (other.loop, loop);
            std::swap (other.handler, handler);
        }

        Steinberg::Linux::IRunLoop* loop = nullptr;
        Steinberg::Linux::IEventHandler* handler = nullptr;
    };

    //==============================================================================
    static Steinberg::Linux::IRunLoop* getRunLoopFromFrame (IPlugFrame* plugFrame)
    {
        Steinberg::Linux::IRunLoop* runLoop = nullptr;

        if (plugFrame != nullptr)
            plugFrame->queryInterface (Steinberg::Linux::IRunLoop::iid, (void**) &runLoop);

        jassert (runLoop != nullptr);
        return runLoop;
    }

    void updateCurrentMessageThread()
    {
        if (! MessageManager::getInstance()->isThisTheMessageThread())
        {
            if (messageThread->isRunning())
                messageThread->stop();

            hostMessageThreadState.setStateWithAction (HostMessageThreadAttached::yes,
                                                       [] { MessageManager::getInstance()->setCurrentThreadAsMessageThread(); });
        }
    }

    void fdCallbacksChanged() override
    {
        // The set of active FDs has changed, so deregister from the current event loop and then
        // re-register the current set of FDs.
        refreshAttachedEventLoop ([]{});
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
    void refreshAttachedEventLoop (Callback&& modifyKnownRunLoops)
    {
        // Deregister the old event loop.
        // It's important to call the destructor from the old attached loop before calling the
        // constructor of the new attached loop.
        attachedEventLoop = AttachedEventLoop();

        modifyKnownRunLoops();

        // If we still know about an extant event loop, attach to it.
        if (hostRunLoops.begin() != hostRunLoops.end())
            attachedEventLoop = AttachedEventLoop (*hostRunLoops.begin(), this);
    }

    SharedResourcePointer<MessageThread> messageThread;

    std::atomic<int> refCount { 1 };

    std::multiset<Steinberg::Linux::IRunLoop*> hostRunLoops;
    AttachedEventLoop attachedEventLoop;

    static HostMessageThreadState hostMessageThreadState;

    //==============================================================================
    JUCE_DECLARE_NON_MOVEABLE (EventHandler)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EventHandler)
};

HostMessageThreadState EventHandler::hostMessageThreadState;

#endif

} // namespace juce
