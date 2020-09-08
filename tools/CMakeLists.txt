project(obj-converter)

add_executable(obj-converter main.cpp)

target_link_libraries(obj-converter assimp glm)
