cmake_minimum_required(VERSION 3.1)
project(osbTools)

#set(PROJECT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT})
#set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# Get the current version
include(version)

add_definitions(
    -DOSBTOOLS_MAJOR_VERSION="${OSBTOOLS_MAJOR_VERSION}"
    -DOSBTOOLS_MINOR_VERSION="${OSBTOOLS_MINOR_VERSION}"
    -DOSBTOOLS_PATCH_VERSION="${OSBTOOLS_PATCH_VERSION}")

set(LINUX FALSE)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(LINUX TRUE)
endif()

if (LINUX)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if (COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif (COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif()
endif()


# Set some debug vs opt flags
if ("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    add_definitions(-DDEBUG=1 -UNDEBUG)
    if (LINUX)
        add_definitions(
            -Wall
            -Wextra
            -Wno-deprecated-register
            -Wno-ignored-qualifiers
            -Wno-reorder
            -Wno-unused-local-typedefs
            -Wno-unused-parameter)
    endif()
elseif ("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    add_definitions(-DNDEBUG=1 -UDEBUG)
    if (LINUX)
        add_definitions(-O3 -Wno-deprecated-register)
    endif()
endif()


add_subdirectory(src)

#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/module.txt" "${PROJECT_PATH}/${PROJECT_NAME}.txt")

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/osbTools.mod.in"
    "${CMAKE_BINARY_DIR}/osbTools.mod")

install(DIRECTORY mel/ DESTINATION scripts)
install(FILES py/InheritParentsName.py DESTINATION plug-ins)
install(FILES py/ResolveDuplicates.py DESTINATION plug-ins)
install(DIRECTORY ui/ DESTINATION python)
install(FILES py/NewtonRenaming.py DESTINATION python)
#install(FILES osbTools.mod DESTINATION .)
install(FILES "${CMAKE_BINARY_DIR}/osbTools.mod" DESTINATION ${CMAKE_INSTALL_PREFIX})
