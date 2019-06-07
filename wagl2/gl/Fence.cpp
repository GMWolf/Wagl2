//
// Created by felix on 5/11/2019.
//

#include "Fence.h"
#include <cassert>
void wagl::gl::Fence::sync() {
    assert(obj == nullptr);
    obj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void wagl::gl::Fence::wait() {
    if(obj) {
        GLenum timeoutflag;
        do {
            timeoutflag = glClientWaitSync(obj, GL_SYNC_FLUSH_COMMANDS_BIT, 10);
        } while (timeoutflag == GL_TIMEOUT_EXPIRED);
        glDeleteSync(obj);
        obj = nullptr;
    }

}

wagl::gl::Fence::~Fence() {
    if (obj) {
        glDeleteSync(obj);
    }
}
