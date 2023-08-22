#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_controller.hpp>

namespace inf::base::format::clap
{

clap_controller::
clap_controller():
plugin_controller(create_topology()) {}

} // inf::base::format::clap