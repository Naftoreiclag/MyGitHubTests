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

    mMaterial->grab();
    mGeometry->grab();

    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    mGeometry->bindBuffers();

    GLuint locationAttribute = glGetAttribLocation(mMaterial->getShaderProg()->getHandle(), "position");
    mGeometry->enablePositionAttrib(locationAttribute);
    GLuint colorAttribute = glGetAttribLocation(mMaterial->getShaderProg()->getHandle(), "color");
    mGeometry->enableColorAttrib(colorAttribute);
    GLuint texCoordAttribute = glGetAttribLocation(mMaterial->getShaderProg()->getHandle(), "texCoord");
    mGeometry->enableTextureAttrib(texCoordAttribute);

    glBindVertexArray(0);

    mLoaded = true;
    return true;
}
bool ModelResource::unload() {
    mGeometry->drop();
    mMaterial->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);

    mLoaded = false;
    return false;
}

void ModelResource::render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat) {
    glUseProgram(mMaterial->getShaderProg()->getHandle());

    GLuint uModel = glGetUniformLocation(mMaterial->getShaderProg()->getHandle(), "uModel");
    GLuint uView = glGetUniformLocation(mMaterial->getShaderProg()->getHandle(), "uView");
    GLuint uProj = glGetUniformLocation(mMaterial->getShaderProg()->getHandle(), "uProj");

    glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projMat));
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));

    mMaterial->bind();

    glBindVertexArray(mVertexArrayObject);
    mGeometry->render();
    glBindVertexArray(0);

    glUseProgram(0);
}
