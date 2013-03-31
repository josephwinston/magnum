/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Circle.h"

#include "Math/Functions.h"
#include "Math/Vector2.h"
#include "Trade/MeshData2D.h"

namespace Magnum { namespace Primitives {

Trade::MeshData2D Circle::solid(UnsignedInt segments) {
    CORRADE_ASSERT(segments >= 3, "Primitives::Circle::solid(): segments must be >= 3",
        Trade::MeshData2D(Mesh::Primitive::TriangleFan, nullptr, {}, {}));

    auto positions = new std::vector<Vector2>;
    positions->reserve(segments+1);

    /* Central point */
    positions->emplace_back();

    /* Points on circle */
    const Rad angleIncrement(2*Constants::pi()/segments);
    for(UnsignedInt i = 0; i != segments; ++i)
        positions->emplace_back(Math::cos(i*angleIncrement), Math::sin(i*angleIncrement));

    return Trade::MeshData2D(Mesh::Primitive::TriangleFan, nullptr, {positions}, {});
}

Trade::MeshData2D Circle::wireframe(UnsignedInt segments) {
    CORRADE_ASSERT(segments >= 3, "Primitives::Circle::wireframe(): segments must be >= 3",
        Trade::MeshData2D(Mesh::Primitive::LineLoop, nullptr, {}, {}));

    auto positions = new std::vector<Vector2>;
    positions->reserve(segments);

    /* Points on circle */
    const Rad angleIncrement(2*Constants::pi()/segments);
    for(UnsignedInt i = 0; i != segments; ++i)
        positions->emplace_back(Math::cos(i*angleIncrement), Math::sin(i*angleIncrement));

    return Trade::MeshData2D(Mesh::Primitive::LineLoop, nullptr, {positions}, {});
}

}}