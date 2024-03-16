cmake_minimum_required(VERSION 3.28)
project(HttpServer C)

add_executable(${PROJECT_NAME} src/main.c src/httpdefs.c)

target_link_directories(${PROJECT_NAME} PRIVATE libs)

target_link_libraries(${PROJECT_NAME} wsock32 LinkedList Queue)
