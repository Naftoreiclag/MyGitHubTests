#include "TextureResource.hpp"

#include <fstream>

#include <GL/gl.h>

#include "json/json.h"

#include "ImageResource.hpp"
#include "ResourceManager.hpp"

TextureResource::TextureResource()
: mLoaded(false) {
}

TextureResource::~TextureResource() {
}

bool TextureResource::load() {
    if(mLoaded) {
        return true;
    }

    Json::Value textureData;
    {
        std::ifstream loader(this->getFile().c_str());
        loader >> textureData;
        loader.close();
    }

    ResourceManager* rmgr = ResourceManager::getSingleton();

    glBindTexture(GL_TEXTURE_2D, mHandle);
    ImageResource* imageRes = rmgr->findImage(textureData["image"].asString());
    imageRes->grab();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageRes->getWidth(), imageRes->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, imageRes->getImage());
    imageRes->drop();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    mLoaded = true;
    return true;
}

bool TextureResource::unload() {
    glDeleteTextures(1, &mHandle);
    mLoaded = false;
    return true;
}

GLuint TextureResource::getHandle() const {
    return mHandle;
}
