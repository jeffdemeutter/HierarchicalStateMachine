cmake_minimum_required(VERSION 3.20.x)

project(StateMachine LANGUAGES CXX VERSION 0.0.1 DESCRIPTION "Simple header-only state machine")

add_subdirectory("include")

option(UseExamples "Wether to compile the examples" OFF)
if(UseExamples)
    add_subdirectory("examples")
endif(UseExamples)
