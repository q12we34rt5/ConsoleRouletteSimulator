#pragma once

#include "Buffer.hpp"
#include "RGBColor.hpp"
#include "Math.hpp"
#include "Shader.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>

#ifdef _WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif

namespace q3 {

class Rasterizer {
public:
    enum class AA_MODE {
        NONE,
        SSAA_2X,
        SSAA_4X,
        SSAA_8X,
        SSAA_16X
    };

public:
    Rasterizer(std::shared_ptr<GraphicsBuffer<RGBColor>> framebuffer, std::shared_ptr<GraphicsBuffer<float>> depthbuffer)
        : target_framebuffer_ptr_(nullptr), target_depthbuffer_ptr_(nullptr),
          aa_mode_(AA_MODE::NONE) {
        setBuffers(framebuffer, depthbuffer);
    }

    inline void setBuffers(std::shared_ptr<GraphicsBuffer<RGBColor>> framebuffer, std::shared_ptr<GraphicsBuffer<float>> depthbuffer) {
        if (framebuffer == nullptr) { throw std::runtime_error("framebuffer is nullptr"); }
        if (depthbuffer == nullptr) { throw std::runtime_error("depthbuffer is nullptr"); }
        if (framebuffer->getWidth() != depthbuffer->getWidth() || framebuffer->getHeight() != depthbuffer->getHeight()) {
            throw std::runtime_error("framebuffer and depthbuffer have different sizes");
        }
        framebuffer_ = framebuffer;
        depthbuffer_ = depthbuffer;
        updateSuperSampleBuffers();
    }

    std::shared_ptr<GraphicsBuffer<RGBColor>> getFramebuffer() const { return framebuffer_; }
    std::shared_ptr<GraphicsBuffer<float>> getDepthbuffer() const { return depthbuffer_; }

    inline void clearFrameBuffer(const RGBColor& color = RGBColor(0, 0, 0, 0)) {
        target_framebuffer_ptr_->fill(color);
    }
    inline void clearDepthBuffer(float value = 1.0f) {
        target_depthbuffer_ptr_->fill(value);
    }

    inline void setAntialiasingMode(AA_MODE mode) {
        aa_mode_ = mode;
        updateSuperSampleBuffers();
    }

    inline void drawBuffer(const DataBuffer<Vector3>& vertices, const DataBuffer<uint32_t>& indices, Shader& shader, BaseDataBufferSampler& sampler) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            uint32_t i0 = indices[i];
            uint32_t i1 = indices[i + 1];
            uint32_t i2 = indices[i + 2];
            const Vector3& v0 = vertices[i0];
            const Vector3& v1 = vertices[i1];
            const Vector3& v2 = vertices[i2];
            void* data0 = sampler.getValue(i0);
            void* data1 = sampler.getValue(i1);
            void* data2 = sampler.getValue(i2);
            drawTriangle(v0, v1, v2, shader, data0, data1, data2);
        }
        downSample();
    }

