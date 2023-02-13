cmake_minimum_required(VERSION 3.23)
project(StartRender)

#C++标准
set(CMAKE_CXX_STANDARD 11)

#源码路径
aux_source_directory(${PROJECT_SOURCE_DIR}/src DIR_SRCS)

#UTF-8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

#告诉编译器生成调试信息
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

#输出目录
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} ${DIR_SRCS} src/math/Vector.cpp src/math/Vector.h src/math/Matrix.cpp src/math/Matrix.h src/MainWindow.cpp src/MainWindow.h src/engine/RenderThread.cpp src/engine/RenderThread.h src/event/EventManager.cpp src/event/EventManager.h src/engine/Context.cpp src/engine/Context.h src/object/Object3D.cpp src/object/Object3D.h src/engine/Util.cpp src/engine/Util.h src/event/Camera.cpp src/event/Camera.h src/viewing/Viewing.cpp src/viewing/Viewing.h src/engine/Render.cpp src/engine/Render.h src/type/type.h src/engine/RenderFrame.cpp src/engine/RenderFrame.h src/object/ObjectFile.cpp src/object/ObjectFile.h src/engine/Config.h)