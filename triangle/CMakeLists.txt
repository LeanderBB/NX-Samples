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

set(ARCHIVE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}/programs
)

NX_CREATE_APP(
    NAME GLTriangle
    EXEC_NAME ${PROJ_NAME}
    DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data
    SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data/shaders
    PROGRAMS ${PROJ_PROGS}
    ARCHIVE_DIRS ${ARCHIVE_DIRS}
    SOURCES ${PROJ_SRC}
    HEADERS ${PROJ_HDR}
    PACKAGE "nx.samples"
)

target_link_libraries(${PROJ_NAME} ${GLTUTCOMMON_LIBRARIES})
