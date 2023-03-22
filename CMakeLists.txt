cmake_minimum_required(VERSION 3.23)
project(StartRender)

#C++标准
set(CMAKE_CXX_STANDARD 11)

#源码路径
aux_source_directory(${PROJECT_SOURCE_DIR}/src/ DIR_SRCS)
aux_source_directory(src/engine/render DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/engine DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/engine/shader DIR_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/engine/data DIR_SRCS)
aux_source_directory(src/engine/render/file DIR_SRCS)
aux_source_directory(src/engine/platform DIR_SRCS)
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

if(MSVC)
    set(MY_VERSIONINFO_RC "${CMAKE_BINARY_DIR}/version.rc")
    configure_file("${PROJECT_SOURCE_DIR}/version.rc"
            "${MY_VERSIONINFO_RC}")
endif()


# 需要生成嵌入的文件
file (GLOB EMBED_FILES
        "${PROJECT_SOURCE_DIR}/resource/*.*"
        )
# 输出文件目录
set (GEN_EMBED_OUTPUT_HDR_DIR
        "${CMAKE_CURRENT_BINARY_DIR}/gen_inc")
set (GEN_EMBED_OUTPUT_SRC_DIR
        "${CMAKE_CURRENT_BINARY_DIR}/gen_src")
file(MAKE_DIRECTORY ${GEN_EMBED_OUTPUT_HDR_DIR})
file(MAKE_DIRECTORY ${GEN_EMBED_OUTPUT_SRC_DIR})

# 依次处理文件
foreach(input_src ${EMBED_FILES})
    # 配置输出文件名
    file(SIZE ${input_src} embed_file_size)
    get_filename_component(embed_file ${input_src} NAME)
    set(gen_embed_file        "${GEN_EMBED_OUTPUT_SRC_DIR}/${embed_file}.cpp")
    set(gen_embed_file_header "${GEN_EMBED_OUTPUT_HDR_DIR}/${embed_file}.h")
    # 清空输出文件
    file(WRITE ${gen_embed_file} "")
    file(WRITE ${gen_embed_file_header} "")
    # for c compatibility
    string(MAKE_C_IDENTIFIER ${embed_file} token)
    # to upper case
    string(TOUPPER ${token} token)
    # read hex data from file
    file(READ ${input_src} filedata HEX)
    # convert hex data for C compatibility
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
    # append data to output file
    file(APPEND ${gen_embed_file}
            "extern const unsigned char R_${token}_DATA[] = {\n${filedata}0x00\n};\n"
            "extern const unsigned long R_${token}_SIZE   = ${embed_file_size};\n")
    file(APPEND ${gen_embed_file_header}
            "extern  const unsigned char R_${token}_DATA[];\n"
            "extern  const unsigned long R_${token}_SIZE;\n")
    # 加入到生成文件列表
    list(APPEND GEN_EMBED_FILES
            ${gen_embed_file}
            ${gen_embed_file_header}
            )
endforeach()

add_custom_target(
        embed_gen_files
        DEPENDS ${GEN_EMBED_FILES}
)
include_directories(${GEN_EMBED_OUTPUT_HDR_DIR})



#生成二进制
add_executable(${PROJECT_NAME} ${DIR_SRCS} ${IMGUI_SRC} ${LODEPNG_SRC} ${MY_VERSIONINFO_RC} ${GEN_EMBED_FILES})
#add_dependencies(${PROJECT_NAME} embed_gen_files)





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


#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FA")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX2")