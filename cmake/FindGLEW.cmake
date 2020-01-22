#Tries to find GLEW library
#
# Copyright <André Rigland Brodtkorb> Andre.Brodtkorb@sintef.no
#
SET(GLEW_ROOT "C:/Users/stfri/CLionProjects/OSSeminarski/client/nuklear/glew" CACHE PATH "Root to GLEW directory")
MARK_AS_ADVANCED( GLEW_ROOT )

#Find glew library
FIND_LIBRARY(GLEW_LIBRARY
        NAMES GLEW libglew libglew32
        PATHS
        ${GLEW_ROOT}/lib
        )


#Find glew header
FIND_PATH(GLEW_INCLUDE_DIR "GL/glew.h"
        ${GLEW_ROOT}/include
        )

#check that we have found everything
SET(GLEW_FOUND "NO")
IF(GLEW_LIBRARY)
    IF(GLEW_INCLUDE_DIR)
        SET(GLEW_FOUND "YES")
    ENDIF(GLEW_INCLUDE_DIR)
ENDIF(GLEW_LIBRARY)

#Mark options as advanced
MARK_AS_ADVANCED(
        GLEW_INCLUDE_DIR
        GLEW_LIBRARY
)