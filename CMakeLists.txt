cmake_minimum_required(VERSION 3.5)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(GraphicsEngine LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)

add_executable(GraphicsEngine
    shaders/ForwardPhongVert.glsl
    shaders/ForwardPhongFrag.glsl
    Main.cpp
    Utility.h
    Utility.cpp
    GLDriver.h
    GLDriver.cpp
    Scene.h
    Scene.cpp
    DrawContext.h
    DrawContext.cpp
    Drawables.h
    Drawables.cpp
    Effects.h
    Effects.cpp
    Viewer.h
    Viewer.cpp
    Plugins.h
    Plugins.cpp
)

target_link_libraries(GraphicsEngine PRIVATE Qt5::Widgets tinyobjloader::tinyobjloader)

file(COPY shaders DESTINATION ${PROJECT_BINARY_DIR})
