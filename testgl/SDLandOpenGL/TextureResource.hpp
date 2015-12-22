#ifndef TEXTURERESOURCE_HPP
#define TEXTURERESOURCE_HPP

#include <GL/gl.h>

#include "Resource.hpp"

class TextureResource : public Resource {
private:
    GLuint mHandle;
    bool mLoaded;
public:
    TextureResource();
    virtual ~TextureResource();
    
    bool load();
    bool unload();
    
    GLuint getHandle() const;

};

#endif // TEXTURERESOURCE_HPP
