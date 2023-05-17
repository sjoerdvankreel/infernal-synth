#include <inf.base.ui/controls/graph_plot.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>
#include <algorithm>

using namespace juce;

namespace inf::base::ui {

static float const graph_bpm = 120.0f;
static float const graph_sample_rate = 48000.0f;

graph_processor* 
inf_graph_plot::processor()
{
  if (_processor) return _processor.get();
  _processor = _controller->topology()->create_graph_processor(_part_id, _graph_type);
  return _processor.get();
}

void 
inf_graph_plot::paint(juce::Graphics& g)
{
  float const graph_pad = 3.0f;
  auto& lnf = dynamic_cast<inf_look_and_feel&>(getLookAndFeel());
  auto bounds = with_container_padding(getLocalBounds()).toFloat();
  auto graph_bounds = bounds.expanded(-graph_pad);

  // fill
  lnf.fill_gradient_rounded_rectangle(g, *this, bounds, 
    inf_look_and_feel::colors::part_graph_fill_low, 
    inf_look_and_feel::colors::part_graph_fill_high, container_radius, 0.5f);

  // grid
  g.setColour(findColour(inf_look_and_feel::colors::part_graph_grid));
  float row_height = bounds.getHeight() / (graph_grid_hcount + 1);
  for(int i = 1; i <= graph_grid_hcount; i++)
    g.fillRect(bounds.getX(), bounds.getY() + i * row_height, bounds.getWidth(), 1.0f);
  float col_count = bounds.getWidth() / row_height;
  std::int32_t grid_vcount = static_cast<std::int32_t>(std::round(col_count));
  float col_width = bounds.getWidth() / col_count;
  for (int i = 1; i <= grid_vcount; i++)
    g.fillRect(bounds.getX() + i * col_width, bounds.getY(), 1.0f, bounds.getHeight());

  // plot data
  bool bipolar;
  param_value const* state = _controller->state();
  //float opacity = _processor->opacity(state);
  std::vector<graph_point> const& graph_data = processor()->plot(
    state, graph_sample_rate,
    static_cast<std::int32_t>(graph_bounds.getWidth()),
    static_cast<std::int32_t>(graph_bounds.getHeight()),
    bipolar);

  if (graph_data.size() != 0)
  {
    Path path;
    float base_y = bipolar ? 0.5f : 1.0f;
    path.startNewSubPath(graph_bounds.getX() + graph_data[0].x, graph_bounds.getHeight() * base_y);
    for (std::size_t i = 1; i < graph_data.size(); i++)
      path.lineTo(graph_bounds.getX() + graph_data[i].x, graph_bounds.getHeight() - graph_data[i].y);
    path.lineTo(graph_bounds.getX() + graph_bounds.getWidth(), bounds.getY() + graph_bounds.getHeight());
    g.setColour(findColour(inf_look_and_feel::colors::part_graph_line));
    g.strokePath(path, PathStrokeType(1.0f));
  }

  // outline
  lnf.stroke_gradient_rounded_rectangle(g, *this, bounds, 
    inf_look_and_feel::colors::part_graph_outline_low, 
    inf_look_and_feel::colors::part_graph_outline_high, container_radius, 0.5f, container_outline_size);
}

} // namespace inf::base::ui