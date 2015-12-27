#ifndef ShaderProgramResource_HPP
#define ShaderProgramResource_HPP

#include <string>
#include <vector>

#include <OpenGLStuff.hpp>

#include "Resource.hpp"
#include "ShaderResource.hpp"

class ShaderProgramResource : public Resource {
public:
    struct Sampler2DControl {
        std::string name;
        GLuint handle;
    };

    const std::vector<Sampler2DControl>& getSampler2Ds() const;

private:

    GLuint mShaderProg;
    bool mLoaded;
    std::string mFragOutSymbol;

    std::vector<ShaderResource*> mLinkedShaders;

    std::vector<Sampler2DControl> mSampler2Ds;

    bool mUseModelMatrix;
    GLuint mModelMatrixUnif;

    bool mUseViewMatrix;
    GLuint mViewMatrixUnif;

    bool mUseProjMatrix;
    GLuint mProjMatrixUnif;

public:
    ShaderProgramResource();
    virtual ~ShaderProgramResource();
    
    bool load();
    bool unload();
    
    GLuint getHandle() const;
    bool needsModelMatrix() const;
    bool needsViewMatrix() const;
    bool needsProjMatrix() const;
    GLuint getModelMatrixUnif() const;
    GLuint getViewMatrixUnif() const;
    GLuint getProjMatrixUnif() const;

};

#endif // ShaderProgramResource_HPP
