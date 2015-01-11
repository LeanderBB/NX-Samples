#------------------------------------------------------------------------------
# triangle Example
#------------------------------------------------------------------------------

set(PROJ_NAME gltriangle)

set(PROJ_HDR
)

set(PROJ_SRC
    triangle.cpp
)

set(PROJ_PROGS
    triangle
)

add_executable(${PROJ_NAME} ${PROJ_HDR} ${PROJ_SRC})

target_link_libraries(${PROJ_NAME} ${GLTUTCOMMON_LIBRARIES})



NX_PROCESS_GPUPROGRAMS(
    ${PROJ_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/data/shaders
    ${CMAKE_CURRENT_BINARY_DIR}/programs
    ${PROJ_PROGS}
)

set(ARCHIVE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}/programs
)

NX_CREATE_ARCHIVE(
    TARGET ${PROJ_NAME}
    ARCHIVE ${CMAKE_CURRENT_BINARY_DIR}/data.yaaf
    DIRS ${ARCHIVE_DIRS}
)
