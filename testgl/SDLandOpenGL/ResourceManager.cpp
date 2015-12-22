#include "ResourceManager.hpp"

#include <fstream>
#include <iostream>

#include "json/json.h"

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
            newRes = mTexts[name] = new StringResource();
        } else if(resType == "image") {
            newRes = mImages[name] = new ImageResource();
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

StringResource* ResourceManager::findText(std::string name) {
    return mTexts[name];
}
ImageResource* ResourceManager::findImage(std::string name) {
    return mImages[name];
}
