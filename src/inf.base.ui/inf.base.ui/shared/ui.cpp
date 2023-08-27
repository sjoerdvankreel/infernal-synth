#include <inf.base.ui/shared/ui.hpp>
#include <inf.base.ui/shared/config.hpp>
#include <inf.base.ui/shared/real_bounds_range.hpp>
#include <inf.base.ui/controls/label.hpp>
#include <inf.base.ui/controls/button.hpp>
#include <inf.base.ui/controls/toggle_button.hpp>
#include <inf.base.ui/controls/param_dropdown.hpp>
#include <inf.base.ui/controls/file_chooser_dialog.hpp>
#include <inf.base/shared/support.hpp>

#include <stack>

using namespace juce;
using namespace inf::base;

namespace inf::base::ui {

static std::int32_t const exact_edit_dialog_h = 78;
static std::int32_t const exact_edit_dialog_w = 200;
static std::int32_t const exact_edit_dialog_pad = 1;

struct dialog_box_state
{
  // mind order of destruction
  std::unique_ptr<inf_look_and_feel> lnf;
  std::unique_ptr<grid_element> content;
  std::unique_ptr<AlertWindow> window;
  
  virtual ~dialog_box_state() 
  {
    if(content) content->set_lnf(nullptr);
    if(content->component()) content->component()->setLookAndFeel(nullptr);
    if (window) window->setLookAndFeel(nullptr);
  }
};

struct confirm_box_state:
public dialog_box_state
{
  confirmed_callback confirmed;
  cancelled_callback cancelled;
  inf::base::plugin_controller* controller;
};

struct file_box_state
{
  // mind order of destruction
  inf::base::plugin_controller* controller;
  std::unique_ptr<inf_look_and_feel> lnf;
  std::unique_ptr<inf_file_chooser_dialog> box;
  std::unique_ptr<WildcardFileFilter> filter;
  std::unique_ptr<FileBrowserComponent> browser;
  virtual ~file_box_state() { if (box) box->setLookAndFeel(nullptr); }
};

struct dropdown_tree
{
  std::string name;
  list_item const* item; // Only for leafs.
  std::vector<std::unique_ptr<dropdown_tree>> children;
};

class exact_edit_dialog_content :
public juce::Component
{
private:
  juce::Label _title;
  juce::TextButton _ok;
  juce::TextButton _cancel;
  juce::TextEditor _editor;

  std::int32_t const _param_index;
  std::unique_ptr<inf_look_and_feel> _lnf;
  base::plugin_controller* const _controller;

public:
  exact_edit_dialog_content(base::plugin_controller* controller,
    std::int32_t param_index, std::unique_ptr<inf_look_and_feel>&& lnf) :
    _param_index(param_index), _lnf(std::move(lnf)), _controller(controller)
  {
    _lnf->setColour(Label::ColourIds::textColourId, _lnf->findColour(inf_look_and_feel::colors::dialog_label_text));
    _lnf->setColour(Label::ColourIds::textWhenEditingColourId, _lnf->findColour(inf_look_and_feel::colors::dialog_label_text));
    _lnf->setColour(Label::ColourIds::backgroundColourId, _lnf->findColour(inf_look_and_feel::colors::dialog_label_background));
    _lnf->setColour(Label::ColourIds::backgroundWhenEditingColourId, _lnf->findColour(inf_look_and_feel::colors::dialog_label_background));
    _lnf->setColour(TextButton::ColourIds::textColourOffId, _lnf->findColour(inf_look_and_feel::colors::dialog_button_text));
    _lnf->setColour(TextButton::ColourIds::buttonColourId, _lnf->findColour(inf_look_and_feel::colors::dialog_button_background));

    std::int32_t pad = exact_edit_dialog_pad;
    setSize(exact_edit_dialog_w + 3 * pad, exact_edit_dialog_h + 6 * pad);
    setLookAndFeel(_lnf.get());

    auto title = juce::String("Edit " + controller->topology()->params[param_index].runtime_name);
    _title.setText(title, juce::dontSendNotification);
    _title.setLookAndFeel(_lnf.get());
    _title.setFont(juce::Font(15.0, juce::Font::bold));
    addAndMakeVisible(_title);
    _title.setBounds(pad, pad, exact_edit_dialog_w - pad, exact_edit_dialog_h / 3);

    _editor.setLookAndFeel(_lnf.get());
    _editor.setFont(juce::Font(14.0));
    _editor.setJustification(juce::Justification::right);
    _editor.setText(juce::String(controller->topology()->params[param_index].descriptor->data.format(false, controller->ui_value_at_index(param_index))));
    addAndMakeVisible(_editor);
    _editor.setBounds(pad, 2 * pad + exact_edit_dialog_h / 3, exact_edit_dialog_w, exact_edit_dialog_h / 3);

    _ok.setButtonText("OK");
    _ok.setLookAndFeel(_lnf.get());
    _ok.addShortcut(KeyPress(KeyPress::returnKey));
    addAndMakeVisible(_ok);
    _ok.setBounds(pad, 3 * pad + exact_edit_dialog_h * 2 / 3, exact_edit_dialog_w / 2 - pad, exact_edit_dialog_h / 3);
    _ok.onClick = [this]() { 
      if (ResizableWindow* w = findParentComponentOfClass<ResizableWindow>())
      {
        param_value new_ui_value;
        auto text = _editor.getText().toStdString();
        auto part_index = _controller->topology()->params[_param_index].part_index;
        if(_controller->topology()->params[_param_index].descriptor->data.parse(false, part_index, text.c_str(), new_ui_value))
          _controller->editor_param_changed(_param_index, new_ui_value);
        w->exitModalState(0);
        delete w;
      }
    };

    _cancel.setButtonText("Cancel");
    _cancel.setLookAndFeel(_lnf.get());
    _cancel.addShortcut(KeyPress(KeyPress::escapeKey));
    addAndMakeVisible(_cancel);
    _cancel.setBounds(pad + exact_edit_dialog_w / 2, 3 * pad + exact_edit_dialog_h * 2 / 3, exact_edit_dialog_w / 2 - pad, exact_edit_dialog_h / 3);
    _cancel.onClick = [this]() {
      if (ResizableWindow* w = findParentComponentOfClass<ResizableWindow>())
      {
        w->exitModalState(0);
        delete w;
      }
    };
  }
};

static void
run_dialog_box(dialog_box_state* state, std::int32_t w, std::int32_t h)
{
  std::int32_t const margin = 5;
  state->content->build(state->lnf.get());
  state->content->component()->setBounds(0, 0, w, h);
  state->content->layout();
  state->window->addCustomComponent(state->content->component());
  state->window->setSize(w + 2 * margin, h + 2 * margin);
  state->content->component()->setBounds(margin, margin, w, h);
  state->window->enterModalState();
}

static file_box_state*
create_preset_file_box_state(inf::base::plugin_controller* controller, 
  lnf_factory lnf_factory, std::string const& title, int flags)
{
  juce::File last_directory;
  auto state = new file_box_state;
  auto filter_match = std::string("*.") + controller->plugin_preset_file_extension();
  flags |= FileBrowserComponent::canSelectFiles;
  state->controller = controller;
  state->lnf = lnf_factory(controller);
  state->lnf->setColour(Label::ColourIds::textColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_label_text));
  state->lnf->setColour(Label::ColourIds::textWhenEditingColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_label_text));
  state->lnf->setColour(Label::ColourIds::backgroundColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_label_background));
  state->lnf->setColour(Label::ColourIds::backgroundWhenEditingColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_label_background));
  state->lnf->setColour(TextButton::ColourIds::textColourOffId, state->lnf->findColour(inf_look_and_feel::colors::dialog_button_text));
  state->lnf->setColour(TextButton::ColourIds::buttonColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_button_background));
  state->lnf->setColour(FileChooserDialogBox::ColourIds::titleTextColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_title));
  state->lnf->setColour(FileBrowserComponent::ColourIds::filenameBoxTextColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_file_text));
  state->lnf->setColour(FileBrowserComponent::ColourIds::filenameBoxBackgroundColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_file_background));
  state->lnf->setColour(DirectoryContentsDisplayComponent::ColourIds::textColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_selector_text));
  state->lnf->setColour(DirectoryContentsDisplayComponent::ColourIds::highlightColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_selector_highlight));
  state->lnf->setColour(DirectoryContentsDisplayComponent::ColourIds::highlightedTextColourId, state->lnf->findColour(inf_look_and_feel::colors::dialog_selector_highlight_text));
  if(!state->controller->get_last_directory().empty()) last_directory = juce::File(state->controller->get_last_directory());
  state->filter = std::make_unique<WildcardFileFilter>(filter_match, String(), "Preset files");
  state->browser = std::make_unique<FileBrowserComponent>(flags, last_directory, state->filter.get(), nullptr);
  auto background = state->lnf->findColour(inf_look_and_feel::colors::dialog_background);
  auto editor = static_cast<juce::Component*>(controller->current_editor_window());
  state->box = std::make_unique<inf_file_chooser_dialog>(controller, title, *state->browser, background, editor);
  state->box->setLookAndFeel(state->lnf.get());
  state->browser->setLookAndFeel(state->lnf.get());
  return state;
}

