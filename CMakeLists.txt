cmake_policy(SET CMP0091 NEW)
cmake_minimum_required(VERSION 3.21)

project(infernal-synth)
SET(INFERNAL_VERSION "1.3")

# Build config.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Dependency versions.
set(HIIR_SRC_DIR "lib/hiir_1.40")
set(JUCE_SRC_DIR "lib/JUCE_7.0.5")
set(VST3_SDK_SRC_DIR "lib/vst3sdk_3.7.8")

# Juce flags.
add_definitions(-DJUCE_USE_CURL=0)
add_definitions(-DJUCE_DISPLAY_SPLASH_SCREEN=0)
add_definitions(-DJUCE_MODAL_LOOPS_PERMITTED=0)
add_definitions(-DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)

# Juce linux dependencies.
if (NOT MSVC)
find_package(Freetype REQUIRED)
endif()

# VST3 flags.
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE=1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEVELOPMENT=1")

# Compiler flags.
if (MSVC)
add_definitions(-DJUCE_WINDOWS=1 -D_CRT_SECURE_NO_WARNINGS=1)
add_compile_options(/W4;/WX;/wd4100;/wd4388;/wd26451;/wd26498;/wd26812;/fp:fast;/arch:AVX;/external:W0)
else ()
add_definitions(-DJUCE_LINUX=1 -DJUCE_GUI_BASICS_INCLUDE_XHEADERS=1)
add_compile_options(-std=c++2a;-Wall;-Wextra;-Werror;-Wno-cpp;-Wno-sign-compare;-Wno-unused-parameter;-Wfatal-errors;-ffast-math;-march=native)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wl,--no-undefined -fvisibility=hidden -fvisibility-inlines-hidden -pthread")
endif()

# Juce core library.
file (GLOB JUCE_CORE_SRC "${JUCE_SRC_DIR}/modules/juce_core/juce_core.cpp")
add_library(lib.juce.core STATIC ${JUCE_CORE_SRC})
target_include_directories(lib.juce.core SYSTEM PRIVATE ${JUCE_SRC_DIR}/modules)

# Juce data structures library.
file (GLOB JUCE_DATA_STRUCTURES_SRC "${JUCE_SRC_DIR}/modules/juce_data_structures/juce_data_structures.cpp")
add_library(lib.juce.data_structures STATIC ${JUCE_DATA_STRUCTURES_SRC})
target_include_directories(lib.juce.data_structures SYSTEM PRIVATE ${JUCE_SRC_DIR}/modules)

# Juce graphics library.
file (GLOB JUCE_GRAPHICS_SRC "${JUCE_SRC_DIR}/modules/juce_graphics/juce_graphics.cpp")
add_library(lib.juce.graphics STATIC ${JUCE_GRAPHICS_SRC})
target_include_directories(lib.juce.graphics SYSTEM PRIVATE ${JUCE_SRC_DIR}/modules)
if (NOT MSVC)
target_link_libraries(lib.juce.graphics ${FREETYPE_LIBRARIES})
target_include_directories(lib.juce.graphics PRIVATE ${FREETYPE_INCLUDE_DIRS})
endif ()

# Juce events library.
file (GLOB JUCE_EVENTS_SRC "${JUCE_SRC_DIR}/modules/juce_events/juce_events.cpp")
add_library(lib.juce.events STATIC ${JUCE_EVENTS_SRC})
target_include_directories(lib.juce.events SYSTEM PRIVATE ${JUCE_SRC_DIR}/modules)

# Juce gui basics library.
file (GLOB JUCE_GUI_BASICS_SRC "${JUCE_SRC_DIR}/modules/juce_gui_basics/juce_gui_basics.cpp")
add_library(lib.juce.gui_basics STATIC ${JUCE_GUI_BASICS_SRC})
target_include_directories(lib.juce.gui_basics SYSTEM PRIVATE ${JUCE_SRC_DIR}/modules)
if (MSVC)
target_compile_options(lib.juce.gui_basics PRIVATE "$<$<CONFIG:Debug>:/bigobj>")
endif ()

