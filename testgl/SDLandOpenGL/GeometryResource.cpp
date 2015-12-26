#include "GeometryResource.hpp"

#include <fstream>
#include <iostream>

#include <OpenGLStuff.hpp>

GeometryResource::GeometryResource()
: mLoaded(false) {
}

GeometryResource::~GeometryResource() {
}

uint32_t readU32(std::ifstream& input) {
    uint32_t value;
    input.read((char*) &value, sizeof value);
    return value;
}

bool readBool(std::ifstream& input) {
    char value;
    input.read(&value, sizeof value);
    return value != 0;
}

uint16_t readU16(std::ifstream& input) {
    uint16_t value;
    input.read((char*) &value, sizeof value);
    return value;
}

uint8_t readU8(std::ifstream& input) {
    char value;
    input.read(&value, sizeof value);
    return value;
}

float readF32(std::ifstream& input) {
    float value;
    input.read((char*) &value, sizeof value);
    return value;
}

bool GeometryResource::load() {
    if(mLoaded) {
        return true;
    }

    std::ifstream input(this->getFile().c_str(), std::ios::in | std::ios::binary);

    mUsePositions = readBool(input);
    mUseColor = readBool(input);
    mUseUV = readBool(input);
    mUseNormals = readBool(input);

    mNumVertices = readU32(input);
    mNumTriangles = readU32(input);

    uint32_t positionOff = 0;
    uint32_t colorOff = positionOff + (mUsePositions ? 3 : 0);
    uint32_t textureOff = colorOff + (mUseColor ? 3 : 0);
    uint32_t normalOff = textureOff + (mUseUV ? 2 : 0);
    uint32_t sizeVertices = normalOff + (mUseNormals ? 3 : 0);

    GLfloat vertices[mNumVertices * sizeVertices];

    for(uint32_t i = 0; i < mNumVertices; ++ i) {
        if(mUsePositions) {
            vertices[(i * sizeVertices) + positionOff + 0] = readF32(input);
            vertices[(i * sizeVertices) + positionOff + 1] = readF32(input);
            vertices[(i * sizeVertices) + positionOff + 2] = readF32(input);
        }
        if(mUseColor) {
            vertices[(i * sizeVertices) + colorOff + 0] = readF32(input);
            vertices[(i * sizeVertices) + colorOff + 1] = readF32(input);
            vertices[(i * sizeVertices) + colorOff + 2] = readF32(input);
        }
        if(mUseUV) {
            vertices[(i * sizeVertices) + textureOff + 0] = readF32(input);
            vertices[(i * sizeVertices) + textureOff + 1] = readF32(input);
        }
        if(mUseNormals) {
            vertices[(i * sizeVertices) + normalOff + 0] = readF32(input);
            vertices[(i * sizeVertices) + normalOff + 1] = readF32(input);
            vertices[(i * sizeVertices) + normalOff + 2] = readF32(input);
        }
    }

    GLint indices[mNumTriangles * 3];
    for(uint32_t i = 0; i < mNumTriangles; ++ i) {
        indices[(i * 3) + 0] = readU32(input);
        indices[(i * 3) + 1] = readU32(input);
        indices[(i * 3) + 2] = readU32(input);
    }

    input.close();


    glGenBuffers(1, &mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);

    if(mUsePositions) {
        GLint locationAttribute = glGetAttribLocation(mShaderProg, "position");
        glEnableVertexAttribArray(locationAttribute);
        glVertexAttribPointer(locationAttribute, 3, GL_FLOAT, GL_FALSE, sizeVertices * sizeof(GLfloat), (void*) (positionOff * sizeof(GLfloat)));
    }

    if(mUseColor) {
        GLint colorAttribute = glGetAttribLocation(mShaderProg, "color");
        glEnableVertexAttribArray(colorAttribute);
        glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, sizeVertices * sizeof(GLfloat), (void*) (colorOff * sizeof(GLfloat)));
    }

    if(mUseUV) {
        GLint texCoordAttribute = glGetAttribLocation(mShaderProg, "texCoord");
        glEnableVertexAttribArray(texCoordAttribute);
        glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeVertices * sizeof(GLfloat), (void*) (textureOff * sizeof(GLfloat)));
    }

    /*
    if(mUseNormals) {
        GLint normalsAttribute = glGetAttribLocation(mShaderProg, "texCoord");
        glEnableVertexAttribArray(normalsAttribute);
        glVertexAttribPointer(normalsAttribute, 3, GL_FLOAT, GL_FALSE, sizeVertices * sizeof(GLfloat), (void*) normalOff);
    }
    */

    glBindVertexArray(0);

    return true;
}

bool GeometryResource::unload() {
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);
    glDeleteVertexArrays(1, &mVertexArrayObject);

    mLoaded = false;
    return true;
}

void GeometryResource::render() {
    glBindVertexArray(mVertexArrayObject);
    glDrawElements(GL_TRIANGLES, mNumTriangles * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

GLuint GeometryResource::getHandle() const {
    return mVertexArrayObject;
}

void GeometryResource::bindBuffers() {

}
