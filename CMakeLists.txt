cmake_minimum_required(VERSION 3.23)
project(StartRender)

#C++标准
set(CMAKE_CXX_STANDARD 11)

#源码路径
aux_source_directory(${PROJECT_SOURCE_DIR}/src/ DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/public DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/engine DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/object DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/platform DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/public DIR_SRCS)


#UTF-8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")



#告诉编译器生成调试信息
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")



#添加imgui
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui)
aux_source_directory(${PROJECT_SOURCE_DIR}/lib/imgui IMGUI_SRC)



#输出目录
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} ${IMGUI_SRC} ${DIR_SRCS} )


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
#glfw
FIND_LIBRARY(GLFW3_LIB glfw3.lib  ${PROJECT_SOURCE_DIR}/lib/glfw)
target_link_libraries(${PROJECT_NAME}  ${GLFW3_LIB})
target_link_libraries(${PROJECT_NAME}  opengl32.lib)
include_directories(${PROJECT_SOURCE_DIR}/lib/glfw/include)

#测试
enable_testing()
add_subdirectory(${PROJECT_SOURCE_DIR}/test)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FA")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX2")