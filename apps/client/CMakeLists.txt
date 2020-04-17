cmake_minimum_required(VERSION 3.0.2)

project("iteco-apps-client")

file(GLOB ITECO_CLIENT_SRCS ${PROJECT_SOURCE_DIR}/*.c)
file(GLOB ITECO_CLIENT_HDRS ${PROJECT_SOURCE_DIR}/*.h)
set(CLIENT_APP_NAME iteco-client)

add_executable(${CLIENT_APP_NAME} ${ITECO_CLIENT_SRCS} ${ITECO_CLIENT_HDRS})
target_include_directories(${CLIENT_APP_NAME} PRIVATE /usr/local/include)
target_link_libraries(${CLIENT_APP_NAME} iteco-common)