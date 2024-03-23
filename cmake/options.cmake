set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_MACOSX_RPATH 1)

option(CMAKE_BUILD_TYPE "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." "Release")
option(VINECOPULIB_SHARED_LIB    "Pre-compiled version"              "OFF")
option(WARNINGS_AS_ERRORS        "Compiler warnings as errors"       "OFF")
option(OPT_ASAN                  "Use adress sanitizer (debug)"      "ON")
option(BUILD_TESTING             "Build tests."                      "ON")
option(CODE_COVERAGE             "Code coverage."                    "OFF")