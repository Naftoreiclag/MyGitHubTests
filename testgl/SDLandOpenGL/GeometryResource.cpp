#include "GeometryResource.hpp"

#include <fstream>
#include <iostream>

#include <GL/gl.h>

GeometryResource::GeometryResource()
: mLoaded(false) {
}

GeometryResource::~GeometryResource() {
}

uint32_t readU32(std::ifstream& input) {
    uint8_t data[4];

    input >> data[0];
    input >> data[1];
    input >> data[2];
    input >> data[3];

    uint32_t value;
    value += data[0] << 24;
    value += data[1] << 16;
    value += data[2] << 8;
    value += data[3];

    return value;
}

bool readBool(std::ifstream& input) {
    uint8_t data;

    input >> data;

    return data > 0;
}

uint16_t readU16(std::ifstream& input) {
    uint16_t data[2];

    input >> data[0];
    input >> data[1];

    uint16_t value;
    value += data[0] << 8;
    value += data[1];

    return value;
}

uint8_t readU8(std::ifstream& input) {
    uint8_t data;

    input >> data;

    return data;
}

float readF32(std::ifstream& input) {
    uint8_t data[4];

    input >> data[0];
    input >> data[1];
    input >> data[2];
    input >> data[3];

    uint32_t value;
    value += data[0] << 24;
    value += data[1] << 16;
    value += data[2] << 8;
    value += data[3];

    float& friar = *((float*) &value);

    return friar;
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
        indices[(i * 3) + 0] = readF32(input);
        indices[(i * 3) + 1] = readF32(input);
        indices[(i * 3) + 2] = readF32(input);
    }

    input.close();

    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    glGenBuffers(1, &mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    if(mUsePositions) {
        GLint locationAttribute = 0;//glGetAttribLocation(shaderProg, "position");
        glEnableVertexAttribArray(locationAttribute);
        glVertexAttribPointer(locationAttribute, 3, GL_FLOAT, GL_FALSE, sizeVertices * sizeof(GLfloat), (void*) positionOff);
    }

    if(mUseColor) {
        GLint colorAttribute = 0;//glGetAttribLocation(shaderProg, "color");
        glEnableVertexAttribArray(colorAttribute);
        glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, sizeVertices * sizeof(GLfloat), (void*) colorOff);
    }

    if(mUseUV) {
        GLint texCoordAttribute = 0;//glGetAttribLocation(shaderProg, "texCoord");
        glEnableVertexAttribArray(texCoordAttribute);
        glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeVertices * sizeof(GLfloat), (void*) textureOff);
    }

    if(mUseNormals) {
        GLint normalsAttribute = 0;//glGetAttribLocation(shaderProg, "texCoord");
        glEnableVertexAttribArray(normalsAttribute);
        glVertexAttribPointer(normalsAttribute, 3, GL_FLOAT, GL_FALSE, sizeVertices * sizeof(GLfloat), (void*) normalOff);
    }

    glGenBuffers(1, &mIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return true;
}

bool GeometryResource::unload() {
    mLoaded = false;
    return true;
}

GLuint GeometryResource::getHandle() const {
    return mVertexArrayObject;
}
