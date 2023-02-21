#ifndef SVN_VST_BASE_SHARED_BOOTSTRAP_HPP
#define SVN_VST_BASE_SHARED_BOOTSTRAP_HPP

#include <pluginterfaces/base/funknown.h>
#include <svn.base/topology/topology_info.hpp>

using namespace svn::base;

extern "C"
{
  topology_info* svn_vst_create_topology_impl();
  SMTG_EXPORT_SYMBOL topology_info* svn_vst_create_topology();
}

#endif // SVN_VST_BASE_SHARED_BOOTSTRAP_HPP