static void 
append_dropdown_tree(dropdown_tree* tree, list_item const* item, std::string const* path, std::size_t path_size)
{
  dropdown_tree* child = nullptr;
  for(std::size_t i = 0; i < tree->children.size(); i++)
    if (tree->children[i]->name == *path)
    {
      child = tree->children[i].get();
      break;
    }
  if (child == nullptr)
  {
    child = new dropdown_tree();
    child->name = *path;
    child->item = nullptr;
    tree->children.push_back(std::unique_ptr<dropdown_tree>(child));
  }
  if(path_size > 1)
    append_dropdown_tree(child, item, path + 1, path_size - 1);
  else
    child->item = item;
}

static void
fill_dropdown_menu(
  inf_param_dropdown* dropdown, PopupMenu* menu, 
  dropdown_tree const* tree, std::int32_t& counter)
{
  if (tree->children.size() == 0)
  {
    assert(tree->item != nullptr);
    std::int32_t id = dropdown_id_offset + counter++;
    menu->addItem(id, tree->name);
    dropdown->set_item_id(tree->item, id);
    return;
  }

  assert(tree->item == nullptr);
  PopupMenu child;
  for (std::size_t i = 0; i < tree->children.size(); i++)
    fill_dropdown_menu(dropdown, &child, tree->children[i].get(), counter);
  menu->addSubMenu(tree->name, child);
}

static void 
fill_dropdown_menu(inf_param_dropdown* dropdown, PopupMenu* menu, std::vector<list_item> const& items)
{
  std::int32_t counter = 0;
  auto tree = std::make_unique<dropdown_tree>();
  for (auto const& item : items)
    append_dropdown_tree(tree.get(), &item, item.submenu_path.data(), item.submenu_path.size());
  for(auto const& child: tree->children)
    fill_dropdown_menu(dropdown, menu, child.get(), counter);
}

static void 
show_exact_edit_dialog(base::plugin_controller* controller,
  std::int32_t param_index, lnf_factory lnf_factory)
{
  auto title = juce::String("Edit " + controller->topology()->params[param_index].runtime_name);
  auto window = std::make_unique<juce::ResizableWindow>(title, true);
  window->setResizable(false, false);
  window->setContentOwned(new exact_edit_dialog_content(controller, param_index, lnf_factory(controller)), false);
  window->setOpaque(true);
  window->addToDesktop(0, nullptr);
  window->centreAroundComponent(
    static_cast<juce::Component*>(controller->current_editor_window()), 
    exact_edit_dialog_w + 3 * exact_edit_dialog_pad, 
    exact_edit_dialog_h + 6 * exact_edit_dialog_pad);
  window->enterModalState(true);
  window.release();
}

