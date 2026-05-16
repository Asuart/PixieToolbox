message(STATUS "Fetching SDL ...")

FetchContent_MakeAvailable(SDL2)

set_target_properties(SDL2 PROPERTIES FOLDER sdl)
set_target_properties(SDL2main PROPERTIES FOLDER sdl)
set_target_properties(SDL2_test PROPERTIES FOLDER sdl)
set_target_properties(SDL2-static PROPERTIES FOLDER sdl)
set_target_properties(sdl_headers_copy PROPERTIES FOLDER sdl)
set_target_properties(uninstall PROPERTIES FOLDER sdl)