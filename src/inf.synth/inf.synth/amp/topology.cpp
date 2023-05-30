#include <inf.synth/amp/topology.hpp>
#include <inf.base/topology/part_descriptor.hpp>

using namespace inf::base;

namespace inf::synth {

param_descriptor const
vamp_params[amp_param::count] =
{
  { "{5A2DF5BA-7D6F-4053-983E-AA6DC5084373}", { { "Gain", "Gain" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{86782F43-7079-47BE-9C7F-8BF6D12A0950}", { { "Bal", "Balance" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
};

param_descriptor const 
gamp_params[amp_param::count] =
{
  { "{536EBE78-85C2-461F-A3E5-2F7ADA11577C}", { { "Gain", "Gain" }, "dB", param_kind::continuous, decibel_bounds(2.0f) } },
  { "{7917BE01-867D-490B-BD72-3CCE267CE872}", { { "Bal", "Balance" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1) } },
};
 
} // namespace inf::synth 