# Vst3 sdk library.
file (GLOB VST3_BASE_SOURCE "${VST3_SDK_SRC_DIR}/base/source/*.cpp" "${VST3_SDK_SRC_DIR}/base/source/*.h")
file (GLOB VST3_BASE_THREAD_SOURCE "${VST3_SDK_SRC_DIR}/base/thread/source/*.cpp" "${VST3_SDK_SRC_DIR}/base/thread/source/*.h")
file (GLOB VST3_BASE_THREAD_INCLUDE "${VST3_SDK_SRC_DIR}/base/thread/include/*.cpp" "${VST3_SDK_SRC_DIR}/base/thread/include/*.h")
file (GLOB VST3_PLUGINTERFACES_GUI "${VST3_SDK_SRC_DIR}/pluginterfaces/gui/*.cpp" "${VST3_SDK_SRC_DIR}/pluginterfaces/gui/*.h")
file (GLOB VST3_PLUGINTERFACES_VST "${VST3_SDK_SRC_DIR}/pluginterfaces/vst/*.cpp" "${VST3_SDK_SRC_DIR}/pluginterfaces/vst/*.h")
file (GLOB VST3_PLUGINTERFACES_BASE "${VST3_SDK_SRC_DIR}/pluginterfaces/base/*.cpp" "${VST3_SDK_SRC_DIR}/pluginterfaces/base/*.h")
file (GLOB VST3_PUBLIC_SDK_SOURCE_VST "${VST3_SDK_SRC_DIR}/public.sdk/source/vst/*.cpp" "${VST3_SDK_SRC_DIR}/public.sdk/source/vst/*.h")
file (GLOB VST3_PUBLIC_SDK_SOURCE_COMMON "${VST3_SDK_SRC_DIR}/public.sdk/source/common/*.cpp" "${VST3_SDK_SRC_DIR}/public.sdk/source/common/*.h")
file (GLOB VST3_PUBLIC_SDK_SOURCE_VST_VSTGUIEDITOR "${VST3_SDK_SRC_DIR}/public.sdk/source/vst/vstguieditor.cpp")
file (GLOB VST3_PUBLIC_SDK_SOURCE_VST_VSTGUI_WIN32_BUNDLE_SUPPORT "${VST3_SDK_SRC_DIR}/public.sdk/source/vst/vstgui_win32_bundle_support.cpp")
list (REMOVE_ITEM VST3_PUBLIC_SDK_SOURCE_VST ${VST3_PUBLIC_SDK_SOURCE_VST_VSTGUIEDITOR})
list (REMOVE_ITEM VST3_PUBLIC_SDK_SOURCE_VST ${VST3_PUBLIC_SDK_SOURCE_VST_VSTGUI_WIN32_BUNDLE_SUPPORT})
if (MSVC)
file (GLOB VST3_PUBLIC_SDK_SOURCE_MAIN 
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/dllmain.cpp"
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/moduleinit.cpp"
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/moduleinit.h"
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/pluginfactory.cpp"
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/pluginfactory.h"
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/pluginfactory_constexpr.h")
else()
file (GLOB VST3_PUBLIC_SDK_SOURCE_MAIN 
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/linuxmain.cpp" 
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/moduleinit.cpp" 
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/moduleinit.h" 
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/pluginfactory.cpp" 
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/pluginfactory.h" 
  "${VST3_SDK_SRC_DIR}/public.sdk/source/main/pluginfactory_constexpr.h")
endif()
add_library(lib.vst3sdk STATIC 
  ${VST3_BASE_SOURCE} 
  ${VST3_BASE_THREAD_SOURCE} ${VST3_BASE_THREAD_INCLUDE}
  ${VST3_PLUGINTERFACES_GUI} ${VST3_PLUGINTERFACES_VST} ${VST3_PLUGINTERFACES_BASE} 
  ${VST3_PUBLIC_SDK_SOURCE_COMMON} ${VST3_PUBLIC_SDK_SOURCE_MAIN} ${VST3_PUBLIC_SDK_SOURCE_VST})
target_include_directories(lib.vst3sdk SYSTEM PRIVATE ${VST3_SDK_SRC_DIR})
if (MSVC)
target_compile_options(lib.vst3sdk PRIVATE /wd4245)
else ()
target_compile_options(lib.vst3sdk PRIVATE -w)
endif ()

# Inf base library.
set(INF_BASE_SRC_DIR "src/inf.base")
file(GLOB_RECURSE INF_BASE_SRC "${INF_BASE_SRC_DIR}/*.*")
add_library(infernal.base STATIC ${INF_BASE_SRC})
target_include_directories(infernal.base PRIVATE ${INF_BASE_SRC_DIR})
target_include_directories(infernal.base SYSTEM PRIVATE ${HIIR_SRC_DIR})
target_include_directories(infernal.base SYSTEM PRIVATE ${JUCE_SRC_DIR}/modules)
if (MSVC)
source_group(TREE "../../${INF_BASE_SRC_DIR}" FILES ${INF_BASE_SRC})
endif()

# Inf base ui library.
set(INF_BASE_UI_SRC_DIR "src/inf.base.ui")
file(GLOB_RECURSE INF_BASE_UI_SRC "${INF_BASE_UI_SRC_DIR}/*.*")
add_library(infernal.base.ui STATIC ${INF_BASE_UI_SRC})
target_include_directories(infernal.base.ui PRIVATE ${INF_BASE_UI_SRC_DIR} ${INF_BASE_SRC_DIR})
target_include_directories(infernal.base.ui SYSTEM PRIVATE ${JUCE_SRC_DIR}/modules)
if (MSVC)
source_group(TREE "../../${INF_BASE_UI_SRC_DIR}" FILES ${INF_BASE_UI_SRC})
endif()

# Inf base format vst3 library.
set(INF_BASE_FORMAT_VST3_SRC_DIR "src/inf.base.format.vst3")
file(GLOB_RECURSE INF_BASE_FORMAT_VST3_SRC "${INF_BASE_FORMAT_VST3_SRC_DIR}/*.*")
add_library(infernal.base.format.vst3 STATIC ${INF_BASE_FORMAT_VST3_SRC})
target_compile_definitions(infernal.base.format.vst3 PRIVATE $<$<CONFIG:Debug>:DEVELOPMENT=1>)
target_include_directories(infernal.base.format.vst3 PRIVATE ${INF_BASE_FORMAT_VST3_SRC_DIR} ${INF_BASE_SRC_DIR} ${INF_BASE_UI_SRC_DIR})
target_include_directories(infernal.base.format.vst3 SYSTEM PRIVATE ${VST3_SDK_SRC_DIR} ${JUCE_SRC_DIR}/modules)
if (MSVC)
source_group(TREE "../../${INF_BASE_FORMAT_VST3_SRC_DIR}" FILES ${INF_BASE_FORMAT_VST3_SRC})
target_compile_options(infernal.base.format.vst3 PRIVATE /external:I ${VST3_SDK_SRC_DIR})
endif()

# Inf plugin infernal synth library.
set(INF_PLUGIN_INFERNAL_SYNTH_SRC_DIR "src/inf.plugin.infernal_synth")
file(GLOB_RECURSE INF_PLUGIN_INFERNAL_SYNTH_SRC "${INF_PLUGIN_INFERNAL_SYNTH_SRC_DIR}/*.*")
add_library(infernal.plugin.infernal_synth STATIC ${INF_PLUGIN_INFERNAL_SYNTH_SRC})
target_include_directories(infernal.plugin.infernal_synth PRIVATE ${INF_PLUGIN_INFERNAL_SYNTH_SRC_DIR} ${INF_BASE_SRC_DIR})
target_include_directories(infernal.plugin.infernal_synth SYSTEM PRIVATE ${HIIR_SRC_DIR})
target_include_directories(infernal.plugin.infernal_synth SYSTEM PRIVATE ${JUCE_SRC_DIR}/modules)
if (MSVC)
source_group(TREE "../../${INF_PLUGIN_INFERNAL_SYNTH_SRC_DIR}" FILES ${INF_PLUGIN_INFERNAL_SYNTH_SRC})
endif()

# Inf plugin infernal synth ui library.
set(INF_PLUGIN_INFERNAL_SYNTH_UI_SRC_DIR "src/inf.plugin.infernal_synth.ui")
file(GLOB_RECURSE INF_PLUGIN_INFERNAL_SYNTH_UI_SRC "${INF_PLUGIN_INFERNAL_SYNTH_UI_SRC_DIR}/*.*")
add_library(infernal.plugin.infernal_synth.ui STATIC ${INF_PLUGIN_INFERNAL_SYNTH_UI_SRC})
target_include_directories(infernal.plugin.infernal_synth.ui PRIVATE ${INF_PLUGIN_INFERNAL_SYNTH_UI_SRC_DIR} 
  ${INF_PLUGIN_INFERNAL_SYNTH_SRC_DIR} ${INF_BASE_UI_SRC_DIR} ${INF_BASE_SRC_DIR})
target_include_directories(infernal.plugin.infernal_synth.ui SYSTEM PRIVATE ${JUCE_SRC_DIR}/modules)
if (MSVC)
source_group(TREE "../../${INF_PLUGIN_INFERNAL_SYNTH_UI_SRC_DIR}" FILES ${INF_PLUGIN_INFERNAL_SYNTH_UI_SRC})
endif()

# Inf plugin infernal synth format vst3fx and vst3i.
function(declare_inf_plugin_infernal_synth_format_vst3 target_name is_fx suffix platform)
set(INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_SRC_DIR "src/inf.plugin.infernal_synth.format.vst3")
file(GLOB_RECURSE INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_SRC "${INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_SRC_DIR}/*.*")
add_library(${target_name} SHARED ${INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_SRC})
target_compile_definitions(${target_name} PRIVATE -DIPISFV3_FX=${is_fx})
target_include_directories(${target_name} SYSTEM PRIVATE ${VST3_SDK_SRC_DIR} ${JUCE_SRC_DIR}/modules)
target_include_directories(${target_name} PRIVATE ${INF_BASE_SRC_DIR} ${INF_BASE_UI_SRC_DIR} ${INF_PLUGIN_INFERNAL_SYNTH_SRC_DIR}
  ${INF_PLUGIN_INFERNAL_SYNTH_UI_SRC_DIR} ${INF_BASE_FORMAT_VST3_SRC_DIR} ${INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_SRC_DIR})
set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "InfernalSynth${suffix}${INFERNAL_VERSION}")
target_link_libraries(${target_name} infernal.plugin.infernal_synth infernal.plugin.infernal_synth.ui infernal.base.format.vst3 
  infernal.base.ui infernal.base lib.vst3sdk lib.juce.gui_basics lib.juce.graphics lib.juce.events lib.juce.data_structures lib.juce.core)
