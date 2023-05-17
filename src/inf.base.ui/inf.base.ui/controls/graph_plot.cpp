#include <inf.base.ui/controls/graph_plot.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>

using namespace juce;

namespace inf::base::ui {

void 
graph_plot::paint(juce::Graphics& g)
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

  // outline
  lnf.stroke_gradient_rounded_rectangle(g, *this, bounds, 
    inf_look_and_feel::colors::part_graph_outline_low, 
    inf_look_and_feel::colors::part_graph_outline_high, container_radius, 0.5f, container_outline_size);
}

} // namespace inf::base::ui