#ifndef ShaderProgramResource_HPP
#define ShaderProgramResource_HPP

#include <string>
#include <vector>

#include <OpenGLStuff.hpp>

#include "Resource.hpp"
#include "ShaderResource.hpp"

class ShaderProgramResource : public Resource {
private:
    GLuint mShaderProg;
    bool mLoaded;
    std::string mFragOutSymbol;

    std::vector<ShaderResource*> mLinkedShaders;

public:
    ShaderProgramResource();
    virtual ~ShaderProgramResource();
    
    bool load();
    bool unload();
    
    GLuint getHandle() const;

};

#endif // ShaderProgramResource_HPP
