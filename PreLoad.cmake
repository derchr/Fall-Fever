if(UNIX)
    set(CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)
endif(UNIX)

if(WIN32)
    set(CMAKE_GENERATOR "MinGW Makefiles" CACHE INTERNAL "" FORCE)
endif(WIN32)