add_custom_target(${target_name}-copy-themes ALL DEPENDS ${target_name} COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/themes
  "${PROJECT_SOURCE_DIR}/dist/${platform}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/InfernalSynth${suffix}${INFERNAL_VERSION}.vst3/Contents/Resources/Themes")
add_custom_target(${target_name}-copy-presets ALL DEPENDS ${target_name} COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/presets 
  "${PROJECT_SOURCE_DIR}/dist/${platform}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/InfernalSynth${suffix}${INFERNAL_VERSION}.vst3/Contents/Resources/Presets")
if (MSVC)
set_target_properties(${target_name} PROPERTIES SUFFIX ".vst3")
source_group(TREE "../../${INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_SRC_DIR}" FILES ${INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_SRC})
target_compile_options(${target_name} PRIVATE "SHELL:/external:I ${VST3_SDK_SRC_DIR}")
set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  "${PROJECT_SOURCE_DIR}/dist/win32/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/InfernalSynth${INFERNAL_VERSION}.vst3/Contents/x86_64-win")
else ()
set_target_properties(${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
  "${PROJECT_SOURCE_DIR}/dist/linux/${CMAKE_BUILD_TYPE}/InfernalSynth${INFERNAL_VERSION}.vst3/Contents/x86_64-linux")
endif()
endfunction()
if (MSVC)
declare_inf_plugin_infernal_synth_format_vst3(infernal.plugin.infernal_synth.format.vst3i 0 "" win32)
declare_inf_plugin_infernal_synth_format_vst3(infernal.plugin.infernal_synth.format.vst3fx 1 "FX" win32)
else ()
declare_inf_plugin_infernal_synth_format_vst3(infernal.plugin.infernal_synth.format.vst3i 0 "" linux)
declare_inf_plugin_infernal_synth_format_vst3(infernal.plugin.infernal_synth.format.vst3fx 1 "FX" linux)
endif ()