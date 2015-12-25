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

    mLoaded = true;
    return true;
}

bool ShaderProgramResource::unload() {
    glDeleteProgram(mShaderProg);
    for(std::vector<ShaderResource*>::iterator iter = mLinkedShaders.begin(); iter != mLinkedShaders.end(); ++ iter) {
        ShaderResource* shader = *iter;
        shader->drop();
    }
    return true;
}

GLuint ShaderProgramResource::getHandle() const {
    return mShaderProg;
}
