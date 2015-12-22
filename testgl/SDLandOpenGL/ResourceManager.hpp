#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <map>

#include <boost/filesystem.hpp>

#include "ImageResource.hpp"
#include "MiscResource.hpp"
#include "Resource.hpp"
#include "StringResource.hpp"
#include "TextureResource.hpp"

class ResourceManager {
public:
    static ResourceManager* getSingleton();
private:
    std::map<std::string, StringResource*> mStrings;
    std::map<std::string, ImageResource*> mImages;
    std::map<std::string, TextureResource*> mTextures;
    std::map<std::string, MiscResource*> mMiscs;

    uint32_t mPermaloadThreshold;

public:
    ResourceManager();
    ~ResourceManager();

    void setPermaloadThreshold(uint32_t size);
    const uint32_t& getPermaloadThreshold();

    void mapAll(boost::filesystem::path data);

    StringResource* findString(std::string name);
    ImageResource* findImage(std::string name);
    TextureResource* findTexture(std::string name);
};


#endif // RESOURCEMANAGER_HPP


