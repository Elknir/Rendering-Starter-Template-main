cmake_minimum_required(VERSION 3.20)
project(TPs-Rendering)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
#add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_executable(main src/main.cpp)
target_include_directories(main PRIVATE src)
target_compile_features(main PRIVATE cxx_std_20)

# Include lib
add_subdirectory(opengl-framework)
target_link_libraries(main PRIVATE opengl_framework::opengl_framework)
gl_target_copy_folder(main res)

