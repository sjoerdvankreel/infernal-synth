#include <inf.vst.tool/diff/plugin/checker.hpp>
#include <inf.vst.tool/ui/generator/generator.hpp>

#include <cstring>
#include <iostream>

using namespace inf::vst::tool::ui;
using namespace inf::vst::tool::diff;

int
main(int argc, char** argv)
{
  if(argc < 2) 
    return std::cout << "Usage: infernal.vst.tool <options>\n", 1;

  // Generate .uidesc from topology.
  if (!strcmp(argv[1], "--generate-ui"))
  {
    if(argc != 4)
      return std::cout << 
        "Usage: infernal.vst.tool --generate-ui " 
        "<full\\path\\to\\plugin.vst3> (dll) <full\\path\\to\\file.uidesc> (json)\n", 1;
    return write_vstgui_json(argv[2], argv[3]);
  }

  // Diffs different plugin versions.
  if (!strcmp(argv[1], "--diff-plugin"))
  {
    if (argc != 4)
      return std::cout << 
        "Usage: infernal.vst.tool --diff-plugin " 
        "<full\\path\\to\\plugin1.vst3> (dll) <full\\path\\to\\plugin2.vst3> (dll)\n", 1;
    return plugin::check(argv[2], argv[3]);
  }

  return std::cout << "Unknown option " << argv[1] << ".\n", 1;
}
