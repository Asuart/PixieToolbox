file(GLOB_RECURSE _source_list CONFIGURE_DEPENDS "*.c" "*.cpp" "*.h" "*.hpp")

add_library(PixieToolboxCore STATIC ${_source_list})

set_target_properties(PixieToolboxCore PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(PixieToolboxCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")

target_include_directories(PixieToolboxCore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}../../../Dependencies/stb/")

target_link_libraries(PixieToolboxCore PUBLIC PixieApplication)
target_link_libraries(PixieToolboxCore PUBLIC ResourceManagement)
target_link_libraries(PixieToolboxCore PUBLIC glaze::glaze)

if (MSVC)
    target_compile_options(PixieToolboxCore PRIVATE /W4 /WX)
else()
    target_compile_options(PixieToolboxCore PRIVATE -Wall -Werror -freflection)
endif()

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" _group_path "${_source_path}")
    string(REPLACE "../" "" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
