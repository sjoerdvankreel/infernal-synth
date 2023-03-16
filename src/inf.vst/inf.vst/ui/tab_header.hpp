#ifndef INF_VST_UI_TAB_HEADER_HPP
#define INF_VST_UI_TAB_HEADER_HPP

#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>
#include <vstgui/uidescription/viewcreator/viewcontainercreator.h>

#include <string>
#include <vector>
#include <cstdint>

namespace inf::vst {

// Tab header control without bitmaps.
class tab_header : public VSTGUI::CParamDisplay
{
  std::string const _title;
  std::vector<std::string> const _items;
  inf::base::tab_header_ui_colors const _colors;
  std::vector<double> _last_draw_boundaries;

  std::int32_t get_index() const;
  void set_index(std::int32_t val);

public:
  CLASS_METHODS(tab_header, CControl)
  void draw(VSTGUI::CDrawContext* context) override;
  void onMouseDownEvent(VSTGUI::MouseDownEvent& event) override;

  tab_header(
    std::string const& title,
    std::vector<std::string> const& items, 
    inf::base::tab_header_ui_colors const& colors);
};

// VSTGUI control factory.
class tab_header_creator : public VSTGUI::ViewCreatorAdapter
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "inf_tab_header"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCControl; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace inf::vst
#endif // INF_VST_UI_TAB_HEADER_HPP