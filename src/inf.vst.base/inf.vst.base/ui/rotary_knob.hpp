#ifndef INF_VST_BASE_UI_ROTARY_KNOB_HPP
#define INF_VST_BASE_UI_ROTARY_KNOB_HPP

#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/viewcreator/knobcreator.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>

namespace inf::vst::base {

// Custom drawn knob, follows color palette.
class rotary_knob : 
public VSTGUI::CKnobBase
{
  inf::base::knob_ui_colors const _colors;
public:
  CLASS_METHODS(rotary_knob, CKnobBase)
  void draw(VSTGUI::CDrawContext* context) override;
public:
  explicit rotary_knob(inf::base::knob_ui_colors const& colors):
  CKnobBase(VSTGUI::CRect(0, 0, 0, 0), nullptr, -1, nullptr), _colors(colors) {}
};

// VSTGUI knob factory.
class rotary_knob_creator :
public VSTGUI::UIViewCreator::KnobBaseCreator
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "inf_rotary_knob"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCControl; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace inf::vst::base
#endif // INF_VST_BASE_UI_ROTARY_KNOB_HPP