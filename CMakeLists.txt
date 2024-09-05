cmake_minimum_required(VERSION 3.28)
project(mc_ench_cmd_gen)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB_RECURSE HEADERS include/*.*)
file(GLOB_RECURSE CPP_SRC src/*.cpp)
file(GLOB_RECURSE UI_SRC ui/*.ui)

add_executable(mc_ench_cmd_gen WIN32 ${HEADERS} ${CPP_SRC} ${UI_SRC})
target_include_directories(mc_ench_cmd_gen PRIVATE include)
target_link_libraries(mc_ench_cmd_gen PRIVATE Qt6::Widgets)
target_link_libraries(mc_ench_cmd_gen PRIVATE nlohmann_json::nlohmann_json)

file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/data)
