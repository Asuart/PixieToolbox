message(STATUS "Fetching stb ...")

set(CMAKE_CXX_STANDARD 20)

FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
  FetchContent_Populate(stb)
endif()

FetchContent_MakeAvailable(stb)

add_library(stb INTERFACE
    ${stb_SOURCE_DIR}/stb_image.h
)
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

set_target_properties(stb PROPERTIES FOLDER stb)