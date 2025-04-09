#pragma once

#include "RGBColor.hpp"
#include "Math.hpp"

#include <cstdint>

namespace q3 {

class Shader {
public:
    template<typename T>
    static inline T perspectiveCorrectInterpolate(const T& value0, const T& value1, const T& value2, const Triangle& triangle, const Barycentric& barycentric) {
        float w = triangle.v0_reciprocal_w * barycentric.l0 + triangle.v1_reciprocal_w * barycentric.l1 + triangle.v2_reciprocal_w * barycentric.l2;
        T interpolated = (value0 * triangle.v0_reciprocal_w * barycentric.l0 +
                          value1 * triangle.v1_reciprocal_w * barycentric.l1 +
                          value2 * triangle.v2_reciprocal_w * barycentric.l2) / w;
        return interpolated;
    }

public:
    virtual std::size_t getContextSize() const = 0;
    virtual bool vertexShader(Vertex& v0, Vertex& v1, Vertex& v2, void* data0, void* data1, void* data2, void* context) = 0;
    virtual RGBColor fragmentShader(const Triangle& triangle, const Barycentric& barycentric, void* data0, void* data1, void* data2, const void* context) = 0;
};

}
