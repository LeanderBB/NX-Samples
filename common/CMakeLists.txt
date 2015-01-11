#------------------------------------------------------------------------------
# Common functions
#------------------------------------------------------------------------------

set(PROJ_NAME gltutcommon)

set(PROJ_HDR
    gltutapp.h
)

set(PROJ_SRC
    gltutapp.cpp
)

add_library(${PROJ_NAME} STATIC  ${PROJ_HDR} ${PROJ_SRC})

set(GLTUTCOMMON_LIBRARIES ${PROJ_NAME} ${NX_LIBRARIES} PARENT_SCOPE)
