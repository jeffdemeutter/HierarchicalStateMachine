cmake_minimum_required(VERSION 3.20.x)

project(example)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} INTERFACE
    StateMachine    
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)