#ifndef INF_PLUGIN_INFERNAL_SYNTH_SYNTH_TOPOLOGY_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_SYNTH_TOPOLOGY_HPP

#include <inf.base/plugin/audio_processor.hpp>
#include <inf.base/plugin/graph_processor.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <inf.plugin.infernal_synth/plugin.hpp>

#include <memory>
#include <cstdint>

namespace inf::plugin::infernal_synth {

inline std::int32_t constexpr master_gcv_count = 3;
extern base::param_descriptor const master_params[];
extern base::part_descriptor const part_descriptors[];
extern base::param_descriptor const edit_selector_params[];

// global topo
struct part_type_t { enum value { 
  vosc, veffect, geffect, vaudio_bank, gaudio_bank, 
  voice, master, vamp, gamp, venv, vlfo, glfo, vcv_bank, 
  gcv_bank, vcv_plot, gcv_plot, edit_selector, active, output, count }; };
typedef part_type_t::value part_type;

struct edit_selector_type_t { enum value {
  edit_voice, edit_global, count }; };
struct edit_selector_param_t { enum value {
  edit_type, count }; };
struct active_param_t { enum value { 
  vosc, veffect, geffect, venv, vlfo, glfo, count }; };
struct master_param_t { enum value {
  gcv1_uni, gcv1_bi, gcv2_uni, 
  gcv2_bi, gcv3_uni, gcv3_bi, 
  midi_mod_wheel, midi_ch_vol, 
  midi_ch_press, midi_pitch_bend, count }; };

typedef active_param_t::value active_param;
typedef master_param_t::value master_param;
typedef edit_selector_type_t::value edit_selector_type;
typedef edit_selector_param_t::value edit_selector_param;

// plugin entry
struct synth_topology :
public base::topology_info
{
  bool const _is_instrument;
  void init_fx_clear_patch(base::param_value* state) const;
  void init_fx_factory_preset(base::param_value* state) const;
  void set_default_instrument_cv(base::param_value* state) const;
  void init_instrument_clear_patch(base::param_value* state) const;
  void init_instrument_factory_preset(base::param_value* state) const;

public:
  synth_topology(
    base::part_descriptor const* static_parts, std::int32_t part_count, std::int32_t max_note_events, bool is_instrument) : 
    topology_info(static_parts, part_count, max_note_events), _is_instrument(is_instrument) {}

  void init_clear_patch(base::param_value* state) const override;
  void init_factory_preset(base::param_value* state) const override;

  bool is_instrument() const override { return _is_instrument; }
  char const* vendor_name() const override { return IPIS_VENDOR_NAME; }
  std::uint16_t version_major() const override { return IPIS_VERSION_MAJOR; }
  std::uint16_t version_minor() const override { return IPIS_VERSION_MINOR; }
  char const* plugin_name() const override { return _is_instrument ? IPIS_INST_NAME : IPIS_FX_NAME; }

  std::unique_ptr<base::audio_processor> 
  create_audio_processor(
    base::param_value* state, std::int32_t* changed, 
    float sample_rate, std::int32_t max_sample_count) const override;
  std::unique_ptr<base::graph_processor>
  create_graph_processor(base::part_id id, std::int32_t graph_type) const override;
  base::param_value convert_param(
    std::int32_t index, base::param_value old_value,
    std::string const& old_text, std::uint16_t old_major, std::uint16_t old_minor) const override;
  std::int32_t try_move_stored_param(
    base::stored_param_id const& id, base::param_value old_value, std::string const& old_text,
    std::uint16_t old_major, std::uint16_t old_minor, bool& can_be_ignored) const override;

private:
  std::unique_ptr<base::graph_processor>
    create_graph_processor_effect(base::part_id id, std::int32_t graph_type) const;
  std::unique_ptr<base::graph_processor>
    create_graph_processor_oscillator(base::part_id id, std::int32_t graph_type) const;
};
 
} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_SYNTH_TOPOLOGY_HPP