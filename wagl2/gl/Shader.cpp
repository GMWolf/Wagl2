//
// Created by felix on 15/01/2019.
//

#include <vector>
#include <iostream>
#include "Shader.h"

wagl::gl::Shader::Shader(std::initializer_list<std::pair<GLenum, std::string>> sources) {

    std::vector<GLuint> shaders;
    shaders.reserve(sources.size());

    //Compile all shaders
    for(const auto& [shaderType, shaderText]  : sources) {
        GLuint sh = glCreateShader(shaderType);
        const char* c_text = shaderText.c_str();
        glShaderSource(sh, 1, &c_text, NULL);
        glCompileShader(sh);

        bool compiled = checkShader(sh, "TODO");
        if (compiled) {
            shaders.push_back(sh);
        } else {
            //Did not compile. Cleanup and exit
            for(auto& s : shaders) {
                glDeleteShader(s);
            }
            glDeleteShader(sh);
            return;
        }
    }

    id = glCreateProgram();

    //attach all shaders
    for(auto sh : shaders) {
        glAttachShader(id, sh);
    }

    //link
    glLinkProgram(id);

    //Detach and delete all shaders
    for(auto sh : shaders) {
        glDetachShader(id, sh);
        glDeleteShader(sh);
    }

    bool linked = checkProgram(id);
    if (!linked) {
        glDeleteProgram(id);
        id = 0;
        return;
    }

    registerUniforms();
}

void wagl::gl::Shader::use() const {
    glUseProgram(id);
}

bool wagl::gl::Shader::compiled() {
    return id != 0;
}

bool wagl::gl::Shader::checkShader(GLuint shader, const char *source) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        std::cerr << "Could not compile shader " << source << ".\n";
        char error[2048];
        glGetShaderInfoLog(shader, sizeof(error), nullptr, error);
        std::cerr << error << "\n";
        return false;
    }

    return true;
}

bool wagl::gl::Shader::checkProgram(GLuint program) {
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        std::cerr << "Could not link shader" << "\n";
        char error[2048];
        glGetProgramInfoLog(program, sizeof(error), nullptr, error);
        std::cerr << error << std::endl;
        return false;
    }

    return true;
}

wagl::gl::Shader::~Shader() {
    glDeleteProgram(id);
}

wagl::gl::ShaderBinary wagl::gl::Shader::getBinary() {
    GLint bufsize;
    glGetProgramiv(id, GL_PROGRAM_BINARY_LENGTH, &bufsize);


    ShaderBinary binary;
    binary.buffer.resize(static_cast<size_t>(bufsize));

    glGetProgramBinary(id, bufsize, &binary.length, &binary.format, binary.buffer.data());

    return binary;
}

void wagl::gl::Shader::getInterfaceiv(GLenum interface, GLenum pname, GLint *params) {
    glGetProgramInterfaceiv(id, interface, pname, params);
}

GLuint wagl::gl::Shader::getResourceIndex(GLenum interface, const char *name) {
    return glGetProgramResourceIndex(id, interface, name);
}

void
wagl::gl::Shader::getResourceiv(GLenum interface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSise,
                            GLsizei *length, GLint *params) {
    glGetProgramResourceiv(id, interface, index, propCount, props, bufSise, length, params);
}

void wagl::gl::Shader::getResourceName(GLenum interface, GLuint index, GLsizei bufsize, GLsizei *length, char *name) {
    glGetProgramResourceName(id, interface, index, bufsize, length, name);
}

GLint wagl::gl::Shader::getResourceLocation(GLenum interface, const char *name) {
    return glGetProgramResourceLocation(id, interface, name);
}

GLint wagl::gl::Shader::getResourceLocationIndex(GLenum interface, const char *name) {
    return glGetProgramResourceLocationIndex(id, interface, name);
}

void wagl::gl::Shader::registerUniforms() {
/*
    {
        GLint uniformCount = 0;
        getInterfaceiv(GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniformCount);

        const GLenum propEnums[] = {GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION};

        union {
            GLint values[4]{};
            struct {
                GLint blockIndex;
                GLint type;
                GLint nameLength;
                GLint location;
            };
        } properties;

        for (GLuint i = 0; i < uniformCount; i++) {
            getResourceiv(GL_UNIFORM, i, 4, propEnums, 4, nullptr, properties.values);

            if (properties.blockIndex != -1) continue; //skip uniforms in blocks

            std::string name(static_cast<size_t>(properties.nameLength), ' ');
            getResourceName(GL_UNIFORM, i, static_cast<GLsizei>(name.length()), nullptr, name.data());

            uniformLocations.insert(name, properties.location);
            //uniformNames.push_back(name)
        }
    }

    {
        //blocks
        GLint numBlocks = 0;
        getInterfaceiv(GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);

        const GLenum blockProperties[] = {GL_BUFFER_BINDING, GL_NAME_LENGTH, GL_BLOCK_INDEX};

        for (GLuint blockIdx = 0; blockIdx < numBlocks; blockIdx++) {
            union {
                GLint values[3]{};
                struct {
                    GLint blockBinding;
                    GLint nameLength;
                    GLint blockIndex;
                };
            } properties;

            getResourceiv(GL_UNIFORM_BLOCK, blockIdx, 2, blockProperties, 2, nullptr, properties.values);
            std::string name(properties.nameLength, ' ');
            getResourceName(GL_UNIFORM_BLOCK, blockIdx, static_cast<GLsizei>(name.length()), nullptr, name.data());
            name.pop_back();

            uniformBlockIndex.insert(name, properties.blockIndex);
            //uniformBlockNames.push_back(name);
        }
    }
    */
}
/*
void wagl::gl::Shader::uniformBlockBinding(const std::string &blockName, const GLuint bindIndex) {
    glUniformBlockBinding(id, uniformBlockIndex.at(blockName), bindIndex);
}
*/