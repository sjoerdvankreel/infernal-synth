#include <inf.vst.tool/diff/checker.hpp>
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
    return check_plugin(argv[2], argv[3]);
  }

  // Diffs 2 vst preset files loaded in 2 different plugin versions.
  if (!strcmp(argv[1], "--diff-preset-file"))
  {
    if (argc != 6)
      return std::cout <<
      "Usage: infernal.vst.tool --diff-preset-file "
      "<full\\path\\to\\plugin1.vst3> (dll) <full\\path_to\\preset1.vstpreset> (preset) "
      "<full\\path\\to\\plugin2.vst3> (dll) <full\\path_to\\preset2.vstpreset> (preset)\n", 1;
    return check_preset_file(argv[2], argv[3], argv[4], argv[5]);
  }

  return std::cout << "Unknown option " << argv[1] << ".\n", 1;
}
