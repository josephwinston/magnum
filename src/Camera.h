#ifndef Magnum_Camera_h
#define Magnum_Camera_h
/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Camera
 */

#include "Object.h"

namespace Magnum {

/**
 * @brief Camera object
 *
 * @todo Subclasses - perspective, FBO postprocessing etc.
 */
class Camera: public Object {
    public:
        /** @brief Aspect ratio policy */
        enum AspectRatioPolicy {
            NotPreserved,       /**< @brief Don't preserve aspect ratio */
            Extend,             /**< @brief Extend on larger side of view */
            Clip                /**< @brief Clip on smaller side of view */
        };

        /**
         * @brief Constructor
         * @param parent        Parent object
         *
         * Calls <tt>setOrthographic(2, 1, 1000)</tt>.
         */
        Camera(Object* parent = 0);

        /**
         * @brief Scene in which the camera is active
         * @return If the camera is not active anywhere, returns 0.
         */
        inline Scene* active() const { return _active; }

        /**
         * @brief Make camera active in given scene
         *
         * If passed 0 as @c scene and this camera is active in an scene, the
         * camera will be removed from that scene.
         */
        void setActive(Scene* scene);

        /** @brief Aspect ratio policy */
        AspectRatioPolicy aspectRatioPolicy() const { return _aspectRatioPolicy; }

        /** @brief Set aspect ratio policy */
        void setAspectRatioPolicy(AspectRatioPolicy policy) { _aspectRatioPolicy = policy; }

        /**
         * @brief Set orthographic projection
         * @param size      Size of (square) view
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         *
         * The volume of given size will be scaled down to range (-1, 1) on all
         * directions.
         */
        void setOrthographic(GLfloat size, GLfloat near, GLfloat far);

        /**
         * @brief Set perspective projection
         * @param fov       Field of view angle
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         */
        void setPerspective(GLfloat fov, GLfloat near, GLfloat far);

        /**
         * @brief Camera matrix
         *
         * Camera matrix describes world position relative to the camera and is
         * applied as first.
         */
        inline Matrix4 cameraMatrix() {
            setClean();
            return _cameraMatrix;
        }

        /**
         * @brief Projection matrix
         *
         * Projection matrix handles e.g. perspective distortion and is applied
         * as last.
         */
        inline Matrix4 projectionMatrix() const { return _projectionMatrix; }

        /**
         * @brief Set viewport
         * @param width     Window / buffer width
         * @param height    Window / buffer height
         *
         * Called when assigning the camera to the scene or when window
         * size changes.
         */
        void setViewport(int width, int height);

        /**
         * Recalculates camera matrix.
         */
        virtual void setClean();

        /**
         * If the camera was active before and is still active, calls
         * setDirty() on the scene, if is not part of the scene anymore, calls
         * setCamera(0) on the scene.
         */
        virtual void setDirty();

    private:
        Matrix4 rawProjectionMatrix;
        Matrix4 _projectionMatrix;
        Matrix4 _cameraMatrix;
        Scene* _active;

        int viewportWidth, viewportHeight;
        AspectRatioPolicy _aspectRatioPolicy;

        void fixAspectRatio();
};

}

#endif