private:
    inline RGBColor alphaBlend(const RGBColor& src, const RGBColor& dst) {
        float src_alpha = src.a / 255.0f;
        float inv_alpha = 1.0f - src_alpha;

        RGBColor result;
        result.r = static_cast<uint8_t>(src.r * src_alpha + dst.r * inv_alpha);
        result.g = static_cast<uint8_t>(src.g * src_alpha + dst.g * inv_alpha);
        result.b = static_cast<uint8_t>(src.b * src_alpha + dst.b * inv_alpha);
        result.a = static_cast<uint8_t>(std::min(255.0f, src.a + dst.a * inv_alpha));
        return result;
    }

    inline void drawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, Shader& shader, void* data0 = nullptr, void* data1 = nullptr, void* data2 = nullptr) {
        Vertex v0_(v0);
        Vertex v1_(v1);
        Vertex v2_(v2);
        void* context = alloca(shader.getContextSize());

        bool drawable = shader.vertexShader(v0_, v1_, v2_, data0, data1, data2, context);
        if (!drawable) return;

        viewportTransform(v0_);
        viewportTransform(v1_);
        viewportTransform(v2_);

        Triangle triangle{v0_.position, v1_.position, v2_.position, 1.0f / v0_.w, 1.0f / v1_.w, 1.0f / v2_.w};

        int32_t bbox_min_x = std::min({static_cast<int32_t>(v0_.position.x), static_cast<int32_t>(v1_.position.x), static_cast<int32_t>(v2_.position.x)});
        int32_t bbox_min_y = std::min({static_cast<int32_t>(v0_.position.y), static_cast<int32_t>(v1_.position.y), static_cast<int32_t>(v2_.position.y)});
        int32_t bbox_max_x = std::max({static_cast<int32_t>(v0_.position.x), static_cast<int32_t>(v1_.position.x), static_cast<int32_t>(v2_.position.x)});
        int32_t bbox_max_y = std::max({static_cast<int32_t>(v0_.position.y), static_cast<int32_t>(v1_.position.y), static_cast<int32_t>(v2_.position.y)});
        bbox_min_x = std::max(0, bbox_min_x);
        bbox_min_y = std::max(0, bbox_min_y);
        bbox_max_x = std::min(static_cast<int32_t>(target_framebuffer_ptr_->getWidth() - 1), bbox_max_x);
        bbox_max_y = std::min(static_cast<int32_t>(target_framebuffer_ptr_->getHeight() - 1), bbox_max_y);

        for (int32_t y = bbox_min_y; y <= bbox_max_y; y++) {
            for (int32_t x = bbox_min_x; x <= bbox_max_x; x++) {
                Barycentric barycentric = calculateBarycentric(triangle, {static_cast<float>(x), static_cast<float>(y)});
                if (barycentric.l0 < 0 || barycentric.l1 < 0 || barycentric.l2 < 0) continue;

                float z = v0_.position.z * barycentric.l0 + v1_.position.z * barycentric.l1 + v2_.position.z * barycentric.l2;
                if (z < 0.0f || z > 1.0f) continue;
                if (z > target_depthbuffer_ptr_->getValue(x, y)) continue;

                RGBColor srcColor = shader.fragmentShader(triangle, barycentric, data0, data1, data2, context);
                if (srcColor.a == 0) continue;
                RGBColor dstColor = target_framebuffer_ptr_->getValue(x, y);
                RGBColor finalColor = alphaBlend(srcColor, dstColor);
                target_framebuffer_ptr_->setValue(x, y, finalColor);

                if (srcColor.a == 255) {
                    target_depthbuffer_ptr_->setValue(x, y, z);
                }
            }
        }
    }

    inline void viewportTransform(Vertex& v) const {
        float width = static_cast<float>(target_framebuffer_ptr_->getWidth());
        float height = static_cast<float>(target_framebuffer_ptr_->getHeight());

        // perspective division
        v.position.x /= v.w;
        v.position.y /= v.w;
        v.position.z /= v.w;

        // NDC to screen space
        v.position.x = (v.position.x + 1.0f) * width * 0.5f;
        v.position.y = (1.0f - v.position.y) * height * 0.5f;
        v.position.z = (v.position.z + 1.0f) * 0.5f;
    }

    inline void updateSuperSampleBuffers() {
        auto update_buffer = [this](uint32_t ssaa) {
            // check buffer already exists
            bool need_update = super_sample_framebuffer_ == nullptr;
            // check buffer size is correct
            if (!need_update) { need_update = super_sample_framebuffer_->getWidth() != framebuffer_->getWidth() * ssaa || super_sample_framebuffer_->getHeight() != framebuffer_->getHeight() * ssaa; }
            // update buffer
            if (need_update) {
                super_sample_framebuffer_ = std::make_shared<GraphicsBuffer<RGBColor>>(framebuffer_->getWidth() * ssaa, framebuffer_->getHeight() * ssaa);
                super_sample_depthbuffer_ = std::make_shared<GraphicsBuffer<float>>(framebuffer_->getWidth() * ssaa, framebuffer_->getHeight() * ssaa);
            }
            target_framebuffer_ptr_ = super_sample_framebuffer_.get();
            target_depthbuffer_ptr_ = super_sample_depthbuffer_.get();
        };
        switch (aa_mode_) {
        case AA_MODE::SSAA_2X:
            update_buffer(2);
            break;
        case AA_MODE::SSAA_4X:
            update_buffer(4);
            break;
        case AA_MODE::SSAA_8X:
            update_buffer(8);
            break;
        case AA_MODE::SSAA_16X:
            update_buffer(16);
            break;
        case AA_MODE::NONE:
        default:
            target_framebuffer_ptr_ = framebuffer_.get();
            target_depthbuffer_ptr_ = depthbuffer_.get();
            // release super sample buffers
            super_sample_framebuffer_ = nullptr;
            super_sample_depthbuffer_ = nullptr;
            break;
        }
    }

    inline void downSample() {
        if (aa_mode_ == AA_MODE::NONE) return;
        auto down_sample = [this](uint32_t ssaa) {
            uint32_t ssaa2f = ssaa * ssaa;
            for (uint32_t y = 0; y < framebuffer_->getHeight(); y++) {
                for (uint32_t x = 0; x < framebuffer_->getWidth(); x++) {
                    Vector3i color;
                    int alpha = 0;
                    float min_depth = std::numeric_limits<float>::max();
                    for (uint32_t j = 0; j < ssaa; j++) {
                        for (uint32_t i = 0; i < ssaa; i++) {
                            const RGBColor& c = super_sample_framebuffer_->getValue(x * ssaa + i, y * ssaa + j);
                            color.x += c.r; color.y += c.g; color.z += c.b;
                            alpha += c.a;
                            float depth = super_sample_depthbuffer_->getValue(x * ssaa + i, y * ssaa + j);
                            min_depth = std::min(min_depth, depth);
                        }
                    }
                    color /= ssaa2f;
                    alpha /= ssaa2f;
                    framebuffer_->setValue(x, y, RGBColor{static_cast<uint8_t>(color.x), static_cast<uint8_t>(color.y), static_cast<uint8_t>(color.z), static_cast<uint8_t>(alpha)});
                    depthbuffer_->setValue(x, y, min_depth);
                }
            }
        };
        switch (aa_mode_) {
        case AA_MODE::SSAA_2X:
            down_sample(2);
            break;
        case AA_MODE::SSAA_4X:
            down_sample(4);
            break;
        case AA_MODE::SSAA_8X:
            down_sample(8);
            break;
        case AA_MODE::SSAA_16X:
            down_sample(16);
            break;
        case AA_MODE::NONE:
        default:
            break;
        }
    }

private:
    // frame buffers
    std::shared_ptr<GraphicsBuffer<RGBColor>> framebuffer_;
    std::shared_ptr<GraphicsBuffer<float>> depthbuffer_;
    // for super sampling
    std::shared_ptr<GraphicsBuffer<RGBColor>> super_sample_framebuffer_;
    std::shared_ptr<GraphicsBuffer<float>> super_sample_depthbuffer_;
    // target buffers
    GraphicsBuffer<RGBColor>* target_framebuffer_ptr_;
    GraphicsBuffer<float>* target_depthbuffer_ptr_;
    // draw options
    AA_MODE aa_mode_;
};

}
