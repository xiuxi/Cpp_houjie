cmake_minimum_required(VERSION 3.2)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(APP_SRC test.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(stltest ${APP_SRC})

install(TARGETS stltest DESTINATION bin)
