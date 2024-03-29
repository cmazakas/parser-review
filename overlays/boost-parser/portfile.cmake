# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO tzlaine/parser
    REF 3c3e44ebf2aa556493f0fa3bc4eafe5d2da4a50d
    SHA512 825b3deb7f2577193d46226b569f77fdfef26f3d0f38cadc37fe6fdc80330fcd9ded562b837f6bdb5a1abba3133b2b8c74075e77fcda9507d002a1a11c597a95
    HEAD_REF develop
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})
