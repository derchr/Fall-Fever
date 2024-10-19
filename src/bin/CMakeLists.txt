add_executable(Fall-Fever
    main.cpp
    controller.cpp
)

target_link_libraries(Fall-Fever PRIVATE fever_core)
