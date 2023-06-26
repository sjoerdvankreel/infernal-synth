#ifndef INF_BASE_PLUGIN_GRAPH_PROCESSOR_HPP
#define INF_BASE_PLUGIN_GRAPH_PROCESSOR_HPP

#include <inf.base/plugin/state.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <vector>
#include <cstdint>

namespace inf::base {

// Renders pretty images.
class graph_processor
{
  base::part_id const _id;
  topology_info const* const _topology;

  std::vector<float> _plot_data;
  std::vector<graph_point> _graph_data;
  std::vector<base::param_value> _state_copy;
  std::vector<float>* const _raw_data_storage;

  // Of size parameter count (so nullptr at block parameter indices).
  std::vector<float*> _continuous_automation;
  // Of size continuous parameter count * max sample count, 
  // so does NOT align with total parameter count.
  std::vector<float> _continuous_automation_buffer;

  // Do the full dsp stuff without transforming to plot.
  std::vector<float> const& process_dsp(param_value const* state, float sample_rate);

protected:
  graph_processor(topology_info const* topology, part_id id, std::vector<float>* raw_data_storage);

public:
  virtual ~graph_processor() {}
  
  // This is the main entry point for the graph plot.
  // Data must be in [0, 1] even if bipolar.
  std::vector<graph_point> const& plot(
    param_value const* state, float sample_rate,
    std::int32_t width, std::int32_t height);

  part_id id() const { return _id; }
  topology_info const* topology() const { return _topology; }

  // Need to know upfront.
  virtual bool bipolar(param_value const* state) const = 0;
  // Allow dense graphs to soften a bit.
  virtual float opacity(param_value const* state) const { return 1.0f; }
  // Allow for some reuse between graphs. Repaint if that parameter changes?
  virtual bool needs_repaint(std::int32_t runtime_param) const = 0;
  // Transforms raw data to plot in (0, 1).
  virtual void dsp_to_plot(graph_plot_input const& input, std::vector<float>& plot) = 0;
  // Need to know size up front.
  virtual std::int32_t sample_count(param_value const* state, float sample_rate) const = 0;
  // Renders data in sample_count samples.
  virtual void process_dsp_core(block_input const& input, float* output, float sample_rate) = 0;
  // May be used to process graph specific requirements, e.g. disable modulation.
  virtual param_value transform_param(std::int32_t rt_index, param_value value) const { (void)rt_index; return value; }
};

inline graph_processor::
graph_processor(topology_info const* topology, part_id id, std::vector<float>* raw_data_storage):
_id(id), _topology(topology), _plot_data(),
_graph_data(), _state_copy(topology->params.size()), _raw_data_storage(raw_data_storage),
_continuous_automation(topology->input_param_count), _continuous_automation_buffer() {}

} // namespace inf::base
#endif // INF_BASE_PLUGIN_GRAPH_PROCESSOR_HPP