ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # -W -Wall -Werror
    # Other flags
)

# Set the name and the supported language of the project
project(hello-world)

# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.6)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
find_package(X11)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(X11 REQUIRED x11)


# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${X11_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
link_directories(${X11_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})


# Add an executable compiled from hello.c
add_executable(hello x11.cpp main.cpp)

# Link the target to the GTK+ libraries
target_link_libraries(hello ${GTK3_LIBRARIES} ${GLIB_LIBRARIES} ${X11_LIBRARIES})
