#ifndef Magnum_AbstractFramebuffer_h
#define Magnum_AbstractFramebuffer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class @ref Magnum::AbstractFramebuffer, enum @ref Magnum::FramebufferClear, @ref Magnum::FramebufferBlit, @ref Magnum::FramebufferBlitFilter, @ref Magnum::FramebufferTarget, enum set @ref Magnum::FramebufferClearMask
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/Math/Range.h"

namespace Magnum {

/**
@brief Mask for framebuffer clearing

@see @ref AbstractFramebuffer, @ref FramebufferClearMask
*/
enum class FramebufferClear: GLbitfield {
    Color = GL_COLOR_BUFFER_BIT,    /**< Color buffer */
    Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth buffer */
    Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil buffer */
};

/**
@brief Mask for clearing

@see @ref AbstractFramebuffer::clear()
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
typedef Containers::EnumSet<FramebufferClear,
    GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> FramebufferClearMask;
#else
typedef Containers::EnumSet<FramebufferClear> FramebufferClearMask;
#endif

/**
@brief Mask for framebuffer blitting

@see @ref AbstractFramebuffer, @ref FramebufferBlitMask
@requires_gl30 Extension @extension{ARB,framebuffer_object}
@requires_gles30 Extension @es_extension{ANGLE,framebuffer_blit} or
    @es_extension{NV,framebuffer_blit} in OpenGL ES 2.0
*/
enum class FramebufferBlit: GLbitfield {
    #ifdef MAGNUM_BUILD_DEPRECATED
    /** @copydoc FramebufferBlit::Color
     * @deprecated Use @ref Magnum::FramebufferBlit::Color "FramebufferBlit::Color" instead.
     */
    ColorBuffer = GL_COLOR_BUFFER_BIT,

    /** @copydoc FramebufferBlit::Depth
     * @deprecated Use @ref Magnum::FramebufferBlit::Depth "FramebufferBlit::Depth" instead.
     */
    DepthBuffer = GL_DEPTH_BUFFER_BIT,

    /** @copydoc FramebufferBlit::Stencil
     * @deprecated Use @ref Magnum::FramebufferBlit::Stencil "FramebufferBlit::Stencil" instead.
     */
    StencilBuffer = GL_STENCIL_BUFFER_BIT,
    #endif

    Color = GL_COLOR_BUFFER_BIT,    /**< Color buffer */
    Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth buffer */
    Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil buffer */
};

/**
@brief Mask for framebuffer blitting

@see @ref AbstractFramebuffer::blit()
@requires_gl30 Extension @extension{ARB,framebuffer_object}
@requires_gles30 Extension @es_extension{ANGLE,framebuffer_blit} or
    @es_extension{NV,framebuffer_blit} in OpenGL ES 2.0
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
typedef Containers::EnumSet<FramebufferBlit,
    GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> FramebufferBlitMask;
#else
typedef Containers::EnumSet<FramebufferBlit> FramebufferBlitMask;
#endif

/**
@brief Framebuffer blit filtering

@see @ref AbstractFramebuffer::blit()
@requires_gl30 Extension @extension{ARB,framebuffer_object}
@requires_gles30 Extension @es_extension{ANGLE,framebuffer_blit} or
    @es_extension{NV,framebuffer_blit} in OpenGL ES 2.0
*/
enum class FramebufferBlitFilter: GLenum {
    Nearest = GL_NEAREST,   /**< Nearest neighbor filtering */
    Linear = GL_LINEAR      /**< Linear interpolation filtering */
};

/**
@brief Target for binding framebuffer

@see @ref DefaultFramebuffer::bind(), @ref Framebuffer::bind()
@requires_gl30 Extension @extension{ARB,framebuffer_object}
*/
enum class FramebufferTarget: GLenum {
    /**
     * For reading only.
     * @requires_gles30 Extension @es_extension{APPLE,framebuffer_multisample},
     *      @es_extension{ANGLE,framebuffer_blit} or @es_extension{NV,framebuffer_blit}
     *      in OpenGL ES 2.0
     */
    #ifndef MAGNUM_TARGET_GLES2
    Read = GL_READ_FRAMEBUFFER,
    #else
    Read = GL_READ_FRAMEBUFFER_APPLE,
    #endif