void
show_context_menu_for_param(
  base::plugin_controller* controller, std::int32_t param_index, 
  bool exact_edit, juce::LookAndFeel* lnf, lnf_factory lnf_factory)
{
  PopupMenu menu;
  menu.setLookAndFeel(lnf);

  if (exact_edit)
    menu.addItem(1, "Edit...");

  auto host_menu = controller->host_menu_for_param_index(param_index);
  std::int32_t host_menu_count = host_menu? host_menu->item_count(): 0;
  if (!exact_edit && host_menu_count == 0) return;

  std::stack<PopupMenu> host_menu_stack;
  host_menu_stack.push(PopupMenu());
  std::stack<host_context_menu_item> host_context_menu_item_stack;
  host_context_menu_item host_item = {};
  host_item.name = "Host";
  host_context_menu_item_stack.push(host_item);

  for (std::int32_t i = 0; i < host_menu_count; i++)
  {
    host_context_menu_item item = host_menu->get_item(i);
    if ((item.flags & host_context_menu_item::group_start) != 0)
    {
      host_menu_stack.push(PopupMenu());
      host_context_menu_item_stack.push(item);
    }
    else if ((item.flags & host_context_menu_item::group_end) != 0)
    {
      auto sub_menu = host_menu_stack.top();
      auto sub_menu_item = host_context_menu_item_stack.top();
      host_menu_stack.pop();
      host_context_menu_item_stack.pop();
      host_menu_stack.top().addSubMenu(sub_menu_item.name, sub_menu, true);
    }
    else if ((item.flags & host_context_menu_item::separator) != 0)
      host_menu_stack.top().addSeparator();
    else
    {
      bool checked = (item.flags & host_context_menu_item::checked) != 0;
      bool enabled = (item.flags & host_context_menu_item::enabled) != 0;
      host_menu_stack.top().addItem(i + 2, item.name, enabled, checked);
    }
  }

  if(host_menu_count != 0)
    menu.addSubMenu("Host", host_menu_stack.top(), true);

  menu.showMenuAsync(PopupMenu::Options(), [controller, param_index, lnf_factory, host_menu_count, host_menu = host_menu.release()](int option) {
    if(option == 1) show_exact_edit_dialog(controller, param_index, lnf_factory);
    else if(option > 1 && option <= host_menu_count + 1) host_menu->item_clicked(option - 2);
    delete host_menu;
  });
}

void
ui_element::relevant_if(part_id id, std::int32_t param_index, bool hide_if_irrelevant, relevance_selector selector)
{
  _relevant_if_part = id;
  _relevant_if_param = param_index;
  _relevant_if_selector = selector;
  _hide_if_irrelevant = hide_if_irrelevant;
}

Component*
ui_element::build(LookAndFeel* lnf)
{
  LookAndFeel* this_lnf = get_lnf() != nullptr? get_lnf(): lnf;
  _component.reset(build_core(*this_lnf));
  _component->setLookAndFeel(this_lnf);
  _component->setVisible(_initially_visible);
  if (_relevant_if_selector != nullptr)
  {
    std::int32_t index = controller()->topology()->param_index(_relevant_if_part, _relevant_if_param);
    assert(controller()->topology()->params[index].descriptor->data.type != param_type::real);
    std::int32_t rt_part_index = controller()->topology()->params[index].part_index;
    std::int32_t part_index = _controller->topology()->parts[rt_part_index].type_index;
    param_value value = controller()->state()[index];
    if(_hide_if_irrelevant) _component->setVisible(_relevant_if_selector(part_index, value.discrete));
    else _component->setEnabled(_relevant_if_selector(part_index, value.discrete));
    _relevance_listener.reset(new relevance_listener(controller(), _component.get(), index, _hide_if_irrelevant, _relevant_if_selector));
  }
  return _component.get();
}

Component* 
label_element::build_core(juce::LookAndFeel& lnf)
{
  Label* result = new inf_label(true);
  result->setBorderSize(BorderSize<int>());
  result->setJustificationType(_justification);
  result->setFont(juce::Font(_font_height, juce::Font::bold));
  result->setText(_text, dontSendNotification);
  result->setColour(Label::ColourIds::textColourId, lnf.findColour(_color));
  return result;
}  

Component* 
last_edit_label_element::build_core(juce::LookAndFeel& lnf)
{
  Label* result = new inf_label(false);
  result->setBorderSize(BorderSize<int>());
  result->setJustificationType(_justification);
  result->setText(format_label_short(controller(), 0), dontSendNotification);
  result->setFont(juce::Font(get_param_label_font_height(controller()), juce::Font::bold));
  result->setColour(Label::ColourIds::textColourId, lnf.findColour(inf_look_and_feel::colors::param_label));
  _listener.reset(new last_edit_label_param_listener(controller(), result));
  return result;
}

Component*
last_edit_value_element::build_core(juce::LookAndFeel& lnf)
{
  TextEditor* result = new TextEditor;
  auto value = controller()->ui_value_at_index(0);
  result->setJustification(juce::Justification::centredRight);
  result->setFont(juce::Font(get_text_edit_font_height(controller()), juce::Font::bold));
  result->setText(controller()->topology()->params[0].descriptor->data.format(false, value));
  result->setColour(TextEditor::ColourIds::textColourId, lnf.findColour(inf_look_and_feel::colors::text_edit_text));
  _listener.reset(new last_edit_value_param_listener(controller(), result));
  result->addListener(_listener.get());
  return result;
}

Component*
button_element::build_core(juce::LookAndFeel& lnf)
{
  inf_button* result = new inf_button(controller());
  result->setButtonText(_text);
  _listener.reset(new button_listener(_callback));
  result->addListener(_listener.get());
  return result;
}

Component*
action_dropdown_element::build_core(juce::LookAndFeel& lnf)
{
  ComboBox* result = new ComboBox;
  result->setTooltip(_default_text);
  result->setTextWhenNothingSelected(_default_text);
  for(std::size_t i = 0; i < _items.size(); i++)
    result->addItem(_items[i], static_cast<std::int32_t>(i) + dropdown_id_offset);
  _listener.reset(new action_dropdown_listener(_callback));
  _reload_listener.reset(new combo_reload_listener(controller(), result, _reload_callback));
  result->addListener(_listener.get());
  result->setSelectedItemIndex(_initial_index, dontSendNotification);
  return result;
}

void
action_dropdown_element::layout()
{
  auto bounds = component()->getBounds().expanded(-dropdown_margin);
  component()->setBounds(bounds);
}

Component* 
root_element::build_core(LookAndFeel& lnf)
{
  container_component* result = new container_component(
    container_component::flags::fill, 0.0f, 0.0f, 
    inf_look_and_feel::colors::root_background, inf_look_and_feel::colors::root_background, 0, 0);
  result->addChildComponent(_content->build(&lnf));
  result->setOpaque(true);
  _tooltip.reset(new TooltipWindow(result));
  return result;
}

