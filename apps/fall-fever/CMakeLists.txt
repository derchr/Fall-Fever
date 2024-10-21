find_package(cxxopts REQUIRED)
find_package(imgui REQUIRED)

add_executable(Fall-Fever
    main.cpp
    controller.cpp
    flycam.cpp
)

target_link_libraries(Fall-Fever PRIVATE
    fever_core
    cxxopts::cxxopts
    imgui::imgui
)
