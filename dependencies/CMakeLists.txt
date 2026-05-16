include(FetchContent)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG release-2.32.4
)
add_subdirectory(sdl)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  GIT_TAG 63f00dae6b9142c2570aaf30b58932153e796567
)
add_subdirectory(imgui)

FetchContent_Declare(
  glm
  GIT_REPOSITORY "https://github.com/g-truc/glm.git"
  GIT_TAG 2d4c4b4dd31fde06cfffad7915c2b3006402322f
)
add_subdirectory(glm)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY "https://github.com/assimp/assimp.git"
  GIT_TAG 925541290612f42a3b4eb37c9dc1c269ce38de06
)
add_subdirectory(assimp)

FetchContent_Declare(
  stb
  GIT_REPOSITORY "https://github.com/nothings/stb.git"
  GIT_TAG f58f558c120e9b32c217290b80bad1a0729fbb2c
)
add_subdirectory(stb)

FetchContent_Declare(
  glslang
  GIT_REPOSITORY "https://github.com/KhronosGroup/glslang.git"
  GIT_TAG 15.4.0
)
add_subdirectory(glslang)
