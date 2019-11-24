cmake_minimum_required(VERSION 2.8.12)
project(rsc)

add_compile_options(-std=c++11 -Wall -Wextra)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-stdlib=libc++)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(
        CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=rsc.map -Wl,--demangle"
    )
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-gdwarf-2)
endif()



##############################
######## library "nb" ########
##############################

set(NB_INC_DIR nb)
set(NB_SRC_DIR nb)

file(GLOB NB_SRCS
    ${NB_SRC_DIR}/*.cpp
)

add_library(nb STATIC ${NB_SRCS})
#target_compile_options(nb PUBLIC -fno-rtti -fno-exceptions)
#target_compile_options(nb PUBLIC -fno-exceptions)

target_include_directories(nb PRIVATE ${NB_INC_DIR} ${NB_INC_DIR})

#if(CYGWIN)
#target_link_libraries(nb dbghelp)
#endif()
#if(WIN32)
#target_link_libraries(nb dbghelp)
#endif()


###############################
######## mapfile "rsc" ########
###############################

# Generates/creates/copies the mapfile file (when it's not there)
add_custom_command(
    OUTPUT "rsc.map"
    COMMAND "${CMAKE_COMMAND}" -E touch "rsc.map"
)



##################################
######## executable "rsc" ########
##################################

#add_executable(rsc rsc/main.cpp rsc.map)
add_executable(rsc_empty rsc/main_empty.cpp rsc.map)

#whole-archive is used to force all symbols from static lib to be kept, not only used ones, allowing the auto-registering of classes to work
    set(CMAKE_EXE_LINKER_PRE_STATIC_LIBS "")
    set(CMAKE_EXE_LINKER_PRE_STATIC_LIB "")
    set(CMAKE_EXE_LINKER_POST_STATIC_LIBS "")
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
   #set(CMAKE_EXE_LINKER_PRE_STATIC_LIB "-Wl,-force_load")
else()
   #set(CMAKE_EXE_LINKER_PRE_STATIC_LIBS "-Wl,--whole-archive")
   #set(CMAKE_EXE_LINKER_POST_STATIC_LIBS "-Wl,--no-whole-archive")
endif()

target_link_libraries(rsc_empty
    ${CMAKE_EXE_LINKER_PRE_STATIC_LIBS}
    ${CMAKE_EXE_LINKER_PRE_STATIC_LIB} nb
    ${CMAKE_EXE_LINKER_POST_STATIC_LIBS})

target_include_directories(rsc_empty
    PRIVATE ${NB_INC_DIR})

set_target_properties(rsc_empty
    PROPERTIES
        LINK_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/rsc.map"
)

