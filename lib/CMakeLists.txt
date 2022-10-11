option(SPDLOG_NO_EXCEPTIONS "" ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/entt)

option(TINYGLTF_HEADER_ONLY "" OFF)
option(TINYGLTF_INSTALL "" OFF)
option(TINYGLTF_NOEXCEPTION "" ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tinygltf)

option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)