void
root_element::layout()
{
  std::int32_t w = _width;
  std::int32_t h = static_cast<std::int32_t>(std::ceil(1.0f / controller()->get_editor_properties().aspect_ratio * w));
  component()->setBounds(0, 0, w, h);
  _content->component()->setBounds(0, 0, w, h);
  _content->layout();
}

Component*
container_element::build_core(LookAndFeel& lnf)
{
  container_component* result = new container_component(
    _flags, container_radius, container_outline_size, _fill_low_color_id, 
    _fill_high_color_id, _outline_low_color_id, _outline_high_color_id);
  result->addChildComponent(_content->build(&lnf));
  return result;
}

void
container_element::layout()
{
  auto bounds = component()->getLocalBounds();
  auto with_padding = Rectangle<int>(
    bounds.getX() + _padding.x / 2,
    bounds.getY() + _padding.y / 2,
    bounds.getWidth() - _padding.x,
    bounds.getHeight() - _padding.y);
  _content->component()->setBounds(with_padding);
  _content->layout();
}

Component*
group_label_element::build_core(LookAndFeel& lnf)
{
  Label* result = new inf_label(false);
  result->setText(_text, dontSendNotification);
  result->setJustificationType(Justification::centred);
  result->setFont(juce::Font(get_group_label_font_height(controller()), juce::Font::bold));
  result->setColour(Label::ColourIds::textColourId, lnf.findColour(inf_look_and_feel::colors::part_group_label));
  return result;
}

void
group_label_element::layout()
{
  auto label = dynamic_cast<Label*>(component());
  label->setBorderSize(BorderSize<int>());
  float rotation_angles = _vertical? 270.0f / 360.0f * 2.0f * pi32: 0.0f;
  auto transform = AffineTransform().rotated(rotation_angles, label->getWidth() / 2.0f, label->getHeight() / 2.0f);
  label->setTransform(transform);
  label->setBounds(label->getBounds().transformedBy(transform));
  label->setJustificationType(_vertical? Justification::centred : Justification::centredBottom);
}

Component*
selector_label_element::build_core(LookAndFeel& lnf)
{
  Label* result = new inf_selector_label(controller(), _part_type, _part_count, _vertical, _routing_dir, _justification);
  result->setText(_text, dontSendNotification);
  result->setFont(juce::Font(get_selector_font_height(controller()), juce::Font::bold));
  return result;
}

Component* 
selector_bar_element::build_core(LookAndFeel& lnf)
{
  std::int32_t index = controller()->topology()->param_index(_selector_part_id, _selector_param_index);
  inf_selector_bar* result = new inf_selector_bar(controller(), _selected_part_type);
  result->setMinimumTabScaleFactor(0.0f);
  for(std::size_t i = 0; i < _headers.size(); i++)
    result->addTab(_headers[i], Colours::black, static_cast<int>(i));
  result->setCurrentTabIndex(controller()->state()[index].discrete, false);
  _listener.reset(new selector_listener(controller(), result, index));
  result->add_listener(_listener.get());
  if(_extra_listener) result->add_listener(_extra_listener.get());
  return result;
}

Component*
param_label_element::build_core(LookAndFeel& lnf)
{
  Label* result = new inf_label(true);
  result->setBorderSize(BorderSize<int>());
  auto topology = controller()->topology();
  float font_height = get_param_label_font_height(controller());
  auto const& desc = topology->get_param_descriptor(_part_id, _param_index);
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  param_value value = controller()->ui_value_at(_part_id, _param_index);
  if(desc.data.type == param_type::real) value.real = desc.data.real.display.to_range(value.real);
  result->setJustificationType(_justification);
  result->setFont(juce::Font(font_height, juce::Font::bold));
  result->setText(get_label_text(&desc, _type, value), dontSendNotification);
  result->setColour(Label::ColourIds::textColourId, lnf.findColour(inf_look_and_feel::colors::param_label));
  _listener.reset(new label_param_listener(controller(), result, index, _type));
  return result;
}

Component*
param_icon_element::build_core(LookAndFeel& lnf)
{
  inf_icon* result = new inf_icon(_value, _selector);
  if(_selector != nullptr)
  {
    std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
    _listener.reset(new icon_param_listener(controller(), result, index));
  }
  return result;
}

std::unique_ptr<param_icon_element>
create_param_icon_ui(
  inf::base::plugin_controller* controller, std::int32_t part_type, 
  std::int32_t part_index, std::int32_t param_index, icon_selector selector)
{ 
  std::int32_t index = controller->topology()->param_index({ part_type, part_index }, param_index);
  std::int32_t icon_value = controller->state()[index].discrete;
  return std::make_unique<param_icon_element>(controller, part_id(part_type, part_index), param_index, icon_value, selector);
}

void 
param_edit_element::layout()
{
  auto margin = _in_table? edit_table_margin: edit_margin;
  auto bounds = component()->getBounds().expanded(-margin, -margin);
  if (_type == edit_type::dropdown)
  {
    float total_height = get_dropdown_height(controller());
    float total_width = bounds.getWidth() - dropdown_hpad;
    float drop_x = bounds.getX() + dropdown_hpad / 2.0f;
    float drop_y = bounds.getY() + (bounds.getHeight() - total_height) / 2.0f;
    bounds = Rectangle<int>(
      static_cast<int>(drop_x), static_cast<int>(drop_y),
      static_cast<int>(total_width), static_cast<int>(total_height));
  }
  component()->setBounds(bounds);
}

Component* 
param_edit_element::build_core(juce::LookAndFeel& lnf)
{
  Component* result = nullptr;
  switch (_type)
  {
  case edit_type::toggle: result = build_toggle_core(lnf); break;
  case edit_type::tab_bar: result = build_tab_bar_core(lnf); break;
  case edit_type::dropdown: result = build_dropdown_core(lnf); break;
  default: result = build_slider_core(lnf); break;
  }
  auto client = dynamic_cast<SettableTooltipClient*>(result);
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  auto const& desc = *controller()->topology()->params[index].descriptor;
  if (_tooltip_type == tooltip_type::label)
    client->setTooltip(desc.data.static_name.detail);
  if(controller()->topology()->static_parts[_part_id.type].kind == part_kind::output)
    result->setEnabled(false);
  else if(_tooltip_type == tooltip_type::value)
  {
    _tooltip_listener.reset(new tooltip_listener(controller(), client, index));
    param_value ui_value = controller()->ui_value_at(_part_id, _param_index);
    client->setTooltip(desc.data.format(false, ui_value) + desc.data.unit);
  }
  return result;
}

