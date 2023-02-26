#include <inf.base/shared/support.hpp>
#include <inf.synth/cv_bank/state.hpp>
#include <inf.synth/cv_bank/topology.hpp>

using namespace inf::base;

namespace inf::synth {

cv_bank_data const cv_bank_data::voice =
{
  part_type::vcv_bank,
  vcv_bank_count,
  vcv_bank_route_count,
  vcv_route_output_total_count,
  vcv_route_output_mapping,
  vcv_route_output_target_counts,
  vcv_route_output_target_mapping,

  multi_list_table_init_in(vcv_route_output_counts, vcv_route_output::count),
  multi_list_table_init_out(vcv_route_output_counts, vcv_route_output::count),
  multi_list_table_init_in(vcv_route_input_counts, vcv_route_input::count),
  multi_list_table_init_out(vcv_route_input_counts, vcv_route_input::count),
  zip_list_table_init_in(vcv_route_output_counts, vcv_route_output_target_counts, vcv_route_output::count),
};

cv_bank_data const cv_bank_data::global = 
{ 
  part_type::gcv_bank,
  gcv_bank_count,
  gcv_bank_route_count,
  gcv_route_output_total_count,
  gcv_route_output_mapping,
  gcv_route_output_target_counts,
  gcv_route_output_target_mapping,

  multi_list_table_init_in(gcv_route_output_counts, gcv_route_output::count),
  multi_list_table_init_out(gcv_route_output_counts, gcv_route_output::count),
  multi_list_table_init_in(gcv_route_input_counts, gcv_route_input::count),
  multi_list_table_init_out(gcv_route_input_counts, gcv_route_input::count),
  zip_list_table_init_in(gcv_route_output_counts, gcv_route_output_target_counts, gcv_route_output::count)
};

cv_bank_state::
cv_bank_state(topology_info const* topology, std::int32_t max_sample_count):
amt(max_sample_count), scale(max_sample_count), offset(max_sample_count), 
in_modified(max_sample_count), clamped(), out(), unmodulated(),
unmodulated_bipolar_range(), key(max_sample_count), 
key_inv(max_sample_count), velo(max_sample_count),
venv(), vlfo(), glfo(), glfo_hold(), 
gcv_bi(), gcv_uni(), gcv_bi_hold(), gcv_uni_hold()
{
  for (std::int32_t i = 0; i < venv_count; i++)
    venv[i] = cv_bank_storage(max_sample_count);
  for (std::int32_t i = 0; i < vlfo_count; i++)
    vlfo[i] = cv_bank_storage(max_sample_count);
  for (std::int32_t i = 0; i < glfo_count; i++)
    glfo[i] = cv_bank_storage(max_sample_count);
  for (std::int32_t i = 0; i < glfo_count; i++)
    glfo_hold[i] = cv_bank_storage(max_sample_count);
  for (std::int32_t i = 0; i < master_gcv_count; i++)
  {
    gcv_bi[i] = cv_bank_storage(max_sample_count);
    gcv_uni[i] = cv_bank_storage(max_sample_count);
    gcv_bi_hold[i] = cv_bank_storage(max_sample_count);
    gcv_uni_hold[i] = cv_bank_storage(max_sample_count);
  }

  // Make room for every part type, even if it's never modulated.
  // This is because we also handle transformation to dsp range here,
  // for example for the envelope parameters which are not modulated.
  std::int32_t max_param_count = 0;
  for (std::int32_t i = 0; i < part_type::count; i++)
    max_param_count = std::max(max_param_count, topology->static_parts[i].param_count);

  // This goes per-target.
  clamped.resize(max_param_count);
  
  // Set up contiguous buffers.
  out.resize(max_param_count, max_sample_count);
  unmodulated.resize(max_param_count, max_sample_count);
  unmodulated_bipolar_range.resize(max_param_count, max_sample_count);
}

} // namespace inf::synth 