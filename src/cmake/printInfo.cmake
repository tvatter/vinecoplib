message( STATUS )
message( STATUS "------------------ General configuration for ${PROJECT_NAME} ${PROJECT_VERSION} ")
message( STATUS )
message( STATUS "CMAKE_SOURCE_DIR=              ${CMAKE_SOURCE_DIR}")
message( STATUS "CMAKE_BINARY_DIR=              ${CMAKE_BINARY_DIR}")
message( STATUS "CMAKE_SYSTEM_PROCESSOR=        ${CMAKE_SYSTEM_PROCESSOR}" )
message( STATUS "Generator=                     ${CMAKE_GENERATOR}")
message( STATUS "CMAKE_SYSTEM_VERSION=          ${CMAKE_SYSTEM_VERSION}")
message( STATUS "CMAKE_CXX_COMPILER_ID=         ${CMAKE_CXX_COMPILER_ID}")
message( STATUS "CMAKE_CXX_COMPILER=            ${CMAKE_CXX_COMPILER}")
message( STATUS "CMAKE_CXX_COMPILER_VERSION=    ${CMAKE_CXX_COMPILER_VERSION}")
message( STATUS "CMAKE_BUILD_TYPE=              ${CMAKE_BUILD_TYPE}")

message( STATUS "CMAKE_CXX_FLAGS=               ${CMAKE_CXX_FLAGS}")
message( STATUS "CMAKE_CXX_FLAGS_DEBUG=         ${CMAKE_CXX_FLAGS_DEBUG}")
message( STATUS "CMAKE_CXX_FLAGS_RELEASE=       ${CMAKE_CXX_FLAGS_RELEASE}")
message( STATUS )
message( STATUS "EXTERNAL_INCLUDE=       ${external_libs}")
message( STATUS )
message( STATUS )
message( STATUS "WARNINGS_AS_ERRORS=            ${WARNINGS_AS_ERRORS}")
#message( STATUS "TEST_DATA_DIR=                 ${TEST_DATA_DIR}")
message( STATUS "OPT_ASAN=                      ${OPT_ASAN}")
message( STATUS )
