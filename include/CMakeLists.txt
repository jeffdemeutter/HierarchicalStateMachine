cmake_minimum_required(VERSION 3.20.x)

project(StateMachine LANGUAGES CXX VERSION 0.0.1 DESCRIPTION "Simple header-only state machine")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "StateMachine.h")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)