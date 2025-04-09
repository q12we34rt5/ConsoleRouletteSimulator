#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <tuple>
#include <memory>

namespace q3 {

template<typename T>
class GraphicsBuffer {
public:
    GraphicsBuffer() : width_(0), height_(0), data_() {}
    GraphicsBuffer(uint32_t width, uint32_t height) 
        : width_(width), height_(height), data_(width * height, T()) {}
    template<typename U>
    GraphicsBuffer(uint32_t width, uint32_t height, U&& value) 
        : width_(width), height_(height), data_(width * height, std::forward<U>(value)) {}
    GraphicsBuffer(const std::vector<T>& data, uint32_t width, uint32_t height)
        : width_(width), height_(height), data_(data) {
        if (data_.size() != width * height) {
            throw std::invalid_argument("Data size does not match the specified width and height.");
        }
    }
    GraphicsBuffer(std::vector<T>&& data, uint32_t width, uint32_t height)
        : width_(width), height_(height), data_(std::move(data)) {
        if (data_.size() != width * height) {
            throw std::invalid_argument("Data size does not match the specified width and height.");
        }
    }

    T* operator[](uint32_t y) { return data_.data() + y * width_; }
    const T* operator[](uint32_t y) const { return data_.data() + y * width_; }

    template<typename U>
    void setValue(uint32_t x, uint32_t y, U&& value) { data_.data()[x + width_ * y] = std::forward<U>(value); }
    T& getValue(uint32_t x, uint32_t y) { return data_.data()[x + width_ * y]; }
    const T& getValue(uint32_t x, uint32_t y) const { return data_.data()[x + width_ * y]; }

    template<typename U>
    void fill(U&& value) { std::fill(data_.begin(), data_.end(), std::forward<U>(value)); }

    uint32_t getWidth() const { return width_; }
    uint32_t getHeight() const { return height_; }
    T* getData() { return data_.data(); }
    const T* getData() const { return data_.data(); }

protected:
    uint32_t width_;
    uint32_t height_;
    std::vector<T> data_;
};

template<typename T>
using DataBuffer = std::vector<T>;

class BaseDataBufferSampler {
public:
    virtual void* getValue(uint32_t index) = 0;
};

class DummyDataBufferSampler : public BaseDataBufferSampler {
public:
    void* getValue(uint32_t index) override { return nullptr; }
};

template<typename T>
class DataBufferSampler : public BaseDataBufferSampler {
public:
    DataBufferSampler(std::shared_ptr<DataBuffer<T>> buffer) : buffer_(buffer) {}

    void* getValue(uint32_t index) override { return &buffer_->operator[](index); }

private:
    std::shared_ptr<DataBuffer<T>> buffer_;
};

/**
 * @brief A flexible sampler for multiple data buffers of different types.
 *
 * AutoDataBufferSampler provides a type-safe and efficient way to handle
 * multiple data buffers simultaneously. It uses advanced template 
 * metaprogramming techniques to combine different buffer types at compile-time,
 * allowing for zero-overhead abstraction.
 *
 * Key features:
 * - Supports any number of q3::DataBuffer<T> instances
 * - Automatic type deduction and safety checks at compile-time
 * - Efficient access to combined buffer data
 * - Compatible with the q3::BaseDataBufferSampler interface
 *
 * Usage example:
 * @code
 * q3::DataBuffer<q3::Vector3> vertices = {{1, 2, 3}};
 * q3::DataBuffer<q3::Vector2> uvs = {{0, 1}};
 * q3::DataBuffer<q3::Vector3> normals = {{0, 1, 0}};
 *
 * AutoDataBufferSampler sampler(vertices, uvs, normals);
 * 
 * // Define a struct that matches the buffer layout
 * struct VertexData {
 *     q3::Vector3& position;
 *     q3::Vector2& uv;
 *     q3::Vector3& normal;
 * };
 * 
 * // Access combined data
 * VertexData* data = static_cast<VertexData*>(sampler.getValue(0));
 * 
 * // Now you can use data->position, data->uv, data->normal
 * @endcode
 *
 * @note When using getValue(), ensure that:
 * 1. You cast the returned pointer to the correct type.
 * 2. The number of fields in your struct matches the number of buffers.
 * 3. The order and types of fields in the struct correspond to the order and types of the buffers.
 */
class AutoDataBufferSampler : public BaseDataBufferSampler {
    // struct to hold a reference to a value of type T
    // the second template parameter is used to avoid ambiguity in inheritance
    template<typename T, std::size_t>
    struct DataUnit {
        using Type = T;

