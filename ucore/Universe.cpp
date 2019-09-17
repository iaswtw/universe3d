#ifdef WINDOWS
#include "pch.h"
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include "Universe.h"
#include "Utils.h"
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>


// Following function was copied from imgui_demo.cpp
// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see misc/fonts/README.txt)
void Universe::HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 1));
        ImGui::BeginTooltip();
        ImGui::PushFont(fixedWidthSmall);

        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        
        ImGui::PopFont();
        ImGui::EndTooltip();
        ImGui::PopStyleColor();
    }
}

static std::string FindFontFile(const char * fileName)
{
    struct stat fileStat;
    std::string fullPath;
    
    fullPath = std::string("fonts/") + fileName;
    if (stat(fullPath.c_str(), &fileStat) == 0)
        return std::string(fullPath);

    fullPath = std::string("../external/fonts/") + fileName;
    if (stat(fullPath.c_str(), &fileStat) == 0)
        return std::string(fullPath);

    // File not found
    return std::string("");
}

Universe::Universe() :
    sunRenderer(sun),
    earthRenderer(earth),
    moonRenderer(moon)
{
}


Universe::~Universe()
{
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::initSceneObjects()
{
    SetDefaultView();
    generateStars();


    axis.generateVertices(
        1600, 1600, 800,
        glm::vec3(0.2f, 0.2f, 0.5f),        // X axis color
        glm::vec3(0.2f, 0.5f, 0.2f),        // Y axis color
        glm::vec3(0.2f, 0.5f, 0.5f)         // Z axis color
    );
    // Sun
    //---------------------------------------
    //sun.setColor(0.7f, 0.7f, 0.1f);
    sun.setColor(1.0f, 1.0f, 0.6f);
    sun.setRotationParameters(140,          // radius
        0,                                  // initial rotation angle
        0.02f,                              // rotation velocity
        glm::radians(0.0f),                 // axis rotation angle
        glm::radians(0.0f)                  // axis tilt angle
    );
    sun.setOrbitalParameters(0,             // radius of orbit
        0.0f,                               // initial orbital angle
        0.01f,                              // revolution velocity
        0,                                  // orbital rotation angle
        0                                   // orbital tilt
    );


    // Earth
    //---------------------------------------
    earth.setColor(0.55f, 0.82f, 1.0f);
    earth.setRotationParameters(80,         // radius
        0,                                  // initial rotation angle
        0.02f,                             // rotation velocity
        glm::radians(270.0f),               // axis rotation angle
        glm::radians(23.5f)                 // axis tilt angle
    );
    earth.setOrbitalParameters(1400,        // radius of orbit
        glm::radians(0.0f),                 // initial orbital angle
        0.001f,                             // revolution velocity
        0.0f,                               // orbital rotation angle
        0                                   // orbital tilt
    );
    earth.setOrbitalPlaneColor(glm::vec3(0.55, 0.82, 1.0));

    // Moon
    //---------------------------------------
    moon.setColor(0.8f, 0.8f, 0.8f);
    moon.setRotationParameters(25,          // radius
        0,                                  // initial rotation angle
        0.005f,                              // rotation velocity
        glm::radians(0.0f),                 // axis rotation angle
        glm::radians(10.0f)                 // axis tilt angle
    );
    moon.setOrbitalParameters(260,          // radius of orbit
        0.0f,                               // initial orbital angle
        0.01f,                             // revolution velocity
        0,                                  // orbital rotation angle
        glm::radians(30.0f)                  // orbital tilt
    );
    moon.setOrbitalPlaneColor(glm::vec3(0.8f, 0.8f, 0.8f));

    moon.setParentSphere(&earth);
    earth.setParentSphere(&sun);
}


/*!
****************************************************************************



****************************************************************************/
void Universe::generateStars()
{
    /*  NOTE:
     *  max_z is the thickness of galaxy at a particular radius
     *  it is dynamically calculated for every randomly generated 'r'
     */


    int i;
    double max_dist;
    double max_radius;
    double max_theta;
    double max_z;
    double radius, theta, phi;
    double w;
    unsigned char color;  // one component of color
    float c;
    float x, y, z, r, g, b;

    srand(20);

    //---------------------------------------------------------------------
    // generate cubically distributed stars
    //---------------------------------------------------------------------
    i = 0;
    max_dist = 500000.0;
    printf("Starting to generate stars\n");
    while (1) {
        if (i == MAXSTARS)
            break;
        
        x = float(max_dist * (double(rand()) / RAND_MAX) - (max_dist / 2));
        y = float(max_dist * (double(rand()) / RAND_MAX) - (max_dist / 2));
        z = float(max_dist * (double(rand()) / RAND_MAX) - (max_dist / 2));
        c = 0.1f + (0.8f * ((float(rand()) / RAND_MAX)));

        r = g = b = c;

        int anotherRandom = int(10 * (float(rand()) / RAND_MAX));
        if (anotherRandom == 5)
            r *= 1.5f;
        if (anotherRandom == 6)
            b *= 1.2f;
        if (anotherRandom == 7)
        {
            r *= 1.5f;
            g *= 1.5f;
        }


        // Discard starts too close to the solar system
        if (fabs(x) > 50000 ||  fabs(y) > 50000 ||  fabs(z) > 50000)
        {
            std::vector<float>* v = (i < 0.75*MAXSTARS) ? &starVertices : &twoPixelWideStarVertices;

            v->push_back(x);  v->push_back(y);  v->push_back(z);   v->push_back(r);  v->push_back(g);  v->push_back(b);  v->push_back(1.0);
            i++;
        }
    }


    //---------------------------------------------------------------------
    // generate stars for the galaxy
    //---------------------------------------------------------------------
    max_radius = 1000000.0;
    max_theta = 2 * M_PI;
    i = 0;
    while (1) {
        if (i == .7 * MAXGALAXYSTARS)
            break;

        radius = max_radius * (double(rand()) / RAND_MAX);
        theta = max_theta * (double(rand()) / RAND_MAX);
        w = radius / max_radius;
        if (radius < 40000)
            max_z = radius;
        else
            max_z = 400000 * (1 / sqrt(1 + 20 * w*w));

        gstar[i].x = radius * cos(theta);
        gstar[i].y = radius * sin(theta);
        gstar[i].z = max_z * double(rand()) / RAND_MAX - max_z / 2;
        color = static_cast<unsigned char>(55) + static_cast<unsigned char>((double(rand()) / RAND_MAX) * 200);
        //c = SDL_MapRGB ( space.surface->format, color, color, color );
        gstar[i].set_color(color, color, color);
               
        if (fabs(gstar[i].x) > 1000 ||
            fabs(gstar[i].y) > 1000 ||
            fabs(gstar[i].z) > 1000)
            i++;
    }

    while (1) {
        if (i == MAXGALAXYSTARS)
            break;

        radius = 200000 * (double(rand()) / RAND_MAX);
        theta = M_PI * (double(rand()) / RAND_MAX);
        phi = 2 * M_PI * (double(rand()) / RAND_MAX);
        gstar[i].x = radius * sin(theta) * cos(phi);
        gstar[i].y = radius * sin(theta) * sin(phi);
        gstar[i].z = radius * cos(theta);
        color = static_cast<unsigned char>((double(rand()) / RAND_MAX) * 255);
        //gstar[i].c = SDL_MapRGB ( space.surface->format, color, color, color );
        gstar[i].set_color(color, color, color);
        i++;

    }

    // shift the Galaxy stars along x axis
    for (i = 0; i < MAXGALAXYSTARS; i++)
        gstar[i].x += max_radius * .66;


    // rotate all stars so that the the plane of the solar system
    // is perpendicular to the plane of the galaxy
    for (i = 0; i < MAXGALAXYSTARS; i++)
        gstar[i] = space.rotate(PNT(0, 0, 0), PNT(100, 0, 0), gstar[i], 80.0);


    for (i = 0; i < MAXGALAXYSTARS; i++)
        gstarVertices.push_back(gstar[i].x);  gstarVertices.push_back(gstar[i].y);  gstarVertices.push_back(gstar[i].z);   gstarVertices.push_back(0.8);  gstarVertices.push_back(0.8);  gstarVertices.push_back(0.8);  gstarVertices.push_back(1.0);

}

/*************************************************************************************************


**************************************************************************************************/
void Universe::printGlError()
{
    GLenum err = glGetError();
    while (err != GL_NO_ERROR)
    {
        std::string error_str;

        switch (err) {
        case GL_INVALID_ENUM:                   error_str = "GL_INVALID_ENUM";                   break;
        case GL_INVALID_VALUE:                  error_str = "GL_INVALID_VALUE";                  break;
        case GL_INVALID_OPERATION:              error_str = "GL_INVALID_OPERATION";              break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error_str = "GL_INVALID_FRAMEBUFFER_OPERATION";  break;
        case GL_OUT_OF_MEMORY:                  error_str = "GL_OUT_OF_MEMORY";                  break;
        case GL_STACK_UNDERFLOW:                error_str = "GL_STACK_UNDERFLOW";                break;
        case GL_STACK_OVERFLOW:                 error_str = "GL_STACK_OVERFLOW";                 break;
        }

        printf("Error: %s\n", error_str.c_str());
        err = glGetError();
    }
}


/*************************************************************************************************


**************************************************************************************************/
void Universe::printShaderCompileStatus(GLuint shader)
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    printf("Shader compile status = %d\n", status);
    if (status == GL_TRUE)
    {
        printf("Compile successful.\n\n");
    }
    else
    {
        printf("!!! === Compile failed === !!!\n");

        char buffer[512];
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        printf("Log:\n%s\n", buffer);

        SDL_Quit();
        exit(1);
    }
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::readAndCompileShader(std::string filePath, GLuint &shaderId)
{
    std::string fileContents;
    std::string line;

    std::ifstream shaderFile(filePath);
    if (shaderFile.fail())
    {
        perror(filePath.c_str());
        printf("Failed to open %s\n", filePath.c_str());
        SDL_Quit();
        exit(1);
    }
    while (std::getline(shaderFile, line))
    {
        fileContents += line + "\n";
    }

    const char *contentsPtr = fileContents.c_str();
    glShaderSource(shaderId, 1, &contentsPtr, nullptr);
    glCompileShader(shaderId);

    // Check compile status
    printShaderCompileStatus(shaderId);
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::compileShaders()
{
    //-------------------------------------------------------------
    // Vertex shader
    //-------------------------------------------------------------
    oglHandles.vertexShader = glCreateShader(GL_VERTEX_SHADER);
    printf("Loading and Compiling Vertex shader.\n");
    readAndCompileShader("../ucore/shaders/shader.vert", oglHandles.vertexShader);

    //-------------------------------------------------------------
    // Fragment shader
    //-------------------------------------------------------------
    oglHandles.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    printf("Loading and Compiling Fragment shader.\n");
    readAndCompileShader("../ucore/shaders/shader.frag", oglHandles.fragmentShader);

    //-------------------------------------------------------------
    // Shader program
    oglHandles.shaderProgram = glCreateProgram();
    glAttachShader(oglHandles.shaderProgram, oglHandles.vertexShader);
    glAttachShader(oglHandles.shaderProgram, oglHandles.fragmentShader);
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::linkShaders()
{
    glLinkProgram(oglHandles.shaderProgram);

    // todo perform check to see if linking was successful
    GLint isLinked = 0;
    glGetProgramiv(oglHandles.shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(oglHandles.shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(oglHandles.shaderProgram, maxLength, &maxLength, &infoLog[0]);

        printf("Linker error log:\n");
        printf("%s\n", infoLog.data());

        // We don't need the program anymore.
        glDeleteProgram(oglHandles.shaderProgram);
    }
    else
    {
        printf("\nShader program link successful\n");
        glDetachShader(oglHandles.shaderProgram, oglHandles.vertexShader);
        glDetachShader(oglHandles.shaderProgram, oglHandles.fragmentShader);
    }

    // Don't leak shaders either.
    glDeleteShader(oglHandles.vertexShader);
    glDeleteShader(oglHandles.fragmentShader);

}


/*************************************************************************************************


**************************************************************************************************/
void Universe::useShaderProgram()
{
    glUseProgram(oglHandles.shaderProgram);
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::unuseShaderProgram()
{
    glUseProgram(0);
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::initializeGL()
{
    printf("Inside initializeGL\n");

    compileShaders();

    glBindAttribLocation(oglHandles.shaderProgram, 0, "position");
    glBindAttribLocation(oglHandles.shaderProgram, 1, "in_color");

    linkShaders();


    //---------------------------------------------------------------------------------------------------
    oglHandles.uniModel = getUniformLocation(oglHandles.shaderProgram, "model");
    oglHandles.uniView = getUniformLocation(oglHandles.shaderProgram, "view");
    oglHandles.uniProj = getUniformLocation(oglHandles.shaderProgram, "proj");

    oglHandles.uniIsStar               = getUniformLocation(oglHandles.shaderProgram, "isStar");
    oglHandles.uniStarPointSize        = getUniformLocation(oglHandles.shaderProgram, "starPointSize");
    oglHandles.uniNightColorMultiplier = getUniformLocation(oglHandles.shaderProgram, "nightColorMultiplier");

    oglHandles.uniMyCenterTransformed  = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.centerTransformed");
    oglHandles.uniMyRadius             = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.radius");
    oglHandles.uniMyIsValud            = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.isValid");
    oglHandles.uniMyIsLightSource      = getUniformLocation(oglHandles.shaderProgram, "sphereInfo.isLightSource");

    oglHandles.uniSunCenterTransformed = getUniformLocation(oglHandles.shaderProgram, "sunCenterTransformed");
    oglHandles.uniSunRadius            = getUniformLocation(oglHandles.shaderProgram, "sunRadius");

    oglHandles.uniOtherSphereCenterTransformed = getUniformLocation(oglHandles.shaderProgram, "otherSphereCenterTransformed");
    oglHandles.uniOtherSphereRadius            = getUniformLocation(oglHandles.shaderProgram, "otherSphereRadius");


    oglHandles.posAttrib = getAttribLocation(oglHandles.shaderProgram, "position");
    oglHandles.colAttrib = getAttribLocation(oglHandles.shaderProgram, "in_color");



    //---------------------------------------------------------------------------------------------------
    // Axis
    glGenVertexArrays(1, &oglHandles.axisVao);
    glBindVertexArray(oglHandles.axisVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * axis.getVertices().size(),
        axis.getVertices().data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------
    // Cube stars - 1 pixel
    glGenVertexArrays(1, &oglHandles.starsVao);
    glBindVertexArray(oglHandles.starsVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * starVertices.size(),
        starVertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------
    // Cube stars - 2 pixel
    glGenVertexArrays(1, &oglHandles.twoPixelWideStarsVao);
    glBindVertexArray(oglHandles.twoPixelWideStarsVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * twoPixelWideStarVertices.size(),
        twoPixelWideStarVertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);


    //---------------------------------------------------------------------------------------------------
    // galaxy stars
    glGenVertexArrays(1, &oglHandles.gstarsVao);
    glBindVertexArray(oglHandles.gstarsVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * gstarVertices.size(),
        gstarVertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(oglHandles.posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    glEnableVertexAttribArray(oglHandles.posAttrib);

    glVertexAttribPointer(oglHandles.colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(oglHandles.colAttrib);

    //---------------------------------------------------------------------------------------------------



	sunRenderer.setAsLightSource();
    sunRenderer.setPolygonCountLevel(PolygonCountLevel_Low);
    sunRenderer.constructVerticesAndSendToGpu(oglHandles);

    
    earthRenderer.setPolygonCountLevel(PolygonCountLevel_High);
    earthRenderer.constructVerticesAndSendToGpu(oglHandles);
    earthRenderer.bShowLatitudesAndLongitudes = true;
    earthRenderer.setNightColorDarkness(NightColorDarkness_Medium);

    
    moonRenderer.setPolygonCountLevel(PolygonCountLevel_Medium);
    moonRenderer.constructVerticesAndSendToGpu(oglHandles);
    moonRenderer.setNightColorDarkness(NightColorDarkness_VeryHigh);


    glBindVertexArray(0);       // Disable VBO


    // Was using the below commented before switching to Space class of the old universe program
    //viewMatrix = glm::lookAt(
    //    glm::vec3(1600.0f, 1600.0f, 1600.0f),
    //    glm::vec3(0.0f, 0.0f, 0.0f),
    //    glm::vec3(0.0f, 0.0f, 1.0f));



    // The following can be used to look at earth at roughly 270 degree position for the purpose of viewing hybrid eclipse.
    //viewMatrix = glm::lookAt(
    //    //            glm::vec3(1600.0f, 1600.0f, 1600.0f),
    //    glm::vec3(-200.0f, -700.0f, 200.0f),
    //    //glm::vec3(0.0f, 0.0f, 0.0f),
    //    glm::vec3(-650.0f, -900.0f, -100.0f),
    //    glm::vec3(0.0f, 0.0f, 1.0f));


}




void Universe::advance(float stepMultiplier)
{
    // Advance parents before children
    sun.advance(stepMultiplier);
    earth.advance(stepMultiplier);
    moon.advance(stepMultiplier);
}

void Universe::onKeyDown(SDL_Event* event)
{
    switch (event->key.keysym.sym) {
    case SDLK_a:
        ChangeBoolean(&bShowAxis, UCmdParam_Toggle);
        break;
    case SDLK_b:
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Toggle);
        break;
    case SDLK_c:
        NavigationLockOntoSun(UCmdParam_Toggle);
        break;
    case SDLK_d:
        SetDefaultView();
        break;
    case SDLK_e:
        Earth_OrbitalPlane(UCmdParam_Toggle);
        break;
    case SDLK_f:
        FastForward(UCmdParam_Start);
        break;
    case SDLK_m:
        Moon_OrbitalPlane(UCmdParam_Toggle);
        break;
    case SDLK_p:
    case SDLK_PAUSE:
    case SDLK_SPACE:
        SimulationPause(UCmdParam_Toggle);
        break;
    case SDLK_r:
        Rewind(UCmdParam_Start);
        break;
    case SDLK_s:
        ChangeBoolean(&bGalaxyStars, UCmdParam_Toggle);
        break;
    case SDLK_v:
        ChangeSidewaysMotionMode();
        break;
    case SDLK_z:
        NavigationLockOntoEarth(UCmdParam_Toggle);
        break;

    case SDLK_0:
        Earth_RevolutionMotion(UCmdParam_Toggle);
        break;
    case SDLK_1:
        Earth_SetOrbitalPositionAngle(0);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_2:
        Earth_SetOrbitalPositionAngle(M_PI / 2);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_3:
        Earth_SetOrbitalPositionAngle(M_PI);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;
    case SDLK_4:
        Earth_SetOrbitalPositionAngle(3 * M_PI / 2);
        Earth_RevolutionMotion(UCmdParam_Off);
        break;



    // Function keys
    case SDLK_F6:
        if (bShiftModifier)
            Earth_PrecessionMotion(UCmdParam_Reset);
        else
            Earth_PrecessionMotion(UCmdParam_Toggle);
        break;
    case SDLK_F5:
        if (bShiftModifier)
            Moon_OrbitalPlaneRotation(UCmdParam_Reset);
        else
            Moon_OrbitalPlaneRotation(UCmdParam_Toggle);
        break;
    case SDLK_F11:
        toggleFullScreen();
        break;

    // Arrow keys
    case SDLK_UP:
        _stepMultiplier *= 1.6666f;
        break;
    case SDLK_DOWN:
        _stepMultiplier *= 0.6f;
        break;


    case SDLK_HOME:
        throttle = nominalThrottle;
        break;
    case SDLK_END:
        throttle = -nominalThrottle;
        break;
    case SDLK_PAGEDOWN:
        yaw = nominalYaw;
        break;
    case SDLK_DELETE:
        yaw = -nominalYaw;
        break;
    case SDLK_PAGEUP:
        if (!bAltModifier)
            pitch = nominalPitch;
        else
            roll = -nominalRoll;
        break;
    case SDLK_INSERT:
        if (!bAltModifier)
            pitch = -nominalPitch; 
        else
            roll = nominalRoll;
        break;

    // Modifiers
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        bCtrlModifier = true;
        break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        bShiftModifier = true;
        break;
    case SDLK_LALT:
    case SDLK_RALT:
        bAltModifier = true;
        break;

    }

}

void Universe::onKeyUp(SDL_Event* event)
{
    switch (event->key.keysym.sym) {
    case SDLK_f:
        FastForward(UCmdParam_Stop);
        break;
    case SDLK_r:
        Rewind(UCmdParam_Stop);
        break;

    case SDLK_HOME:
    case SDLK_END:
        throttle = noThrottle;
        break;
    case SDLK_PAGEDOWN:
    case SDLK_DELETE:
        yaw = noYaw;
        break;
    case SDLK_PAGEUP:
    case SDLK_INSERT:
        roll = noRoll;
        pitch = noPitch;
        break;


        // Modifiers
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        bCtrlModifier = false;
        break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        bShiftModifier = false;
        break;
    case SDLK_LALT:
    case SDLK_RALT:
        bAltModifier = false;
        break;

    }

}

void Universe::onMouseMotion(int xrel, int yrel)
{
    float dx = float(xrel);
    float dy = float(yrel);

    if (bCtrlModifier)
    {
        dx /= 15.0f;
        dy /= 15.0f;
    }

    //printf("dx = %d, dy = %d\n", dx, dy);

    if (!bEarthFollowMode) {
        if (F_REFERENCE_VECTOR_ALONG_Z == 1)
        {
            if (bLeftMouseButtonDown) {
                space.moveFrame(Movement_Forward, -dy * 5);
                if (bLockOntoEarth)
                    space.rotateFrame(earth.getCenter(), dx / 10., 0);
                else if (bLockOntoSun)
                    space.rotateFrame(sun.getCenter(), dx / 10., 0);
            }
            else {
                if (bLockOntoEarth)
                    space.rotateFrame(earth.getCenter(), dx / 10., -dy / 10.);
                else if (bLockOntoSun)
                    space.rotateFrame(sun.getCenter(), dx / 10., -dy / 10.);
            }
            return;
        }
    }

    while (1)
    {
        if (bLeftMouseButtonDown) {
            space.moveFrame(Movement_Forward, -dy * 5);

            if (bEarthFollowMode)
                earthFollowDistance += dy * 5;

            if (!bEarthFollowMode) {
                if (bSidewaysMotionMode == 1) {
                    space.moveFrame(Movement_RotateRight, 90);
                    space.moveFrame(Movement_Forward, dx);
                    space.moveFrame(Movement_RotateLeft, 90);
                }
                else {
                    space.moveFrame(Movement_RotateRight, dx / 10.0);
                }
            }
            break;
        }
        if (!bEarthFollowMode) {
            if (bRightMouseButtonDown) {
                space.moveFrame(Movement_RightAlongSD, dx / 10.0);
                break;
            }

            if (bSidewaysMotionMode) {
                space.moveFrame(Movement_RotateRight, 90);
                space.moveFrame(Movement_Forward, dx);
                space.moveFrame(Movement_RotateLeft, 90);

                space.moveFrame(Movement_RotateUp, 90);
                space.moveFrame(Movement_Forward, -dy);
                space.moveFrame(Movement_RotateDown, 90);

            }
            else {
                space.moveFrame(Movement_RotateRight, (dx / 10.0));
                space.moveFrame(Movement_RotateUp, -(dy / 10.0));
            }
        }
        break;
    }
}


/*!
****************************************************************************



****************************************************************************/
void Universe::ChangeSidewaysMotionMode()
{
    bSidewaysMotionMode = !bSidewaysMotionMode;
    bUpdateUI = true;
}

/*!
****************************************************************************



****************************************************************************/
void Universe::SetDefaultView()
{
    space.pushFrame();

    space.initFrame();
    space.rotateFrame(PNT(0, 0, 0), -10.0, -15.0);
    //space.moveFrame(Movement_Backward, 3400);

    bLockOntoSun = false;
    bLockOntoEarth = false;
    F_REFERENCE_VECTOR_ALONG_Z = 0;

    bUpdateUI = true;
}

/*!
****************************************************************************

 Lock/Unlock/Toggle the earth at the center of the view.  Set the
 reference vector along -Z.

****************************************************************************/
void Universe::NavigationLockOntoEarth(int nParam)
{
    switch (nParam)
    {
    case UCmdParam_On:
        bLockOntoEarth = true;
        bLockOntoSun = false;
        F_REFERENCE_VECTOR_ALONG_Z = 1;

        ChangeBoolean(&earth.bRevolutionMotion, UCmdParam_Off);
        LookAtEarth();
        break;

    case UCmdParam_Off:
        bLockOntoEarth = false;
        F_REFERENCE_VECTOR_ALONG_Z = 0;
        break;

    case UCmdParam_Toggle:
        if (bLockOntoEarth)
        {
            NavigationLockOntoEarth(UCmdParam_Off);
        }
        else
        {
            NavigationLockOntoEarth(UCmdParam_On);
        }
        break;
    }

    bUpdateUI = true;

}

/*!
****************************************************************************

 Lock/Unlock/Toggle the sun at the center of the view.  Set the
 reference vector along -Z.

****************************************************************************/
void Universe::NavigationLockOntoSun(int nParam)
{
    switch (nParam)
    {
    case UCmdParam_On:
        bLockOntoSun = true;
        bLockOntoEarth = false;
        F_REFERENCE_VECTOR_ALONG_Z = 1;
        LookAtSun();
        break;

    case UCmdParam_Off:
        bLockOntoSun = false;
        F_REFERENCE_VECTOR_ALONG_Z = 0;
        break;

    case UCmdParam_Toggle:
        if (bLockOntoSun)
        {
            NavigationLockOntoSun(UCmdParam_Off);
        }
        else
        {
            NavigationLockOntoSun(UCmdParam_On);
        }
        break;
    }

    bUpdateUI = true;

}

void Universe::NavigationLockOntoEarthWithConstantDirection(int nParam)
{
    switch (nParam)
    {
    case UCmdParam_On:
        bEarthFollowMode = true;
        break;

    case UCmdParam_Off:
        bEarthFollowMode = false;
        break;

    case UCmdParam_Toggle:
        ChangeBoolean(&bEarthFollowMode, UCmdParam_Toggle);
        break;
    }

    if (bEarthFollowMode)
    {
        bLockOntoSun = false;
        bLockOntoEarth = false;

        earthFollowVector = VECTOR(space.S, earth.getCenter());
        PNT p = earth.getCenter();
        earthFollowDistance = float(space.S.distanceTo(p));
        bSidewaysMotionMode = false;
    }
    else
    {
        F_REFERENCE_VECTOR_ALONG_Z = 0;
    }

    bUpdateUI = true;
}



void Universe::LookAtEarth()
{
    space.setFrame(AT_POINT,
        space.S,
        VECTOR(space.S, earth.getCenter()),
        PNT(space.S.x, space.S.y, space.S.z - 100));

}

void Universe::LookAtEarthFromSavedVector()
{
    space.setFrame(AT_POINT,
        PNT(earth.getCenter()).translated(-earthFollowDistance, earthFollowVector),
        earthFollowVector,
        PNT(space.S.x, space.S.y, space.S.z - 100));

}

void Universe::LookAtSun()
{
    space.setFrame(AT_POINT,
        space.S,
        VECTOR(space.S, sun.getCenter()),
        PNT(space.S.x, space.S.y, space.S.z - 100));

}


/*!
****************************************************************************

 Set simulation speed to one of the 5 hardcoded values.

****************************************************************************/
void Universe::SetSimulationSpeed(int nParam)
{
    eSimulationSpeed = USimulationSpeedType(nParam);
    switch (eSimulationSpeed)
    {
    case USimulationSpeed_VeryLow:
        _stepMultiplier = 0.005f;
        break;

    case USimulationSpeed_Low2:
        _stepMultiplier = 0.015f;
        break;

    case USimulationSpeed_Low1:
        _stepMultiplier = 0.03f;
        break;

    case USimulationSpeed_Low:
        _stepMultiplier = 0.05f;
        break;

    case USimulationSpeed_Normal:
        _stepMultiplier = 0.36f;
        break;

    case USimulationSpeed_High:
        _stepMultiplier = 3.0f;
        break;

    case USimulationSpeed_VeryHigh:
        _stepMultiplier = 10.0f;
        break;
    }

    bUpdateUI = true;
}

/*!
****************************************************************************

 Set time flow direction to either Forward or Reverse depending on the
 argument.

****************************************************************************/
void Universe::SetTimeDirection(int nParam)
{
    eTimeDirection = (UTimeDirectionType)nParam;
    bUpdateUI = true;
}

/*!
****************************************************************************

 Turn On/Off Fast forward motion.

****************************************************************************/
void Universe::FastForward(int nParam)
{
    ChangeBoolean(&bFastForward, nParam);
}

/*!
****************************************************************************

 Turn On/Off fast Reverse motion.

****************************************************************************/
void Universe::Rewind(int nParam)
{
    ChangeBoolean(&bFastReverse, nParam);
}


/*!
****************************************************************************

 Set, Reset or Toggle a given boolean

 This function operates on a given boolean & sets the boolean to True/False
 or toggles it depending on the second argument which is actually an
 enum of type UCmdParamType.

 Sets bUpdateUI to true unconditionally.

****************************************************************************/
void Universe::ChangeBoolean(bool *pBool, int nParam)
{
    switch (nParam)
    {
    case UCmdParam_Toggle:
        *pBool = !*pBool;
        break;

    case UCmdParam_On:
    case UCmdParam_Start:
        *pBool = true;
        break;

    case UCmdParam_Off:
    case UCmdParam_Stop:
        *pBool = false;
        break;
    }

    bUpdateUI = true;

}


/*!
****************************************************************************



****************************************************************************/
void Universe::SimulationPause(int nParam)
{
    ChangeBoolean(&bSimulationPause, nParam);
    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_RotationMotion(int nParam)
{
    ChangeBoolean(&earth.bRevolutionMotion, nParam);
    bUpdateUI = true;
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_RevolutionMotion(int nParam)
{
    ChangeBoolean(&earth.bRevolutionMotion, nParam);
    //F_REFERENCE_VECTOR_ALONG_Z = 0;
    //bLockOntoEarth = false;

    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_PrecessionMotion(int nParam)
{
    if (nParam == UCmdParam_Reset)
    {
        earth.bPrecessionMotion = false;
        earth.setAxisRotationAngle(glm::radians(270.0f));
    }
    else
    {
        ChangeBoolean(&earth.bPrecessionMotion, nParam);
    }

    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_OrbitalPlane(int nParam)
{
    ChangeBoolean(&earthRenderer.bShowOrbitalPlane, nParam);
    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_OrbitalPlane(int nParam)
{
    ChangeBoolean(&moonRenderer.bShowOrbitalPlane, nParam);
    bUpdateUI = true;

}


/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_OrbitalPlaneRotation(int nParam)
{

    if (nParam == UCmdParam_Reset)
    {
        moon.bOrbitalPlaneRotation = false;
        moon._orbitalPlaneRotationAngle = glm::radians(0.0f);
    }
    else
    {
        ChangeBoolean(&moon.bOrbitalPlaneRotation, nParam);
    }

    bUpdateUI = true;
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_RevolutionMotion(int nParam)
{
    ChangeBoolean(&moon.bRevolutionMotion, nParam);
    bUpdateUI = true;
}


/*!
****************************************************************************



****************************************************************************/
void Universe::SetDotDensity(int nParam)
{
    eDotDensity = (UDotDensityType)nParam;
    switch (eDotDensity)
    {
    case UDotDensity_Normal:
        // todo
        //inc_multiplier = 0.8;
        break;

    case UDotDensity_High:
        // todo
        //inc_multiplier = 0.5;
        break;
    }
}


/*!
****************************************************************************



****************************************************************************/
void Universe::ShowDemo(int nParam)
{
    PNT newS;

    switch ((UDemoType)nParam)
    {
    case UDemo_TotalSolarEclipse:

        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_On);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(-323.5104, 957.3688, 296.3317);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.2f * M_PI / 5);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_Low1);
        SimulationPause(UCmdParam_Off);

        /* F_REFERENCE_VECTOR_ALONG_Z is checked before bLockOntoEarth
           or bLockOntoSun in the function on_MouseMotion().  Need
           to consider if the priority of this check should be reversed.
           Without the setting below, the Lock on earth or sun won't
           work. */
        F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;

        break;
    
    case UDemo_TotalSolarEclipseOnNorthPole:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_On);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(733.4838, 817.9659, 297.3985);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.7f * M_PI / 5);
        Earth_SetOrbitalPositionAngle(0.69f * M_PI / 2);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_Low);
        SimulationPause(UCmdParam_Off);

        F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;
        break;

    case UDemo_AnnularSolarEclipseFromSpace:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_Off);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_Off);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        // the hardcoded values here were found by printing the value of S & D on screen using ImGui
        // while manually going to that position.
        newS = PNT(-92.2673, 1435.5368, 52.9889);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, PNT(641.3813, -2678.9711, -409.2932)),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(-3.3f * M_PI / 5);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_Low2);
        SimulationPause(UCmdParam_Off);

        F_REFERENCE_VECTOR_ALONG_Z = 0;
        bSidewaysMotionMode = false;

        bUpdateUI = true;
        break;

    case UDemo_PartialLunarEclipse:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_On);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        //newS = PNT(earth.getCenter().x + 500, earth.getCenter().y - 700, earth.getCenter().z + 150);
        newS = PNT(1196.26, 462.93, -60.55);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        /* Set proper moon's position so that the moon's shadow will
           fall on the earth shortly */
        Moon_SetOrbitalPositionAngle(0.35f * M_PI / 2);
        Earth_SetOrbitalPositionAngle(0.59f * M_PI / 2);

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);

        // Increase the dot density
        SetDotDensity(UDotDensity_High);
        SetSimulationSpeed(USimulationSpeed_Low);
        SimulationPause(UCmdParam_Off);

        F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;
        break;

    case UDemo_PrecessionMotion:
        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        // Let the moon hide below the earth's orbital plane so that
        // it won't distract the user
        Moon_SetOrbitalPositionAngle(0);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_On);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        // Set S
        newS = PNT(earth.getCenter().x + 300, earth.getCenter().y - 400, earth.getCenter().z + 300);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));


        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_Off);
        Earth_RevolutionMotion(UCmdParam_Off);
        Earth_PrecessionMotion(UCmdParam_Start);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_Off);

        // Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_Normal);
        SimulationPause(UCmdParam_Off);

        /* F_REFERENCE_VECTOR_ALONG_Z is checked before bLockOntoEarth
           or bLockOntoSun in the function on_MouseMotion().  Need
           to consider if the priority of this check should be reversed.
           Without the setting below, the Lock on earth or sun won't
           work. */
        F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;

        break;

    case UDemo_SixMonthLongDayAndNightOnNorthPole:

        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI / 2);

        // Set S
        newS = PNT(earth.getCenter().x + 250, earth.getCenter().y - 350, earth.getCenter().z + 300);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        NavigationLockOntoEarthWithConstantDirection(UCmdParam_On);
        // Adjust navigation view locks on earth and sun

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        //// Adjust Moon's motions
        //Moon_RevolutionMotion(UCmdParam_Off);

        //// Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_High);
        SimulationPause(UCmdParam_Off);

        ///* F_REFERENCE_VECTOR_ALONG_Z is checked before bLockOntoEarth
        //   or bLockOntoSun in the function on_MouseMotion().  Need
        //   to consider if the priority of this check should be reversed.
        //   Without the setting below, the Lock on earth or sun won't
        //   work. */
        //F_REFERENCE_VECTOR_ALONG_Z = 1;

        bUpdateUI = true;

        break;

    case UDemo_SixMonthLongDayAndNightOnNorthPole_AnotherAngle:

        // Set earth at (0,R,0)
        Earth_SetOrbitalPositionAngle(M_PI + M_PI / 4);

        // Set S
        newS = PNT(earth.getCenter().x, earth.getCenter().y - 350, earth.getCenter().z + 100);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, earth.getCenter()),
            PNT(newS.x, newS.y, newS.z - 100));

        NavigationLockOntoEarthWithConstantDirection(UCmdParam_On);
        // Adjust navigation view locks on earth and sun

        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        //// Adjust Moon's motions
        //Moon_RevolutionMotion(UCmdParam_Off);

        //// Adjust orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_Off);

        SetDotDensity(UDotDensity_Normal);
        SetSimulationSpeed(USimulationSpeed_High);
        SimulationPause(UCmdParam_Off);

        break;

    case UDemo_TiltedOrbitalPlanes:
        Earth_SetOrbitalPositionAngle(M_PI / 2 - M_PI / 10);

        // Tilted orbit demo must show both the orbital planes
        Earth_OrbitalPlane(UCmdParam_On);
        Moon_OrbitalPlane(UCmdParam_On);

        // Adjust navigation view locks on earth and sun
        NavigationLockOntoEarth(UCmdParam_Off);
        NavigationLockOntoSun(UCmdParam_Off);
        NavigationLockOntoEarthWithConstantDirection(UCmdParam_Off);

        newS = PNT(earth.getCenter().x + 0, earth.getCenter().y + 1300, earth.getCenter().z + 200);
        space.setFrame(AT_POINT,
            newS,
            VECTOR(newS, PNT(-700, 0, -300)),
            PNT(newS.x, newS.y, newS.z - 100));


        // Adjust earth's motions
        Earth_RotationMotion(UCmdParam_On);
        Earth_RevolutionMotion(UCmdParam_On);
        Earth_PrecessionMotion(UCmdParam_Reset);

        // Adjust Moon's motions
        Moon_RevolutionMotion(UCmdParam_On);
        Moon_SetOrbitalPositionAngle(0.0f);

        SetSimulationSpeed(USimulationSpeed_Normal);
        SetDotDensity(UDotDensity_Normal);
        SimulationPause(UCmdParam_Off);


        bUpdateUI = true;

        break;
    }
}


