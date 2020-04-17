cmake_minimum_required(VERSION 3.0)

project("iteco-apps")

# mbedtls doesn't export its interface headers via CMake's target_include_directory(... INTERFACE ...),
# so using this roundabout trick
if(DEFINED ${MBEDTLS_INCLUDE_PATH})
    include_directories(${MBEDTLS_INCLUDE_PATH})
endif()

add_subdirectory(client)
add_subdirectory(server)