cmake_minimum_required(VERSION 3.16)
project(Asteroids_Run LANGUAGES CXX)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build shared libraries option
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Include SFML via FetchContent
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# Add include directories for headers
include_directories(${CMAKE_SOURCE_DIR}/include)

include_directories(${CMAKE_SOURCE_DIR}/src)


# Add source files
set(SOURCES
    src/main.cpp
    src/Spaceship.cpp
    src/Bullet.cpp
    src/Asteroid.cpp
)

# Add executable target
add_executable(Asteroids_Run ${SOURCES})

# Copy assets folder to build directory
add_custom_command(TARGET Asteroids_Run POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Asteroids_Run>/../assets)

# Link SFML libraries
target_include_directories(Asteroids_Run PRIVATE ${sfml_SOURCE_DIR}/include)
target_link_libraries(Asteroids_Run PRIVATE sfml-graphics sfml-window sfml-system)

# Set C++ standard
target_compile_features(Asteroids_Run PRIVATE cxx_std_17)

# Install executable
install(TARGETS Asteroids_Run)

# Build libserialport during CMake
set(LIBSERIALPORT_DIR ${CMAKE_SOURCE_DIR}/external/libserialport)
add_custom_target(build_libserialport ALL
    COMMAND ./autogen.sh
    COMMAND ./configure
    COMMAND make
    WORKING_DIRECTORY ${LIBSERIALPORT_DIR}
)

# Add include and library paths for libserialport
target_include_directories(Asteroids_Run PRIVATE ${LIBSERIALPORT_DIR})
target_link_directories(Asteroids_Run PRIVATE ${LIBSERIALPORT_DIR}/.libs)

# Link libserialport to your project
add_dependencies(Asteroids_Run build_libserialport)
target_link_libraries(Asteroids_Run PRIVATE serialport)
