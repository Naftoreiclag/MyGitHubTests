#include <iostream>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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
    
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    
    GLfloat vertices[] = {
         0.6f,  0.6f,  2.6f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         0.6f, -0.6f,  2.6f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.6f,  0.6f,  2.6f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        -0.6f, -0.6f,  2.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    GLuint indexBufferObject;
    glGenBuffers(1, &indexBufferObject);
    
    GLuint indices[] = {
        0, 2, 1,
        3, 1, 2
    };
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    boost::filesystem::path resourceDef = "../../../resources/data.package";
    ResourceManager* resman = ResourceManager::getSingleton();
    resman->mapAll(resourceDef);
    
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

    GLint locationAttribute = glGetAttribLocation(shaderProg, "position");
    glEnableVertexAttribArray(locationAttribute);
    glVertexAttribPointer(locationAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

    GLint colorAttribute = glGetAttribLocation(shaderProg, "color");
    glEnableVertexAttribArray(colorAttribute);
    glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    GLint texCoordAttribute = glGetAttribLocation(shaderProg, "texCoord");
    glEnableVertexAttribArray(texCoordAttribute);
    glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

    TextureResource* textureData = resman->findTexture("GreenJellyfish.texture");
    textureData->grab();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureData->getHandle());
    glUniform1i(glGetUniformLocation(shaderProg, "ambientTex"), 0);

    glUseProgram(shaderProg);

    GLint uModel = glGetUniformLocation(shaderProg, "uModel");
    GLint uView = glGetUniformLocation(shaderProg, "uView");
    GLint uProj = glGetUniformLocation(shaderProg, "uProj");

    glm::mat4 viewMat = glm::lookAt(glm::vec3(0.f, 2.f, -2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1280.f / 720.f, 1.f, 10.f);
    glm::mat4 modelMat;

    bool running = true;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }

        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(viewMat));
        glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projMat));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));

        glClearColor(1.f, 1.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(sdlWindow);
    }

    textureData->drop();
    
    glDeleteProgram(shaderProg);
    glDeleteShader(fragShader);
    glDeleteShader(vertShader);
    
    glDeleteBuffers(1, &indexBufferObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
    
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    
    return 0;
}
