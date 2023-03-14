cmake_minimum_required(VERSION 3.23)
project(StartRender)

#C++标准
set(CMAKE_CXX_STANDARD 11)

#源码路径
aux_source_directory(${PROJECT_SOURCE_DIR}/src/ DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/render DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/engine DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/engine/data DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/render/file DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/platform DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/math DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/scene DIR_SRCS)
#UTF-8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")



#告诉编译器生成调试信息
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")


#以源代码的方式添加库
#添加imgui backends
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui)
aux_source_directory(${PROJECT_SOURCE_DIR}/lib/imgui IMGUI_SRC)
#添加lodepng
include_directories(${PROJECT_SOURCE_DIR}/lib/lodepng)
aux_source_directory(${PROJECT_SOURCE_DIR}/lib/lodepng LODEPNG_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/test TEST_SRC)


#输出目录
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} ${DIR_SRCS} ${IMGUI_SRC} ${LODEPNG_SRC} src/engine/shader/VertexShader.cpp src/engine/shader/VertexShader.h src/engine/data/Texture.h src/engine/shader/RasterizationShader.h src/engine/shader/RasterizationShader.cpp src/engine/shader/RasterizationShader.h src/engine/shader/ColorShader.cpp src/engine/shader/ColorShader.h src/engine/data/GBuffer.h src/engine/shader/FragmentShader.cpp src/engine/shader/FragmentShader.h src/engine/data/VertexAttribute.h)
add_executable(${PROJECT_NAME}_Test ${LODEPNG_SRC} ${TEST_SRC})

#导入库
#jsoncpp
FIND_LIBRARY(JSONCPP_LIB_DEBUG jsoncppd.lib ${PROJECT_SOURCE_DIR}/lib/jsoncpp/)
FIND_LIBRARY(JSONCPP_LIB jsoncpp.lib ${PROJECT_SOURCE_DIR}/lib/jsoncpp/)
target_link_libraries(${PROJECT_NAME} debug ${JSONCPP_LIB_DEBUG} optimized ${JSONCPP_LIB})
include_directories(${PROJECT_SOURCE_DIR}/lib/jsoncpp/include)
#log4cpp
FIND_LIBRARY(LOG4CPP_LIB_DEBUG log4cppD.lib  ${PROJECT_SOURCE_DIR}/lib/log4cpp)
FIND_LIBRARY(LOG4CPP_LIB log4cpp.lib  ${PROJECT_SOURCE_DIR}/lib/log4cpp)
target_link_libraries(${PROJECT_NAME} debug ${LOG4CPP_LIB_DEBUG} optimized ${LOG4CPP_LIB})
include_directories(${PROJECT_SOURCE_DIR}/lib/log4cpp/include)


#ghc filesystem
include_directories(${PROJECT_SOURCE_DIR}/lib/filesystem)

#imgui
FIND_LIBRARY(IMGUI_LIB_DEBUG imgui.lib ${PROJECT_SOURCE_DIR}/lib/imgui/debug)
FIND_LIBRARY(IMGUI_LIB imgui.lib ${PROJECT_SOURCE_DIR}/lib/imgui/release)
target_link_libraries(${PROJECT_NAME}  debug ${IMGUI_LIB_DEBUG} optimized ${IMGUI_LIB})


#sdl2
FIND_LIBRARY(SDL2_LIB SDL2-static.lib ${PROJECT_SOURCE_DIR}/lib/sdl2/)
target_link_libraries(${PROJECT_NAME}  ${SDL2_LIB})
include_directories(${PROJECT_SOURCE_DIR}/lib/sdl2/include/SDL2)

#windows
target_link_libraries(${PROJECT_NAME}  Setupapi.lib)
target_link_libraries(${PROJECT_NAME}  Winmm.lib)
target_link_libraries(${PROJECT_NAME}  Imm32.lib)
target_link_libraries(${PROJECT_NAME}  Version.lib)

#libjpeg-turbo
FIND_LIBRARY(LIBJPEG-TURBO turbojpeg-static.lib ${PROJECT_SOURCE_DIR}/lib/libjpeg-turbo/lib/)
target_link_libraries(${PROJECT_NAME}  ${LIBJPEG-TURBO})
include_directories(${PROJECT_SOURCE_DIR}/lib/libjpeg-turbo/include)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FA")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX2")