cmake_minimum_required(VERSION 3.26)
project(FrechetSim C CXX)

# Define variables for source and include directories
set(SRC_DIR src)
set(INC_DIR include)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

file(GLOB SOURCES "${SRC_DIR}/*.cpp")

add_executable(FrechetSim ${SOURCES})

target_include_directories(FrechetSim PUBLIC ${INC_DIR})

target_link_libraries(FrechetSim SDL3::SDL3 SDl3_image SDL3_image)
