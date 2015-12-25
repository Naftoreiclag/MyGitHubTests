#include "ResourceManager.hpp"

#include <fstream>
#include <iostream>

#include "json/json.h"

ResourceManager* ResourceManager::getSingleton() {
    static ResourceManager instance;

    return &instance;
}

ResourceManager::ResourceManager()
: mPermaloadThreshold(0) {
}

ResourceManager::~ResourceManager() {
}

void ResourceManager::setPermaloadThreshold(uint32_t size) {
    mPermaloadThreshold = size;
}
const uint32_t& ResourceManager::getPermaloadThreshold() {
    return mPermaloadThreshold;
}

void ResourceManager::mapAll(boost::filesystem::path dataPackFile) {
    Json::Value dataPackData;
    {
        std::ifstream reader(dataPackFile.c_str());
        reader >> dataPackData;
        reader.close();
    }
    
    boost::filesystem::path dataPackDir = dataPackFile.parent_path();
    
    const Json::Value& resourcesData = dataPackData["resources"];
    
    for(Json::Value::const_iterator iter = resourcesData.begin(); iter != resourcesData.end(); ++ iter) {
        const Json::Value& resourceData = *iter;
        
        std::string resType = resourceData["type"].asString();
        std::string name = resourceData["name"].asString();
        std::string file = resourceData["file"].asString();
        uint32_t size = resourceData["size"].asInt();
        
        Resource* newRes;
        if(resType == "text" || resType == "fragment-shader" || resType == "vertex-shader") {
            newRes = mStrings[name] = new StringResource();
        } else if(resType == "image") {
            newRes = mImages[name] = new ImageResource();
        } else if(resType == "texture") {
            newRes = mTextures[name] = new TextureResource();
        } else if(resType == "geometry") {
            newRes = mGeometries[name] = new GeometryResource();
        } else {
            newRes = mMiscs[name] = new MiscResource();
        }
        
        newRes->setName(name);
        newRes->setFile(dataPackDir / file);
        newRes->setSize(size);
        if(size < mPermaloadThreshold) {
            newRes->grab();
        }
        
    }
}

StringResource* ResourceManager::findString(std::string name) {
    return mStrings[name];
}
ImageResource* ResourceManager::findImage(std::string name) {
    return mImages[name];
}
TextureResource* ResourceManager::findTexture(std::string name) {
    return mTextures[name];
}
GeometryResource* ResourceManager::findGeometry(std::string name) {
    return mGeometries[name];
}
