#include "ModelResource.hpp"

#include <fstream>

#include "json/json.h"

#include "ResourceManager.hpp"

ModelResource::ModelResource()
: mLoaded(false) {
}

ModelResource::~ModelResource() {
}

bool ModelResource::load() {
    if(mLoaded) {
        return true;
    }

    ResourceManager* rmgr = ResourceManager::getSingleton();

    Json::Value mdlData;

    {
        std::ifstream loader(this->getFile().c_str());
        loader >> mdlData;
        loader.close();
    }

    const Json::Value& solidsData = mdlData["solids"];

    for(Json::Value::const_iterator iter = solidsData.begin(); iter != solidsData.end(); ++ iter) {
        const Json::Value& solidData = *iter;

        mGeometry = rmgr->findGeometry(solidData["geometry"].asString());
        mMaterial = rmgr->findMaterial(solidData["material"].asString());

        break;
    }

    mGeometry->grab();
    mMaterial->grab();

    mLoaded = true;
    return true;
}
bool ModelResource::unload() {
    mMaterial->drop();
    mGeometry->drop();

    mLoaded = false;
    return false;
}

void ModelResource::render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat) {
    glUseProgram(mMaterial->getShaderProg()->getHandle());

    GLint uModel = glGetUniformLocation(mMaterial->getShaderProg()->getHandle(), "uModel");
    GLint uView = glGetUniformLocation(mMaterial->getShaderProg()->getHandle(), "uView");
    GLint uProj = glGetUniformLocation(mMaterial->getShaderProg()->getHandle(), "uProj");


    glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projMat));
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));

    mMaterial->bind();

    mGeometry->render();

    glUseProgram(0);
}