    /**
     * For drawing only.
     * @requires_gles30 Extension @es_extension{APPLE,framebuffer_multisample},
     *      @es_extension{ANGLE,framebuffer_blit} or @es_extension{NV,framebuffer_blit}
     *      in OpenGL ES 2.0
     */
    #ifndef MAGNUM_TARGET_GLES2
    Draw = GL_DRAW_FRAMEBUFFER,
    #else
    Draw = GL_DRAW_FRAMEBUFFER_APPLE,
    #endif

    ReadDraw = GL_FRAMEBUFFER       /**< For both reading and drawing. */
};

namespace Implementation { struct FramebufferState; }

/**
@brief Base for default and named framebuffers

See @ref DefaultFramebuffer and @ref Framebuffer for more information.

@anchor AbstractFramebuffer-performance-optimization
## Performance optimizations and security

The engine tracks currently bound framebuffer and current viewport to avoid
unnecessary calls to @fn_gl{BindFramebuffer} and @fn_gl{Viewport} when
switching framebuffers. Framebuffer limits and implementation-defined values
(such as @ref maxViewportSize()) are cached, so repeated queries don't result
in repeated @fn_gl{Get} calls.

If extension @extension{ARB,direct_state_access} (part of OpenGL 4.5) is
available, @ref blit() function uses DSA to avoid unnecessary call to
@fn_gl{BindFramebuffer}. See its documentation for more information.

If @extension{ARB,robustness} is available, @ref read() operations are
protected from buffer overflow.
*/
class MAGNUM_EXPORT AbstractFramebuffer {
    friend Implementation::FramebufferState;

    public:
        /** @todo `GL_IMPLEMENTATION_COLOR_READ_FORMAT`, `GL_IMPLEMENTATION_COLOR_READ_TYPE`, seems to be depending on currently bound FB (aargh) (@extension{ARB,ES2_compatibility}). */

        /**
         * @brief Max supported viewport size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref setViewport(), @fn_gl{Get} with @def_gl{MAX_VIEWPORT_DIMS}
         */
        static Vector2i maxViewportSize();

        /**
         * @brief Max supported draw buffer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If ES extension @extension{NV,draw_buffers} is not
         * available, returns `0`.
         * @see @ref DefaultFramebuffer::mapForDraw(), @ref Framebuffer::mapForDraw(),
         *      @fn_gl{Get} with @def_gl{MAX_DRAW_BUFFERS}
         */
        static Int maxDrawBuffers();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Max supported dual-source draw buffer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,blend_func_extended} (part
         * of OpenGL 3.3) is not available, returns `0`.
         * @see @ref DefaultFramebuffer::mapForDraw(), @ref Framebuffer::mapForDraw(),
         *      @fn_gl{Get} with @def_gl{MAX_DUAL_SOURCE_DRAW_BUFFERS}
         * @requires_gl Multiple blending inputs are not available in
         *      OpenGL ES.
         */
        static Int maxDualSourceDrawBuffers();
        #endif

