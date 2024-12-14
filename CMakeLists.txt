cmake_minimum_required(VERSION 3.16)
project(Asteroids_Run LANGUAGES CXX)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build shared libraries option
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Ensure Git is available for submodule management
find_package(Git REQUIRED)

# Initialize libserialport submodule if not already populated
set(LIBSERIALPORT_DIR ${CMAKE_SOURCE_DIR}/external/libserialport)
if(NOT EXISTS ${LIBSERIALPORT_DIR}/libserialport.h)
    message(STATUS "Submodule 'libserialport' not found. Initializing...")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to initialize submodules. Please run:\n"
                            "  git submodule update --init --recursive")
    endif()
else()
    message(STATUS "Submodule 'libserialport' is already initialized.")
endif()

# Add custom target to build libserialport
add_custom_target(build_libserialport ALL
    COMMAND ./autogen.sh
    COMMAND ./configure
    COMMAND make
    WORKING_DIRECTORY ${LIBSERIALPORT_DIR}
    COMMENT "Building libserialport..."
    VERBATIM
)

# Include SFML via FetchContent
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

# Add source files
set(SOURCES
    src/main.cpp
    src/Spaceship.cpp
    src/Bullet.cpp
    src/Asteroid.cpp
)

# Add executable target
add_executable(Asteroids_Run ${SOURCES})

# Add dependency for Asteroids_Run
add_dependencies(Asteroids_Run build_libserialport)

# Copy assets folder to build directory
add_custom_command(TARGET Asteroids_Run POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Asteroids_Run>/../assets
)

# Link SFML libraries
target_include_directories(Asteroids_Run PRIVATE ${sfml_SOURCE_DIR}/include)
target_link_libraries(Asteroids_Run PRIVATE sfml-graphics sfml-window sfml-system)

# Include and link libserialport
target_include_directories(Asteroids_Run PRIVATE ${LIBSERIALPORT_DIR})
target_link_directories(Asteroids_Run PRIVATE ${LIBSERIALPORT_DIR}/.libs)
target_link_libraries(Asteroids_Run PRIVATE serialport)

# Set C++ standard
target_compile_features(Asteroids_Run PRIVATE cxx_std_17)

# Install executable
install(TARGETS Asteroids_Run)

# Use macOS system OpenAL framework
find_library(OPENAL_FRAMEWORK OpenAL)
if(OPENAL_FRAMEWORK)
    message(STATUS "Using macOS system OpenAL framework: ${OPENAL_FRAMEWORK}")
    target_link_libraries(Asteroids_Run PRIVATE ${OPENAL_FRAMEWORK})
else()
    message(FATAL_ERROR "OpenAL framework not found on macOS.")
endif()
target_include_directories(Asteroids_Run PRIVATE ${CMAKE_SOURCE_DIR}/include)
