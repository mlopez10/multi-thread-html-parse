cmake_minimum_required(VERSION 3.1.0)
project(multithread_Html_Parse VERSION 0.1.0)


include(FetchContent)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

FetchContent_Declare(cpr 
                     GIT_REPOSITORY https://github.com/whoshuu/cpr.git
                     GIT_TAG f4622efcb59d84071ae11404ae61bd821c1c344b # the commit hash for 1.6.2
                     ) 
FetchContent_GetProperties(cpr)
string(TOLOWER "cpr" lcName)
if(NOT ${lcName}_POPULATED)
    FetchContent_populate(cpr)
    set(CPR_BUILD_TESTS off)
    add_subdirectory(${${lcName}_SOURCE_DIR} ${${lcName}_BINARY_DIR})
endif()

FetchContent_Declare(tidy 
                     GIT_REPOSITORY https://github.com/htacg/tidy-html5.git
                     GIT_TAG 5.7.28
                     ) 
FetchContent_GetProperties(tidy)
string(TOLOWER "tidy" lcName)
if(NOT ${lcName}_POPULATED)
    FetchContent_populate(tidy)
    set(SUPPORT_CONSOLE_APP off)
    add_subdirectory(${${lcName}_SOURCE_DIR} ${${lcName}_BINARY_DIR})
endif()


add_executable(multithread_Html_Parse main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr PRIVATE tidy-share)
target_include_directories(${PROJECT_NAME} PRIVATE ${${lcName}_SOURCE_DIR}/include)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_SOURCE_DIR}/bin)

include(CPack)