        DataUnit(T& value) : value(value) {}
        T& value;
    };

    // struct to aggregate references of different types
    // implemented through multiple inheritance of DataUnit
    template<typename... Types>
    struct Data : public Types... {
        // pass references of each DataUnit::Type to the corresponding DataUnit
        Data(typename Types::Type&... values) : Types(values)... {}
    };

    // helper class to generate a composite data structure (Data) from a list of types
    template<typename... Types>
    class DataGenerator {
        // calculate the number of types in the template parameter pack Types
        static const std::size_t N = sizeof...(Types);

        // generates a Data struct by aggregating DataUnit instances
        // uses integer sequence to avoid duplicates in inheritance
        template<std::size_t... I>
        static Data<DataUnit<Types, I>...> helper(std::integer_sequence<std::size_t, I...>);

    public:
        // define Type as the result of calling helper with an integer sequence from 0 to N-1
        // this avoids duplicates in inheritance
        using Type = decltype(helper(std::make_integer_sequence<std::size_t, N>{}));
    };

    // abstract base class providing a generic interface for buffer access
    class BaseBufferWrapper {
    public:
        virtual void* getValue(uint32_t index) = 0;
    };

    // template class for storing and accessing multiple buffers
    template<typename... Buffers>
    class BufferWrapper : public BaseBufferWrapper {
        // use DataGenerator to create a composite data type
        using Data = typename DataGenerator<typename std::remove_reference_t<Buffers>::value_type...>::Type;

        // helper function to initialize the data vector
        template<std::size_t... I>
        void initHelper(std::index_sequence<I...>) {
            for (std::size_t i = 0; i < std::get<0>(buffers_)->size(); i++) {
                data.emplace_back(Data(std::get<I>(buffers_)->operator[](i)...));
            }
        }

    public:
        // takes references to multiple buffers
        BufferWrapper(std::shared_ptr<Buffers>... buffers) : buffers_{buffers...}, data{} {
            data.reserve(std::get<0>(buffers_)->size());
            initHelper(std::make_index_sequence<sizeof...(Buffers)>());
        }

        void* getValue(uint32_t index) override {
            return &data[index];
        }

    private:
        std::tuple<std::shared_ptr<Buffers>...> buffers_;
        std::vector<Data> data; // storing the combined data
    };

public:
    // accepts any number of DataBuffers
    template<typename... Buffers>
    AutoDataBufferSampler(std::shared_ptr<Buffers>... buffers) {
        // ensure at least one buffer is provided
        static_assert(sizeof...(Buffers) > 0, "At least one buffer is required");
        // ensure all provided buffers are of type q3::DataBuffer<T>
        static_assert((std::is_same_v<DataBuffer<typename std::remove_reference_t<Buffers>::value_type>, std::remove_reference_t<Buffers>> && ...), "All buffers must be of type q3::DataBuffer<T>");

        // ensure all buffers have the same size
        auto bufferSize = std::get<0>(std::make_tuple(buffers...))->size();
        if (!((buffers->size() == bufferSize) && ...)) {
            throw std::invalid_argument("All buffers must have the same size");
        }

        buffers_ = std::make_unique<BufferWrapper<Buffers...>>(buffers...);
    }

    void* getValue(uint32_t index) override {
        return buffers_->getValue(index);
    }

private:
    std::unique_ptr<BaseBufferWrapper> buffers_; // polymorphic storage for BufferWrapper
};

}
