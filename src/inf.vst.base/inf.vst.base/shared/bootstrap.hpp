#ifndef SVN_VST_BASE_SHARED_BOOTSTRAP_HPP
#define SVN_VST_BASE_SHARED_BOOTSTRAP_HPP

#include <pluginterfaces/base/funknown.h>
#include <inf.base/topology/topology_info.hpp>

using namespace inf::base;

extern "C"
{
  topology_info* inf_vst_create_topology_impl();
  SMTG_EXPORT_SYMBOL topology_info* inf_vst_create_topology();
}

#endif // SVN_VST_BASE_SHARED_BOOTSTRAP_HPP