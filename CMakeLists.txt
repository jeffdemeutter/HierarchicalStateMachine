cmake_minimum_required(VERSION 3.20.x)

project(ConstexprTests VERSION 0.0.1)


message("Setting variables")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INC_SRC_DIRS)

message("Querying all subdirectories from ${SRC_DIR}")
file(GLOB_RECURSE QRY_FILES LIST_DIRECTORIES TRUE RELATIVE ${SRC_DIR} "${SRC_DIR}/*/")
foreach(QRY_RESULT ${QRY_FILES})
    if(IS_DIRECTORY "${SRC_DIR}/${QRY_RESULT}")
        message(" - Found: ${SRC_DIR}/${QRY_RESULT}")
        list(APPEND INC_SRC_DIRS "${SRC_DIR}/${QRY_RESULT}")
    endif()
endforeach()
message("Finished querying files from ${SRC_DIR}")
include_directories(${SRC_DIR} ${INC_SRC_DIRS})

message("Querying all files")
file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/**.cpp"
    "${SRC_DIR}/**.h"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)