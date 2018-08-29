
set(SOURCE_FILES
    pluginMain.cpp
    holesBorder.cpp
    holesBorder.h
    triangleOverlappingTest.cpp
    UVReversed.cpp
    UVReversed.h
    overlappingUVShells.cpp
    overlappingUVShells.h
    UVCommon.cpp
    UVCommon.h
    intersect.cpp
    intersect.h
    Renaming.cpp
    Renaming.h
    UVSeams.cpp
    UVSeams.h
)

find_package(Maya REQUIRED)
find_package(Boost REQUIRED)

include_directories(${MAYA_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
link_directories(${MAYA_LIBRARY_DIR})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
#target_link_libraries(${PROJECT_NAME} ${MAYA_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_link_libraries(${PROJECT_NAME}
    ${MAYA_OpenMaya_LIBRARY}
    ${MAYA_OpenMayaUI_LIBRARY}
    ${MAYA_OpenMayaAnim_LIBRARY})

if (WIN32)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION plug-ins)
else()
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION plug-ins)
endif()
# MAYA_PLUGIN(${PROJECT_NAME})
