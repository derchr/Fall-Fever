CPMAddPackage(NAME argparse URL "https://github.com/p-ranav/argparse/archive/refs/tags/v3.1.tar.gz")

add_executable(Fall-Fever
    main.cpp
    controller.cpp
    flycam.cpp
)

target_link_libraries(Fall-Fever PRIVATE fever_core argparse)
