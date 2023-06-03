add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/entt)

option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)

option(FX_GLTF_BUILD_TESTS "" OFF)
option(FX_GLTF_INSTALL "" OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fx-gltf)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stb)
