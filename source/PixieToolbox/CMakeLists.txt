file(GLOB_RECURSE _source_list "*.c" "*.cpp" "*.h" "*.hpp")

add_executable(PixieToolbox ${_source_list})

target_link_libraries(PixieToolbox PUBLIC PixieToolboxCore)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/source/PixieToolbox" "" _group_path "${_source_path}")
    string(REPLACE "../" "" _group_path "${_group_path}")
    string(REPLACE "../../" "" _group_path "${_group_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/")
set(SHADER_DESTINATION_DIR "$<TARGET_FILE_DIR:PixieToolbox>/resources/")

add_custom_command(
    TARGET PixieToolbox
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SHADER_SOURCE_DIR}" 
    "${SHADER_DESTINATION_DIR}"
    COMMENT "Copying Resources"
)
