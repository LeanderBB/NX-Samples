#------------------------------------------------------------------------------
# Shadows Example
#------------------------------------------------------------------------------

set(PROJ_NAME glshadows)

set(PROJ_HDR
)

set(PROJ_SRC
    src/main.cpp
)

set(PROJ_PROGS
    shadow-camera
    shadow-light
    shadow-light-view
)

set(ARCHIVE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}/programs
    ${CMAKE_CURRENT_SOURCE_DIR}/data/models
)

NX_CREATE_APP(
    NAME GLShadows
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
