#ifndef INF_BASE_TOPOLOGY_TOPOLOGY_INFO_HPP
#define INF_BASE_TOPOLOGY_TOPOLOGY_INFO_HPP

#include <inf.base/shared/state.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/param_descriptor.hpp>

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <utility>

namespace inf::base {

class audio_processor;
class graph_processor;

// To allow conversions.
struct stored_param_id
{
  std::int32_t io_type;
  std::string part_guid;
  std::string param_guid;
  std::int32_t part_index;
};

bool operator<(stored_param_id const& l, stored_param_id const& r);

// Runtime param info (e.g. lfo 2 rate).
struct param_info
{
  std::int32_t const part_index; // Index into runtime_part array, e.g. 0/1/2/3 when 2 oscillators + 2 lfos.
  std::string const runtime_name; // Runtime parameter name, e.g. "Lfo 2 rate".
  param_descriptor const* const descriptor; // Pointer to static parameter information.
};

// Runtime part info (e.g. osc 2).
struct part_info
{
  std::int32_t const type_index; // Part index within this type, e.g. 0/1 in case of 2 lfos.
  std::string const runtime_name; // Runtime name, e.g. "Lfo 1", "Lfo 2". Equals static name when part count is 1.
  std::int32_t const runtime_param_start; // Index into topology.
  part_descriptor const* const descriptor; // Pointer to static part information.
};

// Runtime part and parameter layout.
// Also acts as entry point into the plugin.
struct topology_info
{
protected:  
  // Output params must follow input params.
  topology_info(
    part_descriptor const* static_parts,
    std::int32_t part_count, std::int32_t max_notes);

public:
  part_descriptor const* static_parts = {}; // Static description of audio processor.
  std::int32_t static_part_count = 0; // Part count in static description.
  std::int32_t max_note_events = 0; // Not necessarily the same as polyphony.
  std::int32_t input_param_count = 0; // Runtime input params.
  std::int32_t output_param_count = 0; // Runtime output params.  
  std::int32_t continuous_param_count = 0; // Runtime sample-accurate automation parameter count.

  std::vector<part_info> parts = {}; // Runtime part descriptor array, e.g. osc 1, osc 2, lfo 1, lfo 2.
  std::vector<param_info> params = {}; // Runtime parameter descriptor array, e.g. osc 1 wave, osc 1 amp, osc 2 wave, etc.
  std::vector<std::vector<std::int32_t>> part_bounds = {}; // Runtime part bounds, e.g. bounds[part_type::osc][1] indexes osc 2 part.
  std::vector<std::vector<std::int32_t>> param_bounds = {}; // Runtime parameter bounds, e.g. bounds[part_type::osc][1] indexes osc 2 wave param.
  std::vector<part_id> inverse_bounds = {}; // Runtime inverse param bounds, e.g. bounds[123] = (part_type::env, 2).

  // Internally all parameters are handled by index, but
  // they are exposed to the vst3 binding as the hash of the parameter guid.
  // This allows us to drop/add/swap parameters and still have stored host automation
  // data work correctly when a track is loaded, as long as param guids are stable.
  std::vector<std::int32_t> param_index_to_id = {};
  std::map<std::int32_t, std::int32_t> param_id_to_index = {};

  // Set parameters to their default value as defined in param descriptor.
  void init_param_defaults(param_value* state, std::int32_t from, std::int32_t to) const;
  void init_all_param_defaults(param_value* state) const { init_param_defaults(state, 0, input_param_count); }
  void set_ui_value(param_value* state, std::int32_t part_type, std::int32_t part_index, std::int32_t param, char const* value) const;

  // Virtual so we can apply some sensible defaults for instrument/fx.
  virtual void init_clear_patch(param_value* state) const { init_all_param_defaults(state); }
  virtual void init_factory_preset(param_value* state) const { init_all_param_defaults(state); }

  // Metadata and conversions (backwards compat).
  virtual char const* vendor_name() const = 0;
  virtual char const* plugin_name() const = 0;
  virtual std::uint16_t version_major() const = 0;
  virtual std::uint16_t version_minor() const = 0;

  // To allow adjusting bounds etc.
  // Returns new target index for old stuff, or -1 if N/A.
  virtual std::int32_t try_move_stored_param(
    base::stored_param_id const& id, base::param_value old_value, 
    std::string const& old_text, std::uint16_t old_major, 
    std::uint16_t old_minor, bool& can_be_ignored) const
    { can_be_ignored = false; return -1; }
  virtual param_value convert_param(
    std::int32_t index, param_value old_value, std::string const& old_text,
    std::uint16_t old_major, std::uint16_t old_minor) const { return old_value; }

  // Sanity check.
  void state_check(param_value const* state) const;

  // Plugin entry.
  virtual ~topology_info() {}
  virtual bool is_instrument() const = 0;

  virtual std::unique_ptr<graph_processor>
  create_graph_processor(part_id id, std::int32_t graph_type) const = 0;
  virtual std::unique_ptr<audio_processor>
    create_audio_processor(param_value* state, std::int32_t* changed,
      float sample_rate, std::int32_t max_sample_count) const = 0;

  param_value ui_to_base_value(std::int32_t param_index, param_value ui_value) const;
  param_value base_to_ui_value(std::int32_t param_index, param_value base_value) const;
  std::int32_t param_start(part_id id) const { return param_bounds[id.type][id.index]; }
  std::int32_t param_index(part_id id, std::int32_t param) const { return param_start(id) + param; }
  part_info const& get_part_info(part_id id) const { return parts[part_bounds[id.type][id.index]]; }
  part_descriptor const& get_part_descriptor(part_id id) const { return *get_part_info(id).descriptor; }
  std::int32_t param_id(part_id id, std::int32_t param) const { return param_index_to_id[param_index(id, param)]; }
  param_info const& get_param_info(part_id id, std::int32_t param) const { return params[param_index(id, param)]; }
  param_descriptor const& get_param_descriptor(part_id id, std::int32_t param) const { return *get_param_info(id, param).descriptor; }
};

} // namespace inf::base
#endif // INF_BASE_TOPOLOGY_TOPOLOGY_INFO_HPP