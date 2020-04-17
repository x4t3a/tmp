
cmake_minimum_required(VERSION 3.02)

project("iteco-common")

file(GLOB ITECO_COMMON_SRCS "${PROJECT_SOURCE_DIR}/src/common/*.c")
file(GLOB ITECO_COMMON_HDRS "${PROJECT_SOURCE_DIR}/include/common/*.h")

add_library(iteco-common STATIC ${ITECO_COMMON_SRCS} ${ITECO_COMMON_HDRS})
target_include_directories(iteco-common PUBLIC "${PROJECT_SOURCE_DIR}/include")

find_library(
    MBEDTLS_LIB_PATH
    NAMES libmbedtls.a
    PATHS "/usr/local/lib" "/usr/lib"
)

find_library(
    MBEDCRYPTO_LIB_PATH
    NAMES libmbedcrypto.a
    PATHS "/usr/local/lib" "/usr/lib"
)

target_link_libraries(iteco-common ${MBEDTLS_LIB_PATH} ${MBEDCRYPTO_LIB_PATH})