file(GLOB_RECURSE _source_list "*.c" "*.cpp" "*.h" "*.hpp" "../../dependencies/glad/src/glad.c")
file(GLOB_RECURSE _shaders "../../resources/shaders/*.glsl")

add_library(PixieToolboxCore STATIC ${_source_list} ${_shaders})

target_include_directories(PixieToolboxCore PUBLIC "${glm_SOURCE_DIR}/")
target_include_directories(PixieToolboxCore PUBLIC "${stb_SOURCE_DIR}/")
target_include_directories(PixieToolboxCore PUBLIC "${ASSIMP_INCLUDE_DIRS}")
target_include_directories(PixieToolboxCore PUBLIC "${SDL2_SOURCE_DIR}/include/")
target_include_directories(PixieToolboxCore PUBLIC "${imgui_SOURCE_DIR}/")
target_include_directories(PixieToolboxCore PUBLIC "${imgui_SOURCE_DIR}/backends/")

target_include_directories(PixieToolboxCore PRIVATE "${CMAKE_SOURCE_DIR}/dependencies/glad/include/")


target_link_libraries(PixieToolboxCore PUBLIC SDL2::SDL2)
target_link_libraries(PixieToolboxCore PUBLIC assimp)
target_link_libraries(PixieToolboxCore PUBLIC stb)
target_link_libraries(PixieToolboxCore PUBLIC imgui)

target_link_libraries(PixieToolboxCore PRIVATE glslang)
target_link_libraries(PixieToolboxCore PRIVATE SPIRV)
target_link_libraries(PixieToolboxCore PRIVATE glslang::glslang-default-resource-limits)
target_link_libraries(PixieToolboxCore PRIVATE Vulkan::Vulkan)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/source/PixieToolboxCore" "" _group_path "${_source_path}")
    string(REPLACE "../../../" "" _group_path "${_group_path}")
    string(REPLACE "../../" "" _group_path "${_group_path}")
    string(REPLACE "../" "" _group_path "${_group_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set_target_properties(PixieToolboxCore PROPERTIES RESOURCE _shaders)

foreach(_shader IN ITEMS ${_shaders})
    source_group("Resources/Shaders" FILES ${_shader})
    message("Found shader: ${_shader}")
endforeach()
