#include <iostream>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <GL/glew.h>
#include <SDL2/SDL.h>

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
    
    SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
    
    SDL_GL_SetSwapInterval(1);
    
    glewInit();
    
    while(true) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            
        }
    
        // Display yellow
        glClearColor(1.f, 1.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(sdlWindow);
    }
    
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    
    return 0;
}

