/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Framebuffer.h"

#include "BufferImage.h"
#include "Image.h"

namespace Magnum {

void Framebuffer::mapForDraw(std::initializer_list<std::int8_t> colorAttachments) {
    GLenum* attachments = new GLenum[colorAttachments.size()];
    for(auto it = colorAttachments.begin(); it != colorAttachments.end(); ++it)
        attachments[it-colorAttachments.begin()] = *it + GL_COLOR_ATTACHMENT0;

    bind(Target::Draw);
    /** @todo Re-enable when extension wrangler is available for ES2 */
    #ifndef MAGNUM_TARGET_GLES2
    glDrawBuffers(colorAttachments.size(), attachments);
    #endif
    delete[] attachments;
}

}