void Universe::toggleFullScreen()
{
    if (bIsWindowFullScreen) {
        SDL_SetWindowFullscreen(window, 0);
        bIsWindowFullScreen = false;
    }
    else {
        printf("  Changing to fullscreen\n");
        //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);        
        bIsWindowFullScreen = true;
    }
}


void Universe::cleanupAndExitApplication()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Moon_SetOrbitalPositionAngle(double fAngle)
{
    moon.setOrbitalAngle(float(fAngle));
    moon.calculateCenterPosition();
}

/*!
****************************************************************************



****************************************************************************/
void Universe::Earth_SetOrbitalPositionAngle(double fAngle)
{
    earth.setOrbitalAngle(float(fAngle));
    earth.calculateCenterPosition();
}


void Universe::setWidgetControlMode()
{
    SDL_ShowCursor(SDL_ENABLE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void Universe::resetWidgetControlMode()
{
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    useShaderProgram();

    glUniform3fv(
        oglHandles.uniSunCenterTransformed,
        1,
        glm::value_ptr(sun.getModelTransformedCenter())
    );

    glUniform1f(oglHandles.uniSunRadius, sun.getRadius());


    //=====================================================================================
    // View and projection remain same for the entire scene
    //=====================================================================================
    // View transformation
    //----------------------------------------------
    glUniformMatrix4fv(oglHandles.uniView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Create the initial View matrix
    viewMatrix = glm::lookAt(
        space.getSourcePoint(),
        space.getDirectionPoint(),
        space.getUpwardDirectionVector());

    // perspective transformation
    //----------------------------------------------
    projectionMatrix = glm::perspective(
        glm::radians(35.0f),
        float(curWidth) / float(curHeight),
        1.0f,
        10000000.0f);

    glUniformMatrix4fv(oglHandles.uniProj, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    //=====================================================================================

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    if (bShowAxis)
    {
        //----------------------------------------------
        // Axis model transformation
        //----------------------------------------------
        glUniformMatrix4fv(
            oglHandles.uniModel,
            1,
            GL_FALSE,
            glm::value_ptr(glm::mat4(1.0))
        );

        glUniform1i(oglHandles.uniMyIsValud, false);
        // ideally, after setting IsValid to false, no need to set the other variables to draw the axis.
        glUniform1i(oglHandles.uniMyIsLightSource, 0);
        glUniform3f(oglHandles.uniMyCenterTransformed, 0.0f, 0.0f, 0.0f);

        glBindVertexArray(oglHandles.axisVao);

        // Draw vertices
        glDrawArrays(GL_LINES, 0, axis.getVertices().size() / 7);
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    glUniform1i(oglHandles.uniIsStar, true);
    glUniform1i(oglHandles.uniMyIsValud, false);
    glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 1.0f);

    if (!bGalaxyStars)
    {
        //----------------------------------------------
        // Cubic stars model transformation
        //----------------------------------------------
        glUniformMatrix4fv(
            oglHandles.uniModel,
            1,
            GL_FALSE,
            glm::value_ptr(glm::mat4(1.0))
        );

        glUniform1ui(oglHandles.uniStarPointSize, 1);
        glBindVertexArray(oglHandles.starsVao);
        // Draw vertices
        glDrawArrays(GL_POINTS, 0, starVertices.size() / 7);

        glUniform1ui(oglHandles.uniStarPointSize, 2);
        glDrawArrays(GL_POINTS, 0, twoPixelWideStarVertices.size() / 7);
    }
    else
    {
        //----------------------------------------------
        // Galaxy stars model transformation
        //----------------------------------------------
        glUniformMatrix4fv(
            oglHandles.uniModel,
            1,
            GL_FALSE,
            glm::value_ptr(glm::mat4(1.0))
        );

        glUniform1i(oglHandles.uniMyIsValud, false);
        glBindVertexArray(oglHandles.gstarsVao);
        glUniform1ui(oglHandles.uniStarPointSize, 1);

        // Draw vertices
        glDrawArrays(GL_POINTS, 0, gstarVertices.size() / 7);
    }

    glDisable(GL_PROGRAM_POINT_SIZE);
    glUniform1i(oglHandles.uniIsStar, false);

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    earthRenderer.render(oglHandles, &moon);
    moonRenderer.render(oglHandles, &earth);
    sunRenderer.render(oglHandles, nullptr);


    glBindVertexArray(0);

    unuseShaderProgram();

}


/*************************************************************************************************


**************************************************************************************************/
void Universe::processFlags()
{
    // Accept the current values of the 4 motions into local variables
    float __throttle = throttle;
    float __yaw = yaw;
    float __pitch = pitch;
    float __roll = roll;
    

    // Amply or attenuate the value based on keyboard modifiers.
    if (bCtrlModifier)
        __throttle /= 10;
    else if (bShiftModifier)
        __throttle *= 100;

    if (bCtrlModifier)
        __yaw /= 10;
    else if (bShiftModifier)
        __yaw *= 10;

    if (bCtrlModifier)
        __pitch /= 100;
    else if (bShiftModifier)
        __pitch *= 10;

    if (bCtrlModifier)
        __roll /= 10;
    else if (bShiftModifier)
        __roll *= 100;

    // Finally, Apply the motion value.
    if (__throttle != 0.0f) {
        space.moveFrame(Movement_Forward, __throttle);
        if (bEarthFollowMode)
            earthFollowDistance -= __throttle;
    }

    if ((__yaw != 0.0f) || (__pitch != 0.0f))
    {
        if (bSidewaysMotionMode) {
            space.moveFrame(Movement_RotateRight, 90);
            space.moveFrame(Movement_Forward, __yaw*50);
            space.moveFrame(Movement_RotateLeft, 90);

            space.moveFrame(Movement_RotateUp, 90);
            space.moveFrame(Movement_Forward, __pitch*50);
            space.moveFrame(Movement_RotateDown, 90);
        }
        else {
            space.moveFrame(Movement_RotateRight, __yaw);
            space.moveFrame(Movement_RotateUp, __pitch);
        }
    }
    if (__roll != 0.0f)
        space.moveFrame(Movement_RightAlongSD, -__roll);        // todo - why the -ve sign?



    if (!bSimulationPause)
    {
        if (bFastForward)
            advance(5 * _stepMultiplier);
        else if (bFastReverse)
            advance(-5 * _stepMultiplier);
        else
            advance(_stepMultiplier);
    }


    if (bLockOntoEarth)
        LookAtEarth();
    if (bLockOntoSun)
        LookAtSun();
    if (bEarthFollowMode)
        LookAtEarthFromSavedVector();


}


void Universe::generateImGuiWidgets()
{
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    // Always showing overlay window showing status of various flags
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(5.0f, io.DisplaySize.y - 5.0f), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Flags", &bShowFlagsOverlay, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMouseInputs))
    {

        ImVec4 onColor  = ImColor(0.3f, 0.7f, 0.3f);
        ImVec4 offColor = ImColor(0.0f, 0.0f, 0.0f);
        ImVec4 color;

        if (bSidewaysMotionMode) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("v"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (bLockOntoEarth) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("z"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (!earth.bRevolutionMotion) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("0"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (bSimulationPause) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("Pause"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (earthRenderer.bShowOrbitalPlane) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("e"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (moonRenderer.bShowOrbitalPlane) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("m"); ImGui::SameLine();
        ImGui::PopStyleColor();

        if (bEarthFollowMode) color = onColor; else color = offColor;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::Button("b");
        ImGui::PopStyleColor();

    }
    ImGui::End();

    if (bShowKeyboardShortcuts)
    {
        //ImGuiCond_FirstUseEver
        ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(1024, 768));
        if (ImGui::Begin("Keyboard Shortcuts", &bShowKeyboardShortcuts))
        {
            auto populateShortcutWindow = [](const char *arr[][2], int numRows) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1));
                for (int i = 0; i < numRows; i++)
                {
                    if (arr[i][0] == nullptr) {
                        ImGui::Separator();
                    }
                    else {
                        ImGui::Text(arr[i][0]);  ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 100);
                        ImGui::TextWrapped(arr[i][1]);  ImGui::NextColumn();
                    }
                }
                ImGui::PopStyleColor();
            };

            const char * shortcutHelp[][2] = {
            { "Space bar",      "Pause time. Navigation will still work." },
            { "f",              "Pressing this key and keeping it pressed will cause simulation time to elapse faster by about 10 times."},
            { "r",              "Pressing this key and keeping it pressed with cause simulation time to run in reverse by about 10 times the nominal forward speed. "
                                "Releasing this key shall restore the forward movement of time at the speed it was just before pressing this key."},
            { "Up arrow",       "Speeds up the simulation time passage by roughly 66% each time this key is pressed."},
            { "Down arrow",     "Slows down the simulation time passage by roughly 66% each time this key is pressed."},
            { "d",              "Bring the camera to the default position and look in the direction of the sun. From this position, the entire earth's orbit is visible. "
                                " It is a convenient starting position for the simulation.  If you get lost navigating, you can hit this key to get your bearings."},
            { nullptr, nullptr },

            { "a",              "Toggle visibility of the XYZ coordinate axis.  When looking from the default view position, +ve X direction is towards bottom left from origin (blue color),"
                                "+ve Y is towards bottom right from origin (green color), and +ve Z is upwards from origin (cyan color)." },
            { "e",              "Toggle visibility of earth's orbital plane." },
            { "m",              "Toggle visibility of moon's orbital plane." },
            
            { "z",              "Toggle lock on earth's position. When turned on, the earth will appear at the center of the screen. Left/right/up/down mouse movements shall rotate the camera around the earth at a constant distance. "
                                "Zoom in and zoom out will work as expected.  Turning lock on will also cause the earth to pause in its orbit." },
            { "c",              "Toggle lock on sun's position. When turned on, the sun will appear at the center of the screen." },
            { "b",              "Toggle directional lock on earth. The direction vector from the camera to the earth will be noted at the time of turning this mode on. "
                                "After that, the camera will follow the earth as the earth moves in its orbit, all the while maintaining the original direction vector."
                                "This mode is used in the 6 month long day and night demo.  Use this mode when you want to observe the earth from a fixed angle as it "
                                "revolves around the sun.  Contrast this mode with the normal lock on earth (z) mode where the camera stays put while only changing its viewing direction. "
                                "While in this mode, only zoom in/out navigation will work." },
            { "0 (zero)",       "Toggle earth's revolution motion.  Use this to pause the earth in its orbit so that you can observe various things such as the tilted orbit, shadows, etc." },


            { nullptr, nullptr },

            { "Home",           "Zoom in" },
            { "End",            "Zoom out" },
            { "Del",            "Turn left (or shift left in Shift mode)"},
            { "Page Down",      "Turn right (or shift right in Shift mode)"},
            { "Page Up",        "Rotate up (or shift up in Shift mode)"},
            { "Insert",         "Rotate doen (or shift down in Shift mode"},
            { "Alt + Page Up",  "Rotate right along the axis the camera is looking at. This will result in the object in front being rotated left."},
            { "Alt + Insert",   "Rotate left along the axis the camera is looking at. This will result in the object in front being rotated right."},
            { "v",              "Toggle 'Shift mode' navigation.  In this mode, left/right/up/down mouse movements will result in the camera being shifted in those directions as "
                                "as opposed to rotating in those directions.  When the camera shifts, the viewing direction vector does not change. It is as if the camera is looking at a point at infinity. "
                                "If you want to look at how the background stars shift due to parallex as the earth moves in its orbit, turn this mode on, go to default "
                                "view (d), and then move the mouse left and right." },


            { nullptr, nullptr },

            { "F5",             "Start/Stop rotation of moon's orbital plane. This is best seen if moon's orbital plane is also visible." },
            { "Shift + F5",     "Reset moon's orbital plane tilt to default." },

            { "F6",             "Start/Stop earth's precession motion." },
            { "Shift + F6",     "Reset earth's axis tilt direction to default." },

            { nullptr, nullptr },

            { "1",              "Set earth's position at 0 degrees measured from the positive X axis (blue)." },
            { "2",              "Set earth's position at 90 degrees measured from the positive X axis (blue)." },
            { "3",              "Set earth's position at 180 degrees measured from the positive X axis (blue)." },
            { "4",              "Set earth's position at 270 degrees measured from the positive X axis (blue)." },

            {nullptr, nullptr },

            };

            ImGui::Columns(2, "mycolumn");
            ImGui::Text("Shortcut"); ImGui::NextColumn();  ImGui::SetColumnOffset(-1, 100);
            ImGui::Text("Description"); ImGui::NextColumn();
            ImGui::Separator();

            populateShortcutWindow(shortcutHelp, sizeof(shortcutHelp) / sizeof(shortcutHelp[0]));
        }
        ImGui::End();

    }


    if (bMouseGrabbed)
    {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2.0f, 10.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(0.35f);
        if (ImGui::Begin("Escape message", &bShowFlagsOverlay, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("Escape key / double-click to get mouse back");
        }
        ImGui::End();
    }
    else
    {
        // Show menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Universe3d"))
            {
                if (ImGui::MenuItem("Exit Application"))
                    bQuit = true;

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Show Fullscreen", "F11"))
                    toggleFullScreen();
                ImGui::MenuItem("Show Control Panel in Navigation mode", nullptr, &bAlwaysShowControlPanel, true);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::MenuItem("Show Keyboard Shortcuts", nullptr, &bShowKeyboardShortcuts);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (!bMouseGrabbed || bAlwaysShowControlPanel)
    {
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 5.0f, 27.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        //ImGui::SetNextWindowSize(ImVec2(350.0f, curHeight - 25.0f));
        ImGui::SetNextWindowBgAlpha(0.8f);

        {
            // Create a window called "Hello, world!" and append into it.
            ImGui::Begin(
                "Control Panel",
                nullptr,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
            );
            static float f = 0.0f;
            ImGui::PushFont(appFontSmall);
            if (ImGui::CollapsingHeader("Demos", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::PushFont(appFontSmall);

                ImGui::SetNextItemWidth(190);
                if (ImGui::Button("Total Solar Eclipse## demo"))
                    ShowDemo(UDemo_TotalSolarEclipse);

                if (ImGui::Button("Total Solar Eclipse on north pole## demo"))
                    ShowDemo(UDemo_TotalSolarEclipseOnNorthPole);

                if (ImGui::Button("Annular Solar Eclipse from space## demo"))
                    ShowDemo(UDemo_AnnularSolarEclipseFromSpace);
                ImGui::SameLine();
                HelpMarker("This also shows umbra starting to travel over the earth near the bottom left of the screen. This happens roughly halfway between 3rd and 4th contact for a duration of about 2 to 3 seconds. So watch out!");

                if (ImGui::Button("Partial Lunar Eclipse## demo"))
                    ShowDemo(UDemo_PartialLunarEclipse);

                if (ImGui::Button("Tilted orbital planes## demo"))
                    ShowDemo(UDemo_TiltedOrbitalPlanes);

                if (ImGui::Button("6 month long day & 6 month long\nnight on north pole"))
                    ShowDemo(UDemo_SixMonthLongDayAndNightOnNorthPole);

                if (ImGui::Button("6 month long day & 6 month long\nnight on north pole (another angle)"))
                    ShowDemo(UDemo_SixMonthLongDayAndNightOnNorthPole_AnotherAngle);

                if (ImGui::Button("Precession motion## demo"))
                    ShowDemo(UDemo_PrecessionMotion);

                ImGui::PopFont();
            }
            ImGui::PopFont();
            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Earth:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();
            ImGui::Indent();
            ImGui::Checkbox("Revolution motion (0)## earth", &earth.bRevolutionMotion);
            ImGui::Checkbox("Orbital plane (e)## earth", &earthRenderer.bShowOrbitalPlane);
            ImGui::Checkbox("Precession motion (F6)## earth", &earth.bPrecessionMotion);
            ImGui::SameLine();
            if (ImGui::Button("Reset## earth precession motion"))
                Earth_PrecessionMotion(UCmdParam_Reset);
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Moon:");               // Display some text (you can use a format strings too)
            ImGui::PopFont();

            ImGui::Indent();
            ImGui::Checkbox("Revolution motion", &moon.bRevolutionMotion);
            ImGui::Checkbox("Orbital plane (m)##moon", &moonRenderer.bShowOrbitalPlane);
            ImGui::Checkbox("Orbital plane rotation (F5)", &moon.bOrbitalPlaneRotation);
            ImGui::SameLine();
            if (ImGui::Button("Reset## moon orbital plane rotation"))
                Moon_OrbitalPlaneRotation(UCmdParam_Reset);
            ImGui::Unindent();
            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Navigation:");
            ImGui::PopFont();

            ImGui::Indent();
            ImGui::Checkbox("Shift mode (v)", &bSidewaysMotionMode);
            ImGui::SameLine(); HelpMarker("When checked, Shift left/right/up/down on mouse movements.\nWhen unchecked, rotate instead.");
            ImGui::Checkbox("Lock on earth (z)", &bLockOntoEarth);
            if (ImGui::IsItemEdited())
                NavigationLockOntoEarth(bLockOntoEarth ? UCmdParam_On : UCmdParam_Off);
            ImGui::SameLine(); HelpMarker("Also pauses earth's revolution. Activate this mode and\nthen use mouse to view earth from different angles.");

            ImGui::Checkbox("Directional lock on earth (b)", &bEarthFollowMode);
            if (ImGui::IsItemEdited())
                NavigationLockOntoEarthWithConstantDirection(bEarthFollowMode ? UCmdParam_On : UCmdParam_Off);
            ImGui::SameLine();
            HelpMarker("Earth follow mode");

            ImGui::Checkbox("Lock on sun (c)", &bLockOntoSun);
            if (ImGui::IsItemEdited())
                NavigationLockOntoSun(bLockOntoSun ? UCmdParam_On : UCmdParam_Off);

            if (ImGui::Button("Show default view (d)"))
                SetDefaultView();
            ImGui::Unindent();

            ImGui::Separator();

            //-----------------------------------------------------

            ImGui::PushFont(appFontSmall);
            ImGui::Text("Time:");
            ImGui::PopFont();
            ImGui::Indent();

            ImGui::Checkbox("Time pause (space bar)", &bSimulationPause);

            ImGui::Button("Fast Rewind (r)");
            if (ImGui::IsItemActivated())       Rewind(UCmdParam_On);
            if (ImGui::IsItemDeactivated())     Rewind(UCmdParam_Off);

            ImGui::SameLine();
            ImGui::Button("Fast Forward (f)");
            if (ImGui::IsItemActivated())       FastForward(UCmdParam_On);
            if (ImGui::IsItemDeactivated())     FastForward(UCmdParam_Off);

            static int timeSpeed = 0;
            ImGui::SetNextItemWidth(100);
            ImGui::Combo("Time Speed", &timeSpeed, "Very Slow\0Slow\0Normal\0Fast\0Very Fast\0\0");
            // todo - actually apply this value in the app

            ImGui::Unindent();

            //-----------------------------------------------------

            ImGui::Separator();
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Separator();
            ImGui::Text("S: %.4f, %.4f, %.4f", space.S.x, space.S.y, space.S.z);
            ImGui::Text("D: %.4f, %.4f, %.4f", space.D.x, space.D.y, space.D.z);
            ImGui::Text("E orbital angle: %.4f", earth._orbitalAngle);
            ImGui::End();
        }

        // 3. Show another simple window.
        //if (show_another_window)
        //{
        //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //    ImGui::Text("Hello from another window!");
        //    if (ImGui::Button("Close Me"))
        //        show_another_window = false;
        //    ImGui::End();
        //}

        // Rendering
    }

    ImGui::Render();
}

/*************************************************************************************************


**************************************************************************************************/
int Universe::run()
{
    setvbuf(stdout, 0, _IONBF, 0);
    const char* glsl_version = "#version 130";

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow("Universe3d", 100, 100, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    printf("Created SDL GL window\n");
    context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    glewInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    std::string fullFontFilePath = FindFontFile("Roboto-Medium.ttf");
    appFontSmall        = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 16);
    appFontSmallMedium  = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 18);
    appFontMedium       = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 20);
    appFontLarge        = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 24);

    fullFontFilePath = FindFontFile("ProggyClean.ttf");
    fixedWidthSmall     = io.Fonts->AddFontFromFileTTF(fullFontFilePath.c_str(), 13);

    if (!appFontSmall || !appFontSmallMedium || !appFontMedium || !appFontLarge || !fixedWidthSmall)
        printf("WARNING: Could not load fonts.  Will use default fixed width font.");

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init(glsl_version);


    SDL_GetWindowSize(window, &curWidth, &curHeight);
    printf("width = %d\n", curWidth);
    printf("height = %d\n", curHeight);
    
    printf("initializing scene objects... ");
    initSceneObjects();
    initializeGL();
    printf("done\n");


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);


    SDL_GetMouseState(&previousX, &previousY);

    SDL_Event event;
    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type)
            {
            case SDL_QUIT:           bQuit = true;                  break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    setWidgetControlMode();
                    bMouseGrabbed = false;
                }
                else
                    onKeyDown(&event);
                break;
            case SDL_KEYUP:
                onKeyUp(&event);
                break;
            case SDL_WINDOWEVENT:
                if ((event.window.event == SDL_WINDOWEVENT_RESIZED) || 
                    (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
                    curWidth = event.window.data1;
                    curHeight = event.window.data2;
                    glViewport(0, 0, curWidth, curHeight);
                }
                break;
            }

            //----------------------------------------------------
            // Pass mouse events to universe3d application only if ImGui isn't using them.
            if (!io.WantCaptureMouse)
            {
                switch (event.type)
                {
                case SDL_MOUSEBUTTONDOWN:
                    switch (event.button.button) {
                    case SDL_BUTTON_LEFT:
                        if (event.button.clicks == 1) {
                            if (!doubleClicked.get() && !bMouseGrabbed) {
                                resetWidgetControlMode();
                                bMouseGrabbed = true;
                            }
                        }
                        if (event.button.clicks == 2) {
                            if (bMouseGrabbed) {
                                doubleClicked.set(50);
                                setWidgetControlMode();
                                bMouseGrabbed = false;
                            }
                        }
                        bLeftMouseButtonDown = true;
                        break;
                    case SDL_BUTTON_RIGHT:
                        bRightMouseButtonDown = true;
                        break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    switch (event.button.button) {
                    case SDL_BUTTON_LEFT:   bLeftMouseButtonDown = false;   break;
                    case SDL_BUTTON_RIGHT:  bRightMouseButtonDown = false;  break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (bMouseGrabbed)
                        onMouseMotion(event.motion.xrel, event.motion.yrel);
                    break;
                case SDL_MOUSEWHEEL:
                    if (bMouseGrabbed) {
                        // Hack
                        bool oldValue = bLeftMouseButtonDown;
                        bLeftMouseButtonDown = true;
                        onMouseMotion(-event.wheel.x * 10, -event.wheel.y * 10);
                        bLeftMouseButtonDown = oldValue;
                    }
                    break;
                }
            }

            if (bQuit)
                break;

        } // while SDL event poll
        
        generateImGuiWidgets();

        if (bQuit)
            break;

        doubleClicked.tick();
        processFlags();
        render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }

    cleanupAndExitApplication();
    return 0;
}