#include <inf.base.ui/controls/graph_plot.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>

#include <chrono>
#include <algorithm>

using namespace juce;

namespace inf::base::ui {

static float const graph_bpm = 120.0f;
static float const graph_sample_rate = 48000.0f;

void 
inf_graph_plot_timer::delayed_repaint_request() 
{
  _dirty = true;
  auto duration = std::chrono::system_clock::now().time_since_epoch();
  _paint_request_time = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

void 
inf_graph_plot_timer::timerCallback()
{ 
  if(_inside_callback || !_dirty) return;
  _inside_callback = true;
  auto duration = std::chrono::system_clock::now().time_since_epoch();
  std::uint64_t now_time = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  if(now_time - _paint_request_time >= timeout_millis)
  {
    _plot->repaint();
    _dirty = false;
  }
  _inside_callback = false;
}

inf_graph_plot::
~inf_graph_plot()
{
  _repaint_timer.stopTimer();
  _repaint_timer.plot(nullptr);
}

inf_graph_plot::
inf_graph_plot(inf::base::plugin_controller* controller, part_id part_id, std::int32_t graph_type) :
  juce::Component(), _part_id(part_id), _graph_type(graph_type), _controller(controller)
{
  _repaint_timer.plot(this);
  _repaint_timer.startTimer(inf_graph_plot_timer::timeout_millis);
}

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
  auto& lnf = dynamic_cast<inf_look_and_feel&>(getLookAndFeel());
  auto bounds = with_container_padding(getLocalBounds()).toFloat();

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
    static_cast<std::int32_t>(bounds.getWidth()),
    static_cast<std::int32_t>(bounds.getHeight()),
    bipolar);

  if (graph_data.size() != 0)
  {
    Path path;
    float base_y = bipolar ? 0.5f : 1.0f;
    path.startNewSubPath(bounds.getX() + graph_data[0].x, bounds.getHeight() * base_y);
    for (std::size_t i = 1; i < graph_data.size(); i++)
      path.lineTo(bounds.getX() + graph_data[i].x, bounds.getHeight() - graph_data[i].y);
    path.lineTo(bounds.getX() + bounds.getWidth(), bounds.getY() + bounds.getHeight());
    g.setColour(findColour(inf_look_and_feel::colors::part_graph_line));
    g.strokePath(path, PathStrokeType(1.0f));
  }

  // outline
  lnf.stroke_gradient_rounded_rectangle(g, *this, bounds, 
    inf_look_and_feel::colors::part_graph_outline_low, 
    inf_look_and_feel::colors::part_graph_outline_high, container_radius, 0.5f, container_outline_size);
}

} // namespace inf::base::ui