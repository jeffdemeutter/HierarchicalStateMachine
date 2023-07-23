cmake_minimum_required(VERSION 3.20.x)

add_executable(example main.cpp)

target_link_libraries(example INTERFACE
    StateMachine    
)
set_property(TARGET example PROPERTY CXX_STANDARD 23)