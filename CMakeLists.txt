cmake_minimum_required(VERSION 2.8.10)
project(webrtcnativelabs)


# Set the source directory and search locations as required for your project
set(sourcedir server)
set(sourcefiles ${sourcedir}/*.cpp)
set(headerfiles ${sourcedir}/*.h)

# Set some default options for LibSourcey
set(WITH_FFMPEG ON)
set(WITH_WEBRTC ON) # ON CACHE BOOL "Enable WebRTC dependency" FORCE)
set(BUILD_APPLICATIONS OFF CACHE BOOL "Disable applications" FORCE)
set(BUILD_SAMPLES OFF CACHE BOOL "Disable samples" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Disable tests" FORCE)
set(BUILD_SHARED_LIBS OFF)

# Include LibSourcey
include("../libsourcey/LibSourcey.cmake")
#print_module_variables(LibSourcey)

include_directories(${sourcedir} ${LibSourcey_INCLUDE_DIRS})
link_directories(${LibSourcey_LIBRARY_DIRS})
link_libraries(${LibSourcey_INCLUDE_LIBRARIES})

# Glob your sources and headers
# Be sure to modify the search paths according to your project structure
file(GLOB_RECURSE sources ${sourcefiles})
file(GLOB_RECURSE headers ${headerfiles})

# Create and install the executable
add_executable(webrtcnativelabs ${sources} ${headers})
add_dependencies(webrtcnativelabs libuv jsoncpp)
add_dependencies(webrtcnativelabs uv base crypto av net http util json socketio symple webrtc)
install(TARGETS webrtcnativelabs RUNTIME DESTINATION bin)
