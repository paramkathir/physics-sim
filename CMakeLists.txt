cmake_minimum_required(VERSION 3.16)
project(PhysicsSim)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/SFML/SFML-2.6.1/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(PhysicsSim src/main.cpp)

target_link_libraries(PhysicsSim sfml-graphics sfml-window sfml-system)