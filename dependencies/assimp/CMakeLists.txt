message(STATUS "Fetching assimp ...")

FetchContent_MakeAvailable(assimp)

set_target_properties(assimp PROPERTIES FOLDER assimp)
set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER assimp)
set_target_properties(unit PROPERTIES FOLDER assimp)
set_target_properties(zlibstatic PROPERTIES FOLDER assimp)
