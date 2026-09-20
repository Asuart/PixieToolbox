add_library(ufbx STATIC ../ufbx/ufbx.c)

target_include_directories(ufbx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../ufbx)