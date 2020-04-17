cmake_minimum_required(VERSION 3.0.2)

project("iteco-apps-server")

file(GLOB ITECO_SERVER_SRCS ${PROJECT_SOURCE_DIR}/*.c)
file(GLOB ITECO_SERVER_HDRS ${PROJECT_SOURCE_DIR}/*.h)
set(SERVER_APP_NAME iteco-server)

add_executable(${SERVER_APP_NAME} ${ITECO_SERVER_SRCS} ${ITECO_SERVER_HDRS})
target_include_directories(${SERVER_APP_NAME} PRIVATE /usr/local/include)
target_link_libraries(${SERVER_APP_NAME} iteco-common)