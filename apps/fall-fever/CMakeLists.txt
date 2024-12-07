find_package(cxxopts CONFIG)

add_executable(Fall-Fever
    main.cpp
    controller.cpp
    flycam.cpp
)

target_link_libraries(Fall-Fever PRIVATE fever_core cxxopts::cxxopts)
