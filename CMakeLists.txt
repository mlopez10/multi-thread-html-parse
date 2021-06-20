cmake_minimum_required(VERSION 3.0.0)
project(nTopology VERSION 0.1.0)

include(FetchContent)

FetchContent_Declare(cpr 
                     GIT_REPOSITORY https://github.com/whoshuu/cpr.git 
                     GIT_TAG f4622efcb59d84071ae11404ae61bd821c1c344b # the commit hash for 1.6.2
                     CPR_BUILD_TESTS off) 
FetchContent_MakeAvailable(cpr)
add_executable(nTopology nTopology.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
