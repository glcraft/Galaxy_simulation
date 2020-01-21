cmake_minimum_required(VERSION 3.10)

project(GalaxySimu)

file(GLOB GalaxySimu_SRC
    "src/*.cpp"
    "include/*.h"
)

find_package(SDL2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(GalaxySimu ${GalaxySimu_SRC})

target_link_libraries(GalaxySimu PRIVATE 
    SDL2::SDL2 SDL2::SDL2main 
    glm
    nlohmann_json::nlohmann_json
)
target_compile_features(GalaxySimu
    PUBLIC
    c_std_11
)
target_compile_features(GalaxySimu
    PUBLIC
    cxx_std_20
)



if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(GalaxySimu
        PUBLIC
            /std:c++latest /permissive- /experimental:preprocessor /fsanitize=address
    )
    set_target_properties(GalaxySimu PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

target_include_directories(GalaxySimu PRIVATE "${CMAKE_SOURCE_DIR}/include")
