#ifndef INF_VST_UI_BOOTSTRAP_HPP
#define INF_VST_UI_BOOTSTRAP_HPP

#include <pluginterfaces/base/funknown.h>
#include <inf.base/topology/topology_info.hpp>

using namespace inf::base;

extern "C"
{
  topology_info* inf_vst_create_topology_impl();
  SMTG_EXPORT_SYMBOL topology_info* inf_vst_create_topology();
}

#endif // INF_VST_UI_BOOTSTRAP_HPP