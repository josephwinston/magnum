#ifndef Magnum_SceneGraph_AbstractCamera_hpp
#define Magnum_SceneGraph_AbstractCamera_hpp
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
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref AbstractCamera.h
 */

#include "Magnum/Math/Functions.h"
#include "Magnum/SceneGraph/AbstractCamera.h"
#include "Magnum/SceneGraph/Drawable.h"

namespace Magnum { namespace SceneGraph {

namespace Implementation {

template<UnsignedInt dimensions, class T> MatrixTypeFor<dimensions, T> aspectRatioFix(AspectRatioPolicy aspectRatioPolicy, const Math::Vector2<T>& projectionScale, const Vector2i& viewport) {
    /* Don't divide by zero / don't preserve anything */
    if(projectionScale.x() == 0 || projectionScale.y() == 0 || viewport.x() == 0 || viewport.y() == 0 || aspectRatioPolicy == AspectRatioPolicy::NotPreserved)
        return {};

    CORRADE_INTERNAL_ASSERT((projectionScale > Math::Vector2<T>(0)).all() && (viewport > Vector2i(0)).all());
    Math::Vector2<T> relativeAspectRatio = Math::Vector2<T>(viewport)*projectionScale;

    /* Extend on larger side = scale larger side down
       Clip on smaller side = scale smaller side up */
    return MatrixTypeFor<dimensions, T>::scaling(Math::Vector<dimensions, T>::pad(
        (relativeAspectRatio.x() > relativeAspectRatio.y()) == (aspectRatioPolicy == AspectRatioPolicy::Extend) ?
        Vector2(relativeAspectRatio.y()/relativeAspectRatio.x(), T(1)) :
        Vector2(T(1), relativeAspectRatio.x()/relativeAspectRatio.y()), T(1)));
}

}

template<UnsignedInt dimensions, class T> AbstractCamera<dimensions, T>::AbstractCamera(AbstractObject<dimensions, T>& object): AbstractFeature<dimensions, T>(object), _aspectRatioPolicy(AspectRatioPolicy::NotPreserved) {
    AbstractFeature<dimensions, T>::setCachedTransformations(CachedTransformation::InvertedAbsolute);
}

template<UnsignedInt dimensions, class T> AbstractCamera<dimensions, T>::~AbstractCamera() = default;

template<UnsignedInt dimensions, class T> void AbstractCamera<dimensions, T>::fixAspectRatio() {
    _projectionMatrix = Implementation::aspectRatioFix<dimensions, T>(_aspectRatioPolicy, {Math::abs(rawProjectionMatrix[0].x()), Math::abs(rawProjectionMatrix[1].y())}, _viewport)*rawProjectionMatrix;
}

template<UnsignedInt dimensions, class T> AbstractCamera<dimensions, T>& AbstractCamera<dimensions, T>::setAspectRatioPolicy(AspectRatioPolicy policy) {
    _aspectRatioPolicy = policy;
    fixAspectRatio();
    return *this;
}

template<UnsignedInt dimensions, class T> void AbstractCamera<dimensions, T>::setViewport(const Vector2i& size) {
    _viewport = size;
    fixAspectRatio();
}

template<UnsignedInt dimensions, class T> void AbstractCamera<dimensions, T>::draw(DrawableGroup<dimensions, T>& group) {
    AbstractObject<dimensions, T>* scene = AbstractFeature<dimensions, T>::object().scene();
    CORRADE_ASSERT(scene, "Camera::draw(): cannot draw when camera is not part of any scene", );

    /* Compute camera matrix */
    AbstractFeature<dimensions, T>::object().setClean();

    /* Compute transformations of all objects in the group relative to the camera */
    std::vector<std::reference_wrapper<AbstractObject<dimensions, T>>> objects;
    objects.reserve(group.size());
    for(std::size_t i = 0; i != group.size(); ++i)
        objects.push_back(group[i].object());
    std::vector<MatrixTypeFor<dimensions, T>> transformations =
        scene->transformationMatrices(objects, _cameraMatrix);

    /* Perform the drawing */
    for(std::size_t i = 0; i != transformations.size(); ++i)
        group[i].draw(transformations[i], *this);
}

}}

#endif
