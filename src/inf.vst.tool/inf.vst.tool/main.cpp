#include <inf.vst.tool/ui/generator/generator.hpp>
#include <cstring>
#include <iostream>

using namespace inf::vst::tool::ui;

int
main(int argc, char** argv)
{
  if(argc < 2) 
    return std::cout << "Usage: infernal.vst.tool <options>\n", 1;
  if (!strcmp(argv[1], "--generate-ui"))
  {
    if(argc != 4)
      return std::cout << "Usage: infernal.vst.tool --generate-ui <full\\path\\to\\plugin.vst3> (dll) <full\\path\\to\\file.uidesc> (json)\n", 1;
    return write_vstgui_json(argv[2], argv[3]);
  }
  return std::cout << "Unknown option " << argv[1] << ".\n", 1;
}