Component*
param_edit_element::build_toggle_core(LookAndFeel& lnf)
{
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  ToggleButton* result = new inf_toggle_button(controller(), index, _force_toggle_on, _lnf_factory);
  _toggle_listener.reset(new toggle_param_listener(controller(), result, index));
  result->setToggleState(controller()->state()[index].discrete != 0, dontSendNotification);
  result->addListener(_toggle_listener.get());
  return result;
}

Component*
param_edit_element::build_tab_bar_core(LookAndFeel& lnf)
{
  TabbedButtonBar* result = new TabbedButtonBar(TabbedButtonBar::TabsAtTop);
  auto const& desc = controller()->topology()->get_param_descriptor(_part_id, _param_index);
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  for(std::size_t i = 0; i < desc.data.discrete.items->size(); i++)
    result->addTab((*desc.data.discrete.items)[i].name, Colours::black, static_cast<int>(i));
  result->setCurrentTabIndex(controller()->state()[index].discrete, dontSendNotification);
  _tab_bar_listener.reset(new tab_bar_param_listener(controller(), result, index));
  result->addChangeListener(_tab_bar_listener.get());
  return result;
}

Component*
param_edit_element::build_dropdown_core(LookAndFeel& lnf)
{
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  std::int32_t part_index = controller()->topology()->params[index].part_index;
  auto const& desc = controller()->topology()->get_param_descriptor(_part_id, _param_index);
  inf_param_dropdown* result = new inf_param_dropdown(controller(), index, _lnf_factory, desc.data.discrete.enabled_selector);
  result->setColour(ComboBox::ColourIds::textColourId, lnf.findColour(inf_look_and_feel::colors::dropdown_text));
  result->setJustificationType(Justification::centred);
  if (desc.data.discrete.items != nullptr)
    if((*desc.data.discrete.items)[0].submenu_path.size() > 0)
      fill_dropdown_menu(result, result->getRootMenu(), *desc.data.discrete.items);
    else
      for (std::int32_t i = 0; i <= desc.data.discrete.effective_max(part_index); i++)
        result->addItem((*desc.data.discrete.items)[i].name, static_cast<std::int32_t>(i) + dropdown_id_offset);
  else if (desc.data.discrete.names != nullptr)
    for (std::int32_t i = 0; i <= desc.data.discrete.effective_max(part_index); i++)
      result->addItem((*desc.data.discrete.names)[i], static_cast<std::int32_t>(i) + dropdown_id_offset);
  else
    for (std::int32_t i = desc.data.discrete.min; i <= desc.data.discrete.effective_max(part_index); i++)
      result->addItem(std::to_string(i), static_cast<std::int32_t>(i) + dropdown_id_offset);
  result->setSelectedId(controller()->state()[index].discrete + dropdown_id_offset, dontSendNotification);
  _dropdown_listener.reset(new dropdown_param_listener(controller(), result, index));
  result->addListener(_dropdown_listener.get());
  return result;
}

Component*
param_edit_element::build_slider_core(LookAndFeel& lnf)
{
  std::int32_t index = controller()->topology()->param_index(_part_id, _param_index);
  auto const& desc = controller()->topology()->get_param_descriptor(_part_id, _param_index);
  auto default_value = controller()->topology()->base_to_ui_value(index, desc.data.default_value());
  std::int32_t part_index = controller()->topology()->params[index].part_index;
  inf_param_slider* result = new inf_param_slider(controller(), index, _type, _lnf_factory);
  result->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
  if(desc.data.type == param_type::real)
  {
    result->setNormalisableRange(real_bounds_range(desc.data.real.display)),
    result->setValue(desc.data.real.display.to_range(controller()->state()[index].real), dontSendNotification);
    result->setDoubleClickReturnValue(true, default_value.real, ModifierKeys::noModifiers);
  }
  else
  {
    // Juce freaks out if min==max. Just set to full range on the assumption that the slider will be disabled anyway.
    std::int32_t effective_max = desc.data.discrete.effective_max(part_index);
    std::int32_t max = effective_max == desc.data.discrete.min? desc.data.discrete.max: effective_max;
    result->setRange(desc.data.discrete.min, max, 1.0);
    result->setValue(controller()->state()[index].discrete, dontSendNotification);
    result->setDoubleClickReturnValue(true, default_value.discrete, ModifierKeys::noModifiers);
  }
  switch (_type)
  {
  case edit_type::knob:
  case edit_type::selector:
    result->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    break;
  case edit_type::hslider:
    result->setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    break;
  case edit_type::vslider:
    result->setSliderStyle(Slider::SliderStyle::LinearVertical);
    break;
  default:
    assert(false);
    break;
  }
  _slider_listener.reset(new slider_param_listener(controller(), result, index));
  result->addListener(_slider_listener.get());
  return result;
}

juce::Component* 
part_graph_element::build_core(juce::LookAndFeel& lnf)
{ 
  auto& inf_lnf = dynamic_cast<inf_look_and_feel&>(lnf);
  inf_graph_plot* result = new inf_graph_plot(controller(), _part_id, _graph_type, inf_lnf.get_graph_bg_image_path(_background_id));
  _listener.reset(new graph_listener(controller(), result, _part_id, _graph_type, _tooltip_param));
  auto const& desc = controller()->topology()->get_part_descriptor(_part_id);
  result->setTooltip(desc.name_selector(controller()->topology(), controller()->state(), _part_id, _graph_type));
  return result;
}

Component*
grid_element::build_core(LookAndFeel& lnf)
{
  Component* result = new Component;
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
    result->addChildComponent(_cell_contents[i]->build(&lnf));
  return result;
}

ui_element*
grid_element::add_cell(std::unique_ptr<ui_element>&& content,
  std::int32_t row, std::int32_t col, std::int32_t row_span, std::int32_t col_span)
{
  ui_element* result = content.get();
  _cell_bounds.push_back(Rectangle<std::int32_t>(col, row, col_span, row_span));
  _cell_contents.push_back(std::move(content));
  return result;
}