        /**
         * @brief Copy block of pixels
         * @param source            Source framebuffer
         * @param destination       Destination framebuffer
         * @param sourceRectangle   Source rectangle
         * @param destinationRectangle Destination rectangle
         * @param mask              Which buffers to perform blit operation on
         * @param filter            Interpolation filter
         *
         * See @ref DefaultFramebuffer::mapForRead(), @ref Framebuffer::mapForRead(),
         * @ref DefaultFramebuffer::mapForDraw() and @ref Framebuffer::mapForDraw()
         * for specifying particular buffers for blitting operation. If
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, @p source framebuffer is bound to @ref FramebufferTarget::Read
         * and @p destination framebuffer to @ref FramebufferTarget::Draw
         * before the operation (if not already).
         * @see @fn_gl2{BlitNamedFramebuffer,BlitFramebuffer}, eventually
         *      @fn_gl{BlitFramebuffer}
         * @requires_gles30 Extension @es_extension{ANGLE,framebuffer_blit} or
         *      @es_extension{NV,framebuffer_blit} in OpenGL ES 2.0
         * @todo NaCl exports `BlitFramebufferEXT` (although no such extension
         *      exists for ES)
         */
        static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);

        /**
         * @brief Copy block of pixels
         *
         * Convenience alternative to the above function when source rectangle
         * is the same as destination rectangle. As the image is copied
         * pixel-by-pixel, no interpolation is needed and thus
         * @ref FramebufferBlitFilter::Nearest filtering is used by default.
         */
        static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& rectangle, FramebufferBlitMask mask) {
            blit(source, destination, rectangle, rectangle, mask, FramebufferBlitFilter::Nearest);
        }

        /**
         * @brief Bind framebuffer for rendering
         *
         * Binds the framebuffer and updates viewport to saved dimensions.
         * @see @ref setViewport(), @ref DefaultFramebuffer::mapForRead(),
         *      @ref Framebuffer::mapForRead(), @ref DefaultFramebuffer::mapForDraw(),
         *      @ref Framebuffer::mapForDraw(), @fn_gl{BindFramebuffer},
         *      @fn_gl{Viewport}
         * @todo Bind internally to ReadDraw if separate binding points are not
         *      supported
         */
        void bind(FramebufferTarget target);

        /** @brief Viewport rectangle */
        Range2Di viewport() const { return _viewport; }

        /**
         * @brief Set viewport
         * @return Reference to self (for method chaining)
         *
         * Saves the viewport to be used at later time in @ref bind(). If the
         * framebuffer is currently bound, updates the viewport to given
         * rectangle. Initial value in @ref DefaultFramebuffer is set to cover
         * whole window, in @ref Framebuffer the initial value is specified in
         * constructor.
         * @see @ref maxViewportSize(), @fn_gl{Viewport}
         */
        AbstractFramebuffer& setViewport(const Range2Di& rectangle);

        /**
         * @brief Clear specified buffers in framebuffer
         * @param mask              Which buffers to clear
         * @return Reference to self (for method chaining)
         *
         * To improve performance you can also use
         * @ref DefaultFramebuffer::invalidate() / @ref Framebuffer::invalidate()
         * instead of clearing given buffer if you will not use it anymore or
         * fully overwrite it later.
         * @see @ref Renderer::setClearColor(), @ref Renderer::setClearDepth(),
         *      @ref Renderer::setClearStencil(), @fn_gl{BindFramebuffer},
         *      @fn_gl{Clear}
         */
        AbstractFramebuffer& clear(FramebufferClearMask mask);

        /**
         * @brief Read block of pixels from framebuffer to image
         * @param rectangle         Framebuffer rectangle to read
         * @param image             Image where to put the data
         *
         * Image parameters like format and type of pixel data are taken from
         * given image.
         *
         * If @extension{ARB,robustness} is available, the operation is
         * protected from buffer overflow.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{ReadPixels} or
         *      @fn_gl_extension{ReadnPixels,ARB,robustness}
         */
        void read(const Range2Di& rectangle, Image2D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image2D image = framebuffer.read(framebuffer.viewport(), {ColorFormat::RGBA, ColorType::UnsignedByte});
         * @endcode
         */
        Image2D read(const Range2Di& rectangle, Image2D&& image);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief read(const Range2Di&, Image2D&)
         * @deprecated Use @ref read(const Range2Di&, Image2D&) instead.
         */
        CORRADE_DEPRECATED("use read(const Range2Di&, Image2D& instead) instead") void read(const Vector2i& offset, const Vector2i& size, Image2D& image) {
            read({offset, size}, image);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Read block of pixels from framebuffer to buffer image
         * @param rectangle         Framebuffer rectangle to read
         * @param image             Buffer image where to put the data
         * @param usage             Buffer usage
         *
         * See @ref read(const Vector2i&, const Vector2i&, Image2D&) for more
         * information.
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void read(const Range2Di& rectangle, BufferImage2D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage2D image = framebuffer.read(framebuffer.viewport(), {ColorFormat::RGBA, ColorType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage2D read(const Range2Di& rectangle, BufferImage2D&& image, BufferUsage usage);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief read(const Range2Di&, BufferImage2D&, BufferUsage)
         * @deprecated Use @ref read(const Range2Di&, BufferImage2D&, BufferUsage)
         *      instead.
         */
        CORRADE_DEPRECATED("use read(const Range2Di&, BufferImage2D&, BufferUsage) instead") void read(const Vector2i& offset, const Vector2i& size, BufferImage2D& image, BufferUsage usage) {
            read({offset, size}, image, usage);
        }
        #endif
        #endif

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        explicit AbstractFramebuffer() = default;
        ~AbstractFramebuffer() = default;

        void MAGNUM_LOCAL createIfNotAlready();

        void MAGNUM_LOCAL bindInternal(FramebufferTarget target);
        FramebufferTarget MAGNUM_LOCAL bindInternal();
        void MAGNUM_LOCAL setViewportInternal();

        GLuint _id;
        bool _created; /* see createIfNotAlready() for details */
        Range2Di _viewport;

    private:
        #ifndef MAGNUM_TARGET_GLES2
        static void MAGNUM_LOCAL blitImplementationDefault(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL blitImplementationDSA(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);
        #endif
        #else
        static void MAGNUM_LOCAL blitImplementationANGLE(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);
        static void MAGNUM_LOCAL blitImplementationNV(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);
        #endif

        GLenum MAGNUM_LOCAL checkStatusImplementationDefault(FramebufferTarget target);
        #ifndef MAGNUM_TARGET_GLES
        GLenum MAGNUM_LOCAL checkStatusImplementationDSA(FramebufferTarget target);
        GLenum MAGNUM_LOCAL checkStatusImplementationDSAEXT(FramebufferTarget target);
        #endif

        void MAGNUM_LOCAL drawBuffersImplementationDefault(GLsizei count, const GLenum* buffers);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL drawBuffersImplementationDSA(GLsizei count, const GLenum* buffers);
        void MAGNUM_LOCAL drawBuffersImplementationDSAEXT(GLsizei count, const GLenum* buffers);
        #endif

        void MAGNUM_LOCAL drawBufferImplementationDefault(GLenum buffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL drawBufferImplementationDSA(GLenum buffer);
        void MAGNUM_LOCAL drawBufferImplementationDSAEXT(GLenum buffer);
        #endif

        void MAGNUM_LOCAL readBufferImplementationDefault(GLenum buffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL readBufferImplementationDSA(GLenum buffer);
        void MAGNUM_LOCAL readBufferImplementationDSAEXT(GLenum buffer);
        #endif

        static void MAGNUM_LOCAL readImplementationDefault(const Range2Di& rectangle, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);
        static void MAGNUM_LOCAL readImplementationRobustness(const Range2Di& rectangle, ColorFormat format, ColorType type, std::size_t dataSize, GLvoid* data);

        void MAGNUM_LOCAL invalidateImplementationNoOp(GLsizei, const GLenum*);
        void MAGNUM_LOCAL invalidateImplementationDefault(GLsizei count, const GLenum* attachments);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImplementationDSA(GLsizei count, const GLenum* attachments);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL invalidateImplementationNoOp(GLsizei, const GLenum*, const Range2Di&);
        void MAGNUM_LOCAL invalidateImplementationDefault(GLsizei count, const GLenum* attachments, const Range2Di& rectangle);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImplementationDSA(GLsizei count, const GLenum* attachments, const Range2Di& rectangle);
        #endif
        #endif
};

CORRADE_ENUMSET_OPERATORS(FramebufferClearMask)
CORRADE_ENUMSET_OPERATORS(FramebufferBlitMask)

}

#endif
