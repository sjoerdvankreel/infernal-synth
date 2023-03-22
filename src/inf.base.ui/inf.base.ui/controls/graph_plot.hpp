#ifndef INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP
#define INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP

#include <inf.base/plugin/graph_processor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vstgui/vstgui.h>
#include <vstgui/vstgui_uidescription.h>
#include <vstgui/uidescription/detail/uiviewcreatorattributes.h>

namespace inf::base::ui {

// Graph component backed by graph_descriptor and graph_processor.
class graph_plot : 
public VSTGUI::CView
{
  static inline float const bpm = 120.0f;
  static inline float const sample_rate = 48000.0f;

  part_id const _part_id;
  std::int32_t const _graph_type;
  graph_ui_colors const _colors;
  std::int32_t const _row_span;
  std::int32_t const _column_span;
  std::unique_ptr<graph_processor> _processor;
public:
  void draw(VSTGUI::CDrawContext* context) override;
  graph_processor* processor() const { return _processor.get(); }
public:
  graph_plot(part_id part_id, std::int32_t graph_type, graph_ui_colors const& colors, std::int32_t row_span, std::int32_t column_span):
  VSTGUI::CView(VSTGUI::CRect(0, 0, 0, 0)), _part_id(part_id), _graph_type(graph_type),
  _colors(colors), _row_span(row_span), _column_span(column_span) { }
};

// VSTGUI graph factory.
class graph_plot_creator :
public VSTGUI::ViewCreatorAdapter
{
public:
  VSTGUI::IdStringPtr getViewName() const override { return "inf_graph_plot"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCView; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP