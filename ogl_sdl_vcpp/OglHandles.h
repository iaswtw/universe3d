#pragma once


#include <GL/glew.h>

struct OglHandles
{
    GLuint shaderProgram;
    GLuint vertexShader;
    GLuint fragmentShader;


    GLint posAttrib;
    GLint colAttrib;

    GLint uniColor;
    GLint uniModel;
    GLint uniView;
    GLint uniProj;

    GLuint axisVao;

    GLint uniMyCenterTransformed;
    //GLint uniMyRadius;
    GLint uniMyIsValud;
    GLint uniMyIsLightSource;

    GLint uniOtherSphereCenterTransformed;
    GLint uniOtherSphereRadius;

    GLint uniSunCenterTransformed;
    GLint uniSunRadius;

};
