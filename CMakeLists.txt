# cmake_minimum_required(VERSION 3.21)
project(X_Programming)

set(CMAKE_CXX_STANDARD 17)

add_executable(X_Programming main.c)
add_executable(xdraw sample/xdraw.c)
add_executable(Perlin3D sample/teisyutsu/12_Perlin3D.c)
target_link_libraries(X_Programming X11)
target_link_libraries(xdraw X11)
target_link_libraries(Perlin3D X11)
target_link_libraries(Perlin3D m)