void
grid_element::layout()
{
  Grid grid;
  for(std::size_t row = 0; row < _row_distribution.size(); row++)
    grid.templateRows.add(_row_distribution[row]);
  for (std::size_t col = 0; col < _column_distribution.size(); col++)
    grid.templateColumns.add(_column_distribution[col]);
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
  {
    GridItem item(_cell_contents[i]->component());
    Rectangle<std::int32_t> bounds = _cell_bounds[i];
    grid.items.add(item.withArea(bounds.getY() + 1, bounds.getX() + 1, GridItem::Span(bounds.getHeight()), GridItem::Span(bounds.getWidth())));
  }
  grid.performLayout(component()->getLocalBounds());
  for (std::size_t i = 0; i < _cell_contents.size(); i++)
    _cell_contents[i]->layout();
}

std::unique_ptr<grid_element>
create_grid_ui(
  plugin_controller* controller,
  std::vector<std::int32_t> const& row_distribution_relative, 
  std::vector<std::int32_t> const& column_distribution_relative)
{
  std::vector<Grid::TrackInfo> row_distribution;
  std::vector<Grid::TrackInfo> column_distribution;
  for(std::size_t i = 0; i < row_distribution_relative.size(); i++)
    row_distribution.push_back(Grid::TrackInfo(Grid::Fr(row_distribution_relative[i])));
  for (std::size_t i = 0; i < column_distribution_relative.size(); i++)
    column_distribution.push_back(Grid::TrackInfo(Grid::Fr(column_distribution_relative[i])));
  return create_grid_ui(controller, row_distribution, column_distribution);
}

std::unique_ptr<ui_element>
create_param_ui(
  plugin_controller* controller, std::unique_ptr<ui_element>&& label_or_icon, std::int32_t part_type, 
  std::int32_t part_index, std::int32_t param_index, edit_type edit_type, tooltip_type tooltip_type, lnf_factory lnf_factory, bool force_toggle_on, std::int32_t hslider_cols)
{
  if (edit_type == edit_type::hslider)
  {
    assert(hslider_cols != -1);
    if(hslider_cols == -1) return {};
    auto result = create_grid_ui(controller, 1, hslider_cols);
    result->add_cell(std::move(label_or_icon), 0, 0);
    result->add_cell(create_param_edit_ui(controller, part_type, part_index, param_index, edit_type, tooltip_type, lnf_factory, force_toggle_on), 0, 1, 1, hslider_cols - 1);
    return result;
  }
  else
  {
    auto auto_rest = Grid::TrackInfo(Grid::Fr(1));
    auto fixed_label_height = Grid::TrackInfo(Grid::Px(get_param_label_total_height(controller)));
    auto result = create_grid_ui(controller, { auto_rest, fixed_label_height }, { auto_rest });
    result->add_cell(create_param_edit_ui(controller, part_type, part_index, param_index, edit_type, tooltip_type, lnf_factory, force_toggle_on), 0, 0);
    result->add_cell(std::move(label_or_icon), 1, 0);
    return result;
  }
}

std::unique_ptr<ui_element>
create_labeled_param_ui(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  edit_type edit_type, label_type label_type, tooltip_type tooltip_type, lnf_factory lnf_factory, bool force_toggle_on, std::int32_t hslider_cols)
{
  auto justification = edit_type == edit_type::hslider? juce::Justification::centredRight: juce::Justification::centred;
  auto label = create_param_label_ui(controller, part_type, part_index, param_index, label_type, justification);
  return create_param_ui(controller, std::move(label), part_type, part_index, param_index, edit_type, tooltip_type, lnf_factory, force_toggle_on, hslider_cols);
}

std::unique_ptr<ui_element>
create_iconed_param_ui(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  edit_type edit_type, icon_type icon_type, tooltip_type tooltip_type, lnf_factory lnf_factory, bool force_toggle_on)
{
  auto icon = create_param_icon_ui(controller, icon_type);
  return create_param_ui(controller, std::move(icon), part_type, part_index, param_index, edit_type, tooltip_type, lnf_factory, force_toggle_on);
}

std::unique_ptr<ui_element>
create_iconed_param_ui(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, std::int32_t param_index, 
  edit_type edit_type, icon_selector icon_selector, tooltip_type tooltip_type, lnf_factory lnf_factory, bool force_toggle_on)
{
  auto icon = create_param_icon_ui(controller, part_type, part_index, param_index, icon_selector);
  return create_param_ui(controller, std::move(icon), part_type, part_index, param_index, edit_type, tooltip_type, lnf_factory, force_toggle_on);
}

std::unique_ptr<ui_element>
create_part_group_ui(plugin_controller* controller, std::unique_ptr<group_label_element>&& label, std::unique_ptr<ui_element>&& content)
{
  std::vector<Grid::TrackInfo> rows;
  std::vector<Grid::TrackInfo> cols;
  auto label_height = get_group_label_total_height(controller);
  if (label->vertical())
  {
    rows.push_back(Grid::TrackInfo(Grid::Fr(1)));
    cols.push_back(Grid::TrackInfo(Grid::Px(label_height)));
    cols.push_back(Grid::TrackInfo(Grid::Fr(1)));
  }
  else
  {
    rows.push_back(Grid::TrackInfo(Grid::Px(label_height)));
    rows.push_back(Grid::TrackInfo(Grid::Fr(1)));
    cols.push_back(Grid::TrackInfo(Grid::Fr(1)));
  }
  auto result = create_grid_ui(controller, rows, cols);
  if (label->vertical())
  {
    result->add_cell(std::move(label), 0, 0);
    result->add_cell(std::move(content), 0, 1);
  }
  else
  {
    result->add_cell(std::move(label), 0, 0);
    result->add_cell(std::move(content), 1, 0);
  }
  return result;
}

