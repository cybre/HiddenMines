#Tries to find GLUT library
#
# Copyright <André Rigland Brodtkorb> Andre.Brodtkorb@sintef.no
#
SET(GLUT_ROOT "C:/Users/stfri/CLionProjects/OSSeminarski/client/nuklear/freeglut" CACHE PATH "Root to GLUT directory")
MARK_AS_ADVANCED( GLUT_ROOT )

#Find glew library
FIND_LIBRARY(GLUT_LIBRARY
        NAMES GLUT libfreeglut
        PATHS
        ${GLUT_ROOT}/lib/x64
        )


#Find glut header
FIND_PATH(GLUT_INCLUDE_DIR "GL/glut.h"
        ${GLUT_ROOT}/include
        )

#check that we have found everything
SET(GLUT_FOUND "NO")
IF(GLUT_LIBRARY)
    IF(GLUT_INCLUDE_DIR)
        SET(GLUT_FOUND "YES")
    ENDIF(GLUT_INCLUDE_DIR)
ENDIF(GLUT_LIBRARY)

#Mark options as advanced
MARK_AS_ADVANCED(
        GLUT_INCLUDE_DIR
        GLUT_LIBRARY
)