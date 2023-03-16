#ifndef INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP
#define INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP

#include <inf.base/processor/graph_processor.hpp>
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

  std::int32_t const _row_span;
  std::int32_t const _column_span;
  graph_ui_colors const _colors;
  std::unique_ptr<graph_processor> _processor;
public:
  void draw(VSTGUI::CDrawContext* context) override;
  graph_processor* processor() const { return _processor.get(); }
public:
  graph_plot(graph_ui_colors const& colors, std::int32_t row_span,
    std::int32_t column_span, std::unique_ptr<graph_processor>&& processor):
  VSTGUI::CView(VSTGUI::CRect(0, 0, 0, 0)), 
  _row_span(row_span), _column_span(column_span), 
  _colors(colors), _processor(std::move(processor)) { }
};

// VSTGUI graph factory.
class graph_plot_creator :
public VSTGUI::ViewCreatorAdapter
{
  topology_info const* _topology;
public:
  graph_plot_creator(topology_info const* topology) : _topology(topology) {}
  VSTGUI::IdStringPtr getViewName() const override { return "inf_graph_plot"; }
  VSTGUI::IdStringPtr getBaseViewName() const override { return VSTGUI::UIViewCreator::kCView; }
  VSTGUI::CView* create(VSTGUI::UIAttributes const& attrs, VSTGUI::IUIDescription const* desc) const override;
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_GRAPH_PLOT_HPP