#include <iostream>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <OpenGLStuff.hpp>
#include <SDL2/SDL.h>

#include "ResourceManager.hpp"
    
int main(int argc, char* argv[]) {

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Error" << std::endl;
        return -1;
    }
    
    SDL_Window* sdlWindow = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    
    if(!sdlWindow) {
        std::cout << "SDL Window Error" << std::endl;
        return -1;
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // Use OpenGL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    // Enable double-buffers
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Create SDL context
    SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
    SDL_GL_SetSwapInterval(1);
    
    // Use experimental drivers #yolo
    glewExperimental = GL_TRUE;
    glewInit();

    glClearColor(0.098f, 0.486f, 0.502f, 1.f);
    glEnable(GL_DEPTH_TEST);

    boost::filesystem::path resourceDef = "../../../resources/data.package";
    ResourceManager* resman = ResourceManager::getSingleton();
    resman->mapAll(resourceDef);

    // SHADER DATA

    /*
    StringResource* vertText = resman->findString("Hello.vertexShader");
    StringResource* fragText = resman->findString("Hello.fragmentShader");

    vertText->grab();
    fragText->grab();

    const GLchar* vertSrc = vertText->getString().c_str();
    const GLchar* fragSrc = fragText->getString().c_str();

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSrc, 0);
    glCompileShader(vertShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSrc, 0);
    glCompileShader(fragShader);

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertShader);
    glAttachShader(shaderProg, fragShader);
    glBindFragDataLocation(shaderProg, 0, "fragColor");
    glLinkProgram(shaderProg);
    glDetachShader(shaderProg, vertShader);
    glDetachShader(shaderProg, fragShader);

    */

    VertexShaderResource* vertexShaderRes = (VertexShaderResource*) resman->findShader("Hello.vertexShader");
    FragmentShaderResource* fragShaderRes = (FragmentShaderResource*) resman->findShader("Hello.fragmentShader");
    fragShaderRes->grab();
    vertexShaderRes->grab();

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShaderRes->getHandle());
    glAttachShader(shaderProg, fragShaderRes->getHandle());
    glBindFragDataLocation(shaderProg, 0, "fragColor");
    glLinkProgram(shaderProg);
    glDetachShader(shaderProg, vertexShaderRes->getHandle());
    glDetachShader(shaderProg, fragShaderRes->getHandle());

    /*
    ShaderProgramResource* shaderProgRes = resman->findShaderProgram("Hello.shaderProgram");
    shaderProgRes->grab();
    GLuint shaderProg = shaderProgRes->getHandle();
    */

    // MESH DATA

    GeometryResource* benvolio = resman->findGeometry("Cube.geometry");
    benvolio->mShaderProg = shaderProg;
    benvolio->grab();

    // TEXTURE DATA

    TextureResource* textureData = resman->findTexture("128Rose.texture");
    textureData->grab();

    // SCENE DATA

    GLint uModel = glGetUniformLocation(shaderProg, "uModel");
    GLint uView = glGetUniformLocation(shaderProg, "uView");
    GLint uProj = glGetUniformLocation(shaderProg, "uProj");
    GLint uTex = glGetUniformLocation(shaderProg, "ambientTex");

    glm::mat4 viewMat = glm::lookAt(glm::vec3(0.f, 2.f, -2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1280.f / 720.f, 1.f, 10.f);
    glm::mat4 modelMat;

    uint32_t prev = SDL_GetTicks();

    bool running = true;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }

        uint32_t now = SDL_GetTicks();
        double tps = now - prev;
        prev = now;

        modelMat = glm::rotate(modelMat, glm::radians((float) (tps * 0.1)), glm::vec3(0.0f, 1.0f, 0.0f));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glUseProgram(shaderProg);

        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(viewMat));
        glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projMat));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureData->getHandle());
        glUniform1i(uTex, 0);

        benvolio->render();

        SDL_GL_SwapWindow(sdlWindow);
    }

    textureData->drop();

    //shaderProgRes->drop();

    /*
    glDeleteProgram(shaderProg);
    glDeleteShader(fragShader);
    glDeleteShader(vertShader);
    */
    benvolio->drop();
    
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    
    return 0;
}