std::unique_ptr<ui_element>
create_part_single_ui(
  plugin_controller* controller, std::string const& header, std::int32_t selected_part_type, 
  bool vertical, selector_routing_dir routing_dir, std::unique_ptr<ui_element>&& part)
{
  auto selector_grid = create_grid_ui(controller, 1, 1);
  selector_grid->add_cell(create_selector_label_ui(controller, header, selected_part_type, 1, vertical, routing_dir, Justification::centred), 0, 0);
  if(!vertical)
  {
    auto selector_height = static_cast<std::int32_t>(std::ceil(get_selector_height(controller)));
    auto result = create_grid_ui(controller, { Grid::Px(selector_height), Grid::Fr(1) }, { Grid::Fr(1) });
    result->add_cell(create_part_group_container_ui(controller, std::move(selector_grid), container_selector_padding), 0, 0);
    result->add_cell(std::move(part), 1, 0);
    return result;
  } else {
    auto selector_width = static_cast<std::int32_t>(std::ceil(get_selector_height(controller))) + get_selector_vlabel_pad(controller);
    auto result = create_grid_ui(controller, { Grid::Fr(1) }, { Grid::Px(selector_width), Grid::Fr(1) });
    result->add_cell(create_part_group_container_ui(controller, std::move(selector_grid), container_selector_padding), 0, 0);
    result->add_cell(std::move(part), 0, 1);
    return result;
  }
}

std::unique_ptr<ui_element>
create_part_selector_ui(
  plugin_controller* controller, std::string const& header, std::int32_t selector_part_type,
  std::int32_t selector_param_index, std::int32_t selected_part_type, std::int32_t label_columns, 
  std::int32_t selector_columns, bool label_right, selector_routing_dir routing_dir, Justification justification,
  std::vector<std::unique_ptr<ui_element>>&& selected_parts)
{
  inf::base::part_id selector_id = { selector_part_type, 0 };
  auto label_col = label_right? selector_columns: 0;
  auto selector_col = label_right? 0: label_columns;
  auto selector_grid = create_grid_ui(controller, 1, selector_columns + label_columns);
  selector_grid->add_cell(create_selector_label_ui(controller, header, selected_part_type, static_cast<std::int32_t>(selected_parts.size()), false, routing_dir, justification), 0, label_col, 1, label_columns);
  auto selector_bar = create_selector_bar(controller, selector_id, selector_param_index, selected_part_type);
  auto selector_bar_ptr = selector_bar.get();
  for(std::int32_t i = 0; i < static_cast<std::int32_t>(selected_parts.size()); i++)
    selector_bar->add_header(std::to_string(i + 1));
  selector_grid->add_cell(std::move(selector_bar), 0, selector_col, 1, selector_columns);

  std::vector<ui_element*> tabs;
  auto selector_height = static_cast<std::int32_t>(std::ceil(get_selector_height(controller)));
  auto result = create_grid_ui(controller, { Grid::Px(selector_height), Grid::Fr(1) }, { Grid::Fr(1) });
  result->add_cell(create_part_group_container_ui(controller, std::move(selector_grid), container_selector_padding), 0, 0);
  for(std::size_t i = 0; i < selected_parts.size(); i++)
  {
    tabs.push_back(selected_parts[i].get());
    result->add_cell(std::move(selected_parts[i]), 1, 0);
  }

  std::int32_t selected_index = controller->state()[controller->topology()->param_index(selector_id, selector_param_index)].discrete;
  for(std::int32_t i = 0; i < static_cast<std::int32_t>(selected_parts.size()); i++)
    tabs[i]->initially_visible(i == selected_index);
  selector_bar_ptr->set_extra_listener(std::make_unique<selector_extra_listener>(
    std::function<void(inf_selector_bar*)>([tabs](inf_selector_bar* bar) {
      for(std::int32_t i = 0; i < static_cast<std::int32_t>(tabs.size()); i++)
        tabs[i]->component()->setVisible(i == bar->getCurrentTabIndex()); })));
  return result;
}

std::unique_ptr<ui_element>
create_factory_preset_ui(
  plugin_controller* controller, lnf_factory lnf_factory)
{
  std::vector<std::string> items;
  std::int32_t initial_index = -1;
  File file(File::getSpecialLocation(File::currentExecutableFile));
  auto presets = controller->factory_presets(file.getFullPathName().toStdString());
  for(std::size_t i = 0; i < presets.size(); i++)
  {
    items.push_back(presets[i].name);
    if(presets[i].name == controller->get_factory_preset()) 
      initial_index = static_cast<std::int32_t>(i);
  }
  return create_action_dropdown_ui(controller, initial_index, "Factory preset", items, 
    [controller, presets, lnf_factory](juce::ComboBox* dropdown) {
      std::int32_t selected_index = dropdown->getSelectedItemIndex();
      if (0 <= selected_index && selected_index < static_cast<std::int32_t>(presets.size()))
      {
        controller->load_plugin_preset(presets[selected_index].path);
        controller->set_factory_preset(presets[selected_index].name);
      } },  
    [controller, presets](juce::ComboBox* combo) {
      for (std::size_t i = 0; i < presets.size(); i++)
        if (presets[i].name == controller->get_factory_preset())
        {
          combo->setSelectedItemIndex(static_cast<std::int32_t>(i), dontSendNotification);
          return;
        }
      combo->setSelectedId(0, dontSendNotification); });
}

std::unique_ptr<ui_element>
create_theme_selector_ui(
  plugin_controller* controller, lnf_factory lnf_factory)
{
  std::vector<std::string> items;
  std::int32_t initial_index = -1;
  File file(File::getSpecialLocation(File::currentExecutableFile));
  auto themes = controller->themes(file.getFullPathName().toStdString());
  std::string theme = controller->get_theme();
  for (std::size_t i = 0; i < themes.size(); i++)
  {
    items.push_back(themes[i].name);
    if (themes[i].name == theme)
      initial_index = static_cast<std::int32_t>(i);
  }
  return create_action_dropdown_ui(controller, initial_index, "Theme", items,
    [controller, themes, lnf_factory](juce::ComboBox* dropdown) {
      std::int32_t selected_index = dropdown->getSelectedItemIndex();
      if (0 <= selected_index && selected_index < static_cast<std::int32_t>(themes.size()))
      {
        controller->set_theme(themes[selected_index].name);
        controller->reload_editor(-1);
      }
    }, 
    [controller, themes](juce::ComboBox* combo) {
      std::string theme = controller->get_theme();
      for (std::size_t i = 0; i < themes.size(); i++)
        if (themes[i].name == theme)
        {
          combo->setSelectedItemIndex(static_cast<std::int32_t>(i), dontSendNotification);
          return;
        }
      combo->setSelectedId(0, dontSendNotification);
    });
}

