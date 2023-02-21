#ifndef SVN_VST_BASE_UI_PART_CONNECTOR_HPP
#define SVN_VST_BASE_UI_PART_CONNECTOR_HPP

#include <svn.base/topology/part_ui_descriptor.hpp>

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>

namespace svn::vst::base {

// Arrow pointing from one part to another.
class part_connector : 
public VSTGUI::CView
{
  svn::base::ui_color const _color;
  svn::base::connector_direction const _direction;
public:
  void draw(VSTGUI::CDrawContext* context) override;
public:
  part_connector(svn::base::connector_direction direction, svn::base::ui_color const& color):
  VSTGUI::CView(VSTGUI::CRect(0, 0, 0, 0)), _color(color), _direction(direction) {}
};

// VSTGUI connector factory.
class part_connector_creator :
public VSTGUI::ViewCreatorAdapter
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "svn_part_connector"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCView; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_UI_PART_CONNECTOR_HPP