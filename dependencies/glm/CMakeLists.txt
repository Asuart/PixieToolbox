message(STATUS "Fetching glm ...")

FetchContent_MakeAvailable(glm)

target_include_directories(glm PUBLIC ${glm_SOURCE_DIR})

set_target_properties(glm PROPERTIES FOLDER glm)