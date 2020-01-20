cmake_minimum_required(VERSION 3.10)

project(GalaxySimu)

file(GLOB GalaxySimu_SRC
    "src/*.cpp"
    "include/*.h"
)

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(GalaxySimu ${GalaxySimu_SRC})

target_link_libraries(GalaxySimu PRIVATE 
    SDL2::SDL2 SDL2::SDL2main
    glm)
target_include_directories(GalaxySimu PRIVATE "${CMAKE_SOURCE_DIR}/include")
message("${GalaxySimu_SRC}")