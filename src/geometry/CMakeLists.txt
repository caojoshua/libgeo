add_library(geo)

target_link_libraries(
  geo
  PUBLIC geodatastruct
  )

add_subdirectory(structure)
