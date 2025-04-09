#pragma once

#include "RGBColor.hpp"
#include "Buffer.hpp"
#include "Math.hpp"

#include <cstdint>
#include <cmath>
#include <memory>

namespace q3 {

class Texture {
public:
    Texture() : imagebuffer_(nullptr), imagebuffer_ptr_(nullptr) {}
    Texture(std::shared_ptr<GraphicsBuffer<RGBColor>> imagebuffer) : imagebuffer_(imagebuffer), imagebuffer_ptr_(imagebuffer.get()) {}

    void setImageBuffer(std::shared_ptr<GraphicsBuffer<RGBColor>> imagebuffer) {
        imagebuffer_ = imagebuffer;
        imagebuffer_ptr_ = imagebuffer.get();
    }

    std::shared_ptr<GraphicsBuffer<RGBColor>> getImageBuffer() const { return imagebuffer_; }

    inline RGBColor sample(const Vector2& uv) const {
        return sample(uv.x, uv.y);
    }
    inline RGBColor sample(float u, float v) const {
        // u, v to [0, 1]
        u = std::fmod(u, 1.0f);
        v = std::fmod(v, 1.0f);
        if (u < 0) { u += 1.0f; }
        if (v < 0) { v += 1.0f; }
        // [0, 1] to [0, width-1] and [0, height-1]
        uint32_t width = imagebuffer_ptr_->getWidth();
        uint32_t height = imagebuffer_ptr_->getHeight();
        uint32_t x = static_cast<uint32_t>(u * width);
        uint32_t y = static_cast<uint32_t>(v * height);
        y = height - y - 1; // flip y
        return imagebuffer_ptr_->getValue(x, y);
    }

private:
    std::shared_ptr<GraphicsBuffer<RGBColor>> imagebuffer_;
    // for fast access
    GraphicsBuffer<RGBColor>* imagebuffer_ptr_;
};

}
