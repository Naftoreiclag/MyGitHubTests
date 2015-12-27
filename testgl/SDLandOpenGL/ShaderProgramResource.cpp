#include "ShaderProgramResource.hpp"

#include <fstream>

#include "json/json.h"

#include "ResourceManager.hpp"

ShaderProgramResource::ShaderProgramResource()
: mLoaded(false) {
}

ShaderProgramResource::~ShaderProgramResource() {
}

bool ShaderProgramResource::load() {
    if(mLoaded) {
        return true;
    }

    Json::Value progData;

    {
        std::ifstream loader(this->getFile().c_str());
        loader >> progData;
        loader.close();
    }

    mFragOutSymbol = progData["fragOut"].asString();

    const Json::Value& links = progData["link"];
    ResourceManager* rsmngr = ResourceManager::getSingleton();
    for(Json::Value::const_iterator iter = links.begin(); iter != links.end(); ++ iter) {
        const Json::Value& entry = *iter;
        std::string name = entry.asString();
        ShaderResource* shader = rsmngr->findShader(name);
        mLinkedShaders.push_back(shader);
        shader->grab();
    }

    mShaderProg = glCreateProgram();

    for(std::vector<ShaderResource*>::iterator iter = mLinkedShaders.begin(); iter != mLinkedShaders.end(); ++ iter) {
        ShaderResource* shader = *iter;
        glAttachShader(mShaderProg, shader->getHandle());
    }

    glBindFragDataLocation(mShaderProg, 0, mFragOutSymbol.c_str());

    glLinkProgram(mShaderProg);

    for(std::vector<ShaderResource*>::iterator iter = mLinkedShaders.begin(); iter != mLinkedShaders.end(); ++ iter) {
        ShaderResource* shader = *iter;
        glDetachShader(mShaderProg, shader->getHandle());
    }

    const Json::Value& matrices = progData["matrices"];
    const Json::Value& modelMatrix = matrices["model"];
    const Json::Value& viewMatrix = matrices["view"];
    const Json::Value& projMatrix = matrices["proj"];

    if(modelMatrix.isNull()) {
        mUseModelMatrix = false;
    } else {
        mUseModelMatrix = true;
        std::string symbol = modelMatrix.asString();
        mModelMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
    }
    if(viewMatrix.isNull()) {
        mUseViewMatrix = false;
    } else {
        mUseViewMatrix = true;
        std::string symbol = viewMatrix.asString();
        mViewMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
    }
    if(projMatrix.isNull()) {
        mUseProjMatrix = false;
    } else {
        mUseProjMatrix = true;
        std::string symbol = projMatrix.asString();
        mProjMatrixUnif = glGetUniformLocation(mShaderProg, symbol.c_str());
    }

    const Json::Value& controls = progData["controls"];

    const Json::Value& sampler2Ds = controls["sampler2D"];
    for(Json::Value::const_iterator iter = sampler2Ds.begin(); iter != sampler2Ds.end(); ++ iter) {
        const Json::Value& entry = *iter;

        std::string name = entry.asString();

        Sampler2DControl control;
        control.name = entry.asString();
        control.handle = glGetUniformLocation(mShaderProg, control.name.c_str());

        mSampler2Ds.push_back(control);
    }

    mLoaded = true;
    return true;
}

bool ShaderProgramResource::unload() {
    glDeleteProgram(mShaderProg);
    for(std::vector<ShaderResource*>::iterator iter = mLinkedShaders.begin(); iter != mLinkedShaders.end(); ++ iter) {
        ShaderResource* shader = *iter;
        shader->drop();
    }
    mLoaded = false;
    return true;
}

GLuint ShaderProgramResource::getHandle() const { return mShaderProg; }
bool ShaderProgramResource::needsModelMatrix() const { return mUseModelMatrix; }
bool ShaderProgramResource::needsViewMatrix() const { return mUseViewMatrix; }
bool ShaderProgramResource::needsProjMatrix() const { return mUseProjMatrix; }
GLuint ShaderProgramResource::getModelMatrixUnif() const { return mModelMatrixUnif; }
GLuint ShaderProgramResource::getViewMatrixUnif() const { return mViewMatrixUnif; }
GLuint ShaderProgramResource::getProjMatrixUnif() const { return mProjMatrixUnif; }
const std::vector<ShaderProgramResource::Sampler2DControl>& ShaderProgramResource::getSampler2Ds() const { return mSampler2Ds; }
