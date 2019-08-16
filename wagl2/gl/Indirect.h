//
// Created by felix on 11/08/2019.
//

#ifndef CODE_INDIRECT_H
#define CODE_INDIRECT_H
#include <glad/glad.h>
namespace wagl::gl {
    struct DrawElementsIndirectCommand {
        GLuint count;
        GLuint instanceCount;
        GLuint firstIndex;
        GLuint baseVertex;
        GLuint baseInstance;
    };


}
#endif //CODE_INDIRECT_H
