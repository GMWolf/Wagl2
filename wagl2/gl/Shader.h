//
// Created by felix on 15/01/2019.
//

#ifndef WAGL2_SHADER_H
#define WAGL2_SHADER_H

#include <utility>
#include <string>
#include <cstddef>
#include "Object.h"
#include "util/FlatMap.h"

namespace wagl {

    namespace gl {

        struct ShaderBinary {
            GLenum format;
            GLint length;
            std::vector<std::byte> buffer;
        };

        class Shader : Object {
        public:

            Shader(std::initializer_list<std::pair<GLenum, std::string>> sources);

            ~Shader();

            bool compiled();

            void use() const;

            void getInterfaceiv(GLenum interface, GLenum pname, GLint *params);

            GLuint getResourceIndex(GLenum interface, const char *name);

            void getResourceiv(GLenum interface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSise,
                               GLsizei *length, GLint *params);

            void getResourceName(GLenum interface, GLuint index, GLsizei bufsize, GLsizei *length, char *name);

            GLint getResourceLocation(GLenum interface, const char *name);

            GLint getResourceLocationIndex(GLenum interface, const char *name);

            ShaderBinary getBinary();

            //void uniformBlockBinding(const std::string& blockName, const GLuint bindIndex);

        private:
            static bool checkShader(GLuint shader, const char *source);

            static bool checkProgram(GLuint program);

            void registerUniforms();
            /*
            Flatmap<std::string, GLint> uniformLocations;
            Flatmap<std::string, GLint> uniformBlockIndex;*/
            /*std::vector<std::string> uniformNames;
            std::vector<std::string> uniformBlockNames;*/

        };

    }

}



#endif //WAGL2_SHADER_H
