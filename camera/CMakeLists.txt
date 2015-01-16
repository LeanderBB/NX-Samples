#------------------------------------------------------------------------------
# Virtual Camera Example
#------------------------------------------------------------------------------

set(PROJ_NAME glcamera)

set(PROJ_HDR
)

set(PROJ_SRC
    glcamera.cpp
)

set(PROJ_PROGS
    camera
)

set(ARCHIVE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}/programs
    ${CMAKE_CURRENT_SOURCE_DIR}/data/models
)

#add_executable(${PROJ_NAME} ${PROJ_HDR} ${PROJ_SRC})

add_definitions(-DNX_GLTUTAPP_NAME=${PROJ_NAME})

NX_CREATE_APP(
    NAME GLCamera
    EXEC_NAME ${PROJ_NAME}
    DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data
    SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data/shaders
    ARCHIVE_NAME data
    PROGRAMS ${PROJ_PROGS}
    ARCHIVE_DIRS ${ARCHIVE_DIRS}
    SOURCES ${PROJ_SRC}
    HEADERS ${PROJ_HDR}
)

target_link_libraries(${PROJ_NAME} ${GLTUTCOMMON_LIBRARIES})

#NX_PROCESS_GPUPROGRAMS(
#    ${PROJ_NAME}
#    ${CMAKE_CURRENT_SOURCE_DIR}/data
#    ${CMAKE_CURRENT_SOURCE_DIR}/data/shaders
#    ${CMAKE_CURRENT_BINARY_DIR}/programs
#    ${PROJ_PROGS}
#)

#NX_CREATE_ARCHIVE(
#    TARGET ${PROJ_NAME}
#    ARCHIVE ${CMAKE_CURRENT_BINARY_DIR}/data.yaaf
#    DIRS ${ARCHIVE_DIRS}
#    )
