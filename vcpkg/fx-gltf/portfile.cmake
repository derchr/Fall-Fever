vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO jessey-git/fx-gltf
    REF v2.0.0
    SHA512 0731732b5919a55045df2fe06bda6ea2a4d4d889556d738d0ade5acf39341a0748deb13bb130cbd6226146c70c2566ebfa67e6ac398157be00fefcf1f071a9bd
    HEAD_REF main
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS
        -DFX_GLTF_BUILD_TESTS=OFF
)

vcpkg_install_cmake()