std::unique_ptr<ui_element>
create_ui_size_ui(
  plugin_controller* controller, lnf_factory lnf_factory)
{
  std::int32_t initial_index = -1;
  std::vector<std::string> size_names;
  std::string ui_size = controller->get_ui_size();
  auto props = controller->get_editor_properties();
  for(std::size_t i = 0; i < props.ui_size_names.size(); i++)
  {
    size_names.push_back(props.ui_size_names[i]);
    if (size_names[i] == ui_size)
      initial_index = static_cast<std::int32_t>(i);
  }
  return create_action_dropdown_ui(controller, initial_index, "UI Size", size_names, 
    [controller, size_names](juce::ComboBox* dropdown) {
      std::int32_t selected_index = dropdown->getSelectedItemIndex();
      if (0 <= selected_index && selected_index < static_cast<std::int32_t>(size_names.size()))
      {
        controller->set_ui_size(size_names[selected_index]);
        controller->reload_editor(controller->ui_size_to_editor_width(selected_index));
      }
    }, 
    [size_names, controller](juce::ComboBox* combo){
      std::string ui_size = controller->get_ui_size();
      for (std::size_t i = 0; i < size_names.size(); i++)
        if (size_names[i] == ui_size)
        {
          combo->setSelectedItemIndex(static_cast<std::int32_t>(i), dontSendNotification);
          return;
        }
      combo->setSelectedId(0, dontSendNotification);
    });
}

void
show_ok_box(
  inf::base::plugin_controller* controller,
  std::string const& header, std::unique_ptr<inf_look_and_feel>&& lnf)
{
  dialog_box_state* state = new dialog_box_state;
  state->lnf = std::move(lnf);
  state->content = create_grid_ui(controller, 2, 2);
  state->window = std::make_unique<AlertWindow>("", "", MessageBoxIconType::NoIcon);
  state->content->add_cell(create_label_ui(controller, header, Justification::left, dialog_font_header_height, inf_look_and_feel::colors::alert_text), 0, 0, 1, 2);
  state->content->add_cell(create_button_ui(controller, "OK", Justification::centred, [state]() {
    state->window->exitModalState();
    delete state; }), 1, 1);
  run_dialog_box(state, 180, 60);
}

void
save_preset_file(
  inf::base::plugin_controller* controller, lnf_factory lnf_factory)
{
  auto flags = FileBrowserComponent::saveMode;
  auto state = create_preset_file_box_state(controller, lnf_factory, "Save preset", flags);
  auto saved = [state, lnf_factory](int result)
  {
    if (result != 0)
    {
      std::string dot_extension = std::string(".") + state->controller->plugin_preset_file_extension();
      auto selected = state->browser->getSelectedFile(0).getFullPathName().toStdString();
      if (!selected.ends_with(dot_extension)) selected += dot_extension;
      state->controller->save_plugin_preset(selected);
      show_ok_box(state->controller, "Preset file saved.", lnf_factory(state->controller));
      state->controller->set_last_directory(state->browser->getSelectedFile(0).getParentDirectory().getFullPathName().toStdString());
    }
    state->box->exitModalState();
    delete state;
  };
  auto current_window = static_cast<juce::Component*>(controller->current_editor_window());
  state->box->setOpaque(true);
  state->box->addToDesktop(0, nullptr);
  state->box->center_around(current_window);
  state->box->enterModalState(true, ModalCallbackFunction::create(saved), false);
}

void
load_preset_file(
  inf::base::plugin_controller* controller, lnf_factory lnf_factory)
{  
  auto props = controller->get_editor_properties();
  auto flags = FileBrowserComponent::openMode | FileBrowserComponent::filenameBoxIsReadOnly;
  auto state = create_preset_file_box_state(controller, lnf_factory, "Load preset", flags);
  auto selected = [state, controller, lnf_factory, props](int result)
  {
    std::string old_theme = state->controller->get_theme();
    std::string old_size = state->controller->get_ui_size();
    if (result != 0)
    {
      auto selected = state->browser->getSelectedFile(0); 
      if (!state->controller->load_plugin_preset(selected.getFullPathName().toStdString()))
        show_ok_box(state->controller, "Could not load preset file.", lnf_factory(state->controller));        
      state->controller->set_last_directory(state->browser->getSelectedFile(0).getParentDirectory().getFullPathName().toStdString());
    }
    state->box->exitModalState();
    delete state;
    if (result != 0 && (controller->get_theme() != old_theme || controller->get_ui_size() != old_size))
    {
      std::size_t size_index = 0;
      auto size_names = props.ui_size_names;
      for(std::size_t i = 0; i < size_names.size(); i++)
        if(size_names[i] == controller->get_ui_size())
          size_index = i;
      controller->reload_editor(controller->ui_size_to_editor_width(static_cast<std::int32_t>(size_index)));
    }
  };
  auto current_window = static_cast<juce::Component*>(controller->current_editor_window());
  state->box->setOpaque(true);
  state->box->addToDesktop(0, nullptr);
  state->box->center_around(current_window);
  state->box->enterModalState(true, ModalCallbackFunction::create(selected), false);
}

void
show_confirm_box(
  inf::base::plugin_controller* controller, std::string const& header,
  lnf_factory lnf_factory, confirmed_callback confirmed, cancelled_callback cancelled)
{
  confirm_box_state* state = new confirm_box_state;
  state->confirmed = confirmed;
  state->cancelled = cancelled;
  state->controller = controller;
  state->lnf = lnf_factory(controller);
  state->content = create_grid_ui(controller, 3, 2);
  state->window = std::make_unique<AlertWindow>("", "", MessageBoxIconType::NoIcon);
  state->content->add_cell(create_label_ui(controller, header, Justification::left, dialog_font_header_height, inf_look_and_feel::colors::alert_header_text), 0, 0, 1, 2);
  state->content->add_cell(create_label_ui(controller, "Are you sure?", Justification::left, dialog_font_height, inf_look_and_feel::colors::alert_text), 1, 0, 1, 2);
  state->content->add_cell(create_button_ui(controller, "OK", Justification::centred, [state]() {
    state->window->exitModalState();
    state->confirmed(state->controller);
    delete state; }), 2, 0);
  state->content->add_cell(create_button_ui(controller, "Cancel", Justification::centred, [state]() {
    state->window->exitModalState();
    state->cancelled();
    delete state; }), 2, 1);
  run_dialog_box(state, 180, 90);
}

} // namespace inf::base::ui