#pragma once

#include <cstdint>
#include <cmath>

namespace q3 {

template<typename T> class Vector3T;
template<typename T> class Vector4T;
template<typename T> class Matrix4T;

template<typename T>
class Vector2T {
public:
    constexpr Vector2T() noexcept : x(), y() {}
    constexpr Vector2T(T x, T y) noexcept : x(x), y(y) {}
    // from Vector3
    constexpr explicit Vector2T(const Vector3T<T>& other) noexcept;

    // +, -, *, /
    constexpr Vector2T<T> operator+(const Vector2T<T>& other) const noexcept { return {x + other.x, y + other.y}; }
    constexpr Vector2T<T> operator-(const Vector2T<T>& other) const noexcept { return {x - other.x, y - other.y}; }
    constexpr Vector2T<T> operator*(const Vector2T<T>& other) const noexcept { return {x * other.x, y * other.y}; }
    constexpr Vector2T<T> operator/(const Vector2T<T>& other) const noexcept { return {x / other.x, y / other.y}; }
    constexpr Vector2T<T> operator+(T scalar) const noexcept { return {x + scalar, y + scalar}; }
    constexpr Vector2T<T> operator-(T scalar) const noexcept { return {x - scalar, y - scalar}; }
    constexpr Vector2T<T> operator*(T scalar) const noexcept { return {x * scalar, y * scalar}; }
    constexpr Vector2T<T> operator/(T scalar) const noexcept { return {x / scalar, y / scalar}; }
    template<typename U> friend constexpr Vector2T<U> operator+(U scalar, const Vector2T<U>& vec) noexcept;
    template<typename U> friend constexpr Vector2T<U> operator-(U scalar, const Vector2T<U>& vec) noexcept;
    template<typename U> friend constexpr Vector2T<U> operator*(U scalar, const Vector2T<U>& vec) noexcept;
    template<typename U> friend constexpr Vector2T<U> operator/(U scalar, const Vector2T<U>& vec) noexcept;
    // +=, -=, *=, /=
    constexpr Vector2T<T>& operator+=(const Vector2T<T>& other) noexcept { x += other.x; y += other.y; return *this; }
    constexpr Vector2T<T>& operator-=(const Vector2T<T>& other) noexcept { x -= other.x; y -= other.y; return *this; }
    constexpr Vector2T<T>& operator*=(const Vector2T<T>& other) noexcept { x *= other.x; y *= other.y; return *this; }
    constexpr Vector2T<T>& operator/=(const Vector2T<T>& other) noexcept { x /= other.x; y /= other.y; return *this; }
    constexpr Vector2T<T>& operator+=(T scalar) noexcept { x += scalar; y += scalar; return *this; }
    constexpr Vector2T<T>& operator-=(T scalar) noexcept { x -= scalar; y -= scalar; return *this; }
    constexpr Vector2T<T>& operator*=(T scalar) noexcept { x *= scalar; y *= scalar; return *this; }
    constexpr Vector2T<T>& operator/=(T scalar) noexcept { x /= scalar; y /= scalar; return *this; }
    // negation
    constexpr Vector2T<T> operator-() const noexcept { return {-x, -y}; }
    // comparison
    constexpr bool operator==(const Vector2T<T>& other) const noexcept { return x == other.x && y == other.y; }
    // dot product
    constexpr T dot(const Vector2T<T>& other) const noexcept { return x * other.x + y * other.y; }
    // norm
    constexpr T norm() const noexcept { return std::sqrt(x * x + y * y); }
    // in-place normalize
    constexpr Vector2T<T>& normalize() noexcept { return *this /= norm(); }
    // normalize
    constexpr Vector2T<T> normalized() const noexcept { return *this / norm(); }

    T x, y;
};

template<typename T> constexpr Vector2T<T>::Vector2T(const Vector3T<T>& other) noexcept : x(other.x), y(other.y) {}

template<typename T> constexpr Vector2T<T> operator+(T scalar, const Vector2T<T>& vec) noexcept { return vec + scalar; }
template<typename T> constexpr Vector2T<T> operator-(T scalar, const Vector2T<T>& vec) noexcept { return {scalar - vec.x, scalar - vec.y}; }
template<typename T> constexpr Vector2T<T> operator*(T scalar, const Vector2T<T>& vec) noexcept { return vec * scalar; }
template<typename T> constexpr Vector2T<T> operator/(T scalar, const Vector2T<T>& vec) noexcept { return {scalar / vec.x, scalar / vec.y}; }

template<typename T>
class Vector3T {
public:
    constexpr Vector3T() noexcept : x(), y(), z() {}
    constexpr Vector3T(T x, T y, T z) noexcept : x(x), y(y), z(z) {}
    constexpr Vector3T(const Vector2T<T>& other, T z) noexcept : x(other.x), y(other.y), z(z) {}
    constexpr Vector3T(const Vector2T<T>& other) noexcept : x(other.x), y(other.y), z() {}
    // from Vector4
    constexpr explicit Vector3T(const Vector4T<T>& other) noexcept;

    // +, -, *, /
    constexpr Vector3T<T> operator+(const Vector3T<T>& other) const noexcept { return {x + other.x, y + other.y, z + other.z}; }
    constexpr Vector3T<T> operator-(const Vector3T<T>& other) const noexcept { return {x - other.x, y - other.y, z - other.z}; }
    constexpr Vector3T<T> operator*(const Vector3T<T>& other) const noexcept { return {x * other.x, y * other.y, z * other.z}; }
    constexpr Vector3T<T> operator/(const Vector3T<T>& other) const noexcept { return {x / other.x, y / other.y, z / other.z}; }
    constexpr Vector3T<T> operator+(T scalar) const noexcept { return {x + scalar, y + scalar, z + scalar}; }
    constexpr Vector3T<T> operator-(T scalar) const noexcept { return {x - scalar, y - scalar, z - scalar}; }
    constexpr Vector3T<T> operator*(T scalar) const noexcept { return {x * scalar, y * scalar, z * scalar}; }
    constexpr Vector3T<T> operator/(T scalar) const noexcept { return {x / scalar, y / scalar, z / scalar}; }
    template<typename U> friend constexpr Vector3T<U> operator+(U scalar, const Vector3T<U>& vec) noexcept;
    template<typename U> friend constexpr Vector3T<U> operator-(U scalar, const Vector3T<U>& vec) noexcept;
    template<typename U> friend constexpr Vector3T<U> operator*(U scalar, const Vector3T<U>& vec) noexcept;
    template<typename U> friend constexpr Vector3T<U> operator/(U scalar, const Vector3T<U>& vec) noexcept;
    // +=, -=, *=, /=
    constexpr Vector3T<T>& operator+=(const Vector3T<T>& other) noexcept { x += other.x; y += other.y; z += other.z; return *this; }
    constexpr Vector3T<T>& operator-=(const Vector3T<T>& other) noexcept { x -= other.x; y -= other.y; z -= other.z; return *this; }
    constexpr Vector3T<T>& operator*=(const Vector3T<T>& other) noexcept { x *= other.x; y *= other.y; z *= other.z; return *this; }
    constexpr Vector3T<T>& operator/=(const Vector3T<T>& other) noexcept { x /= other.x; y /= other.y; z /= other.z; return *this; }
    constexpr Vector3T<T>& operator+=(T scalar) noexcept { x += scalar; y += scalar; z += scalar; return *this; }
    constexpr Vector3T<T>& operator-=(T scalar) noexcept { x -= scalar; y -= scalar; z -= scalar; return *this; }
    constexpr Vector3T<T>& operator*=(T scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; return *this; }
    constexpr Vector3T<T>& operator/=(T scalar) noexcept { x /= scalar; y /= scalar; z /= scalar; return *this; }
    // negation
    constexpr Vector3T<T> operator-() const noexcept { return {-x, -y, -z}; }
    // comparison
    constexpr bool operator==(const Vector3T<T>& other) const noexcept { return x == other.x && y == other.y && z == other.z; }
    // dot product
    constexpr T dot(const Vector3T<T>& other) const noexcept { return x * other.x + y * other.y + z * other.z; }
    // cross product
    constexpr Vector3T<T> cross(const Vector3T<T>& other) const noexcept { return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x}; }
    // norm
    constexpr T norm() const noexcept { return std::sqrt(x * x + y * y + z * z); }
    // in-place normalize
    constexpr Vector3T<T>& normalize() noexcept { return *this /= norm(); }
    // normalize
    constexpr Vector3T<T> normalized() const noexcept { return *this / norm(); }

    T x, y, z;
};

template<typename T> constexpr Vector3T<T>::Vector3T(const Vector4T<T>& other) noexcept : x(other.position.x), y(other.position.y), z(other.position.z) {}

template<typename T> constexpr Vector3T<T> operator+(T scalar, const Vector3T<T>& vec) noexcept { return vec + scalar; }
template<typename T> constexpr Vector3T<T> operator-(T scalar, const Vector3T<T>& vec) noexcept { return {scalar - vec.x, scalar - vec.y, scalar - vec.z}; }
template<typename T> constexpr Vector3T<T> operator*(T scalar, const Vector3T<T>& vec) noexcept { return vec * scalar; }
template<typename T> constexpr Vector3T<T> operator/(T scalar, const Vector3T<T>& vec) noexcept { return {scalar / vec.x, scalar / vec.y, scalar / vec.z}; }

template<typename T>
class Vector4T {
public:
    constexpr Vector4T() noexcept : position(), w() {}
    constexpr Vector4T(T x, T y, T z, T w) noexcept : position(x, y, z), w(w) {}
    constexpr Vector4T(const Vector3T<T>& other, T w) noexcept : position(other), w(w) {}
    constexpr Vector4T(const Vector3T<T>& other) noexcept : position(other), w() {}

    // +, -, *, /
    constexpr Vector4T<T> operator+(const Vector4T<T>& other) const noexcept { return {position + other.position, w + other.w}; }
    constexpr Vector4T<T> operator-(const Vector4T<T>& other) const noexcept { return {position - other.position, w - other.w}; }
    constexpr Vector4T<T> operator*(const Vector4T<T>& other) const noexcept { return {position * other.position, w * other.w}; }
    constexpr Vector4T<T> operator/(const Vector4T<T>& other) const noexcept { return {position / other.position, w / other.w}; }
    constexpr Vector4T<T> operator+(T scalar) const noexcept { return {position + scalar, w + scalar}; }
    constexpr Vector4T<T> operator-(T scalar) const noexcept { return {position - scalar, w - scalar}; }
    constexpr Vector4T<T> operator*(T scalar) const noexcept { return {position * scalar, w * scalar}; }
    constexpr Vector4T<T> operator/(T scalar) const noexcept { return {position / scalar, w / scalar}; }
    template<typename U> friend constexpr Vector4T<U> operator+(U scalar, const Vector4T<U>& vec) noexcept;
    template<typename U> friend constexpr Vector4T<U> operator-(U scalar, const Vector4T<U>& vec) noexcept;
    template<typename U> friend constexpr Vector4T<U> operator*(U scalar, const Vector4T<U>& vec) noexcept;
    template<typename U> friend constexpr Vector4T<U> operator/(U scalar, const Vector4T<U>& vec) noexcept;
    // +=, -=, *=, /=
    constexpr Vector4T<T>& operator+=(const Vector4T<T>& other) noexcept { position += other.position; w += other.w; return *this; }
    constexpr Vector4T<T>& operator-=(const Vector4T<T>& other) noexcept { position -= other.position; w -= other.w; return *this; }
    constexpr Vector4T<T>& operator*=(const Vector4T<T>& other) noexcept { position *= other.position; w *= other.w; return *this; }
    constexpr Vector4T<T>& operator/=(const Vector4T<T>& other) noexcept { position /= other.position; w /= other.w; return *this; }
    constexpr Vector4T<T>& operator+=(T scalar) noexcept { position += scalar; w += scalar; return *this; }
    constexpr Vector4T<T>& operator-=(T scalar) noexcept { position -= scalar; w -= scalar; return *this; }
    constexpr Vector4T<T>& operator*=(T scalar) noexcept { position *= scalar; w *= scalar; return *this; }
    constexpr Vector4T<T>& operator/=(T scalar) noexcept { position /= scalar; w /= scalar; return *this; }
    // negation
    constexpr Vector4T<T> operator-() const noexcept { return {-position, -w}; }
    // comparison
    constexpr bool operator==(const Vector4T<T>& other) const noexcept { return position == other.position && w == other.w; }
    // dot product
    constexpr T dot(const Vector4T<T>& other) const noexcept { return position.dot(other.position) + w * other.w; }
    constexpr Vector4T<T> dot(const Matrix4T<T>& other) const noexcept;
    // norm
    constexpr T norm() const noexcept { return std::sqrt(position.x * position.x + position.y * position.y + position.z * position.z + w * w); }
    // in-place normalize
    constexpr Vector4T<T>& normalize() noexcept { return *this /= norm(); }
    // normalize
    constexpr Vector4T<T> normalized() const noexcept { return *this / norm(); }

    Vector3T<T> position;
    T w;
};

template<typename T> constexpr Vector4T<T> operator+(T scalar, const Vector4T<T>& vec) noexcept { return vec + scalar; }
template<typename T> constexpr Vector4T<T> operator-(T scalar, const Vector4T<T>& vec) noexcept { return {scalar - vec.position, scalar - vec.w}; }
template<typename T> constexpr Vector4T<T> operator*(T scalar, const Vector4T<T>& vec) noexcept { return vec * scalar; }
template<typename T> constexpr Vector4T<T> operator/(T scalar, const Vector4T<T>& vec) noexcept { return {scalar / vec.position, scalar / vec.w}; }

template<typename T> constexpr Vector4T<T> Vector4T<T>::dot(const Matrix4T<T>& other) const noexcept {
    return {
        position.x * other[0][0] + position.y * other[1][0] + position.z * other[2][0] + w * other[3][0],
        position.x * other[0][1] + position.y * other[1][1] + position.z * other[2][1] + w * other[3][1],
        position.x * other[0][2] + position.y * other[1][2] + position.z * other[2][2] + w * other[3][2],
        position.x * other[0][3] + position.y * other[1][3] + position.z * other[2][3] + w * other[3][3]
    };
}

template<typename T>
class Matrix4T {
    enum class Major { ROW, COL };
    template<Major> struct Indexer {};
public:
    static constexpr Indexer<Major::ROW> ROW{};
    static constexpr Indexer<Major::COL> COL{};

    constexpr Matrix4T() noexcept : d_{} {}
    constexpr Matrix4T(const Vector4T<T>& v0, const Vector4T<T>& v1, const Vector4T<T>& v2, const Vector4T<T>& v3, Indexer<Major::ROW> = {}) noexcept
        : d_{
            {v0.position.x, v0.position.y, v0.position.z, v0.w},
            {v1.position.x, v1.position.y, v1.position.z, v1.w},
            {v2.position.x, v2.position.y, v2.position.z, v2.w},
            {v3.position.x, v3.position.y, v3.position.z, v3.w}
        } {}
    constexpr Matrix4T(const Vector4T<T>& v0, const Vector4T<T>& v1, const Vector4T<T>& v2, const Vector4T<T>& v3, Indexer<Major::COL>) noexcept
        : d_{
            {v0.position.x, v1.position.x, v2.position.x, v3.position.x},
            {v0.position.y, v1.position.y, v2.position.y, v3.position.y},
            {v0.position.z, v1.position.z, v2.position.z, v3.position.z},
            {v0.w, v1.w, v2.w, v3.w}
        } {}
    constexpr Matrix4T(const T(&list)[4][4], Indexer<Major::ROW> = {}) noexcept {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                d_[i][j] = list[i][j];
    }
    constexpr Matrix4T(const T(&list)[4][4], Indexer<Major::COL>) noexcept {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                d_[i][j] = list[j][i];
    }

    inline constexpr T* operator[](int index) noexcept { return d_[index]; }
    inline constexpr const T* operator[](int index) const noexcept { return d_[index]; }

    // +, -, *, /
    constexpr Matrix4T<T> operator+(const Matrix4T<T>& other) const noexcept {
        return {
            {d_[0][0] + other.d_[0][0], d_[0][1] + other.d_[0][1], d_[0][2] + other.d_[0][2], d_[0][3] + other.d_[0][3]},
            {d_[1][0] + other.d_[1][0], d_[1][1] + other.d_[1][1], d_[1][2] + other.d_[1][2], d_[1][3] + other.d_[1][3]},
            {d_[2][0] + other.d_[2][0], d_[2][1] + other.d_[2][1], d_[2][2] + other.d_[2][2], d_[2][3] + other.d_[2][3]},
            {d_[3][0] + other.d_[3][0], d_[3][1] + other.d_[3][1], d_[3][2] + other.d_[3][2], d_[3][3] + other.d_[3][3]}
        };
    }
    constexpr Matrix4T<T> operator-(const Matrix4T<T>& other) const noexcept {
        return {
            {d_[0][0] - other.d_[0][0], d_[0][1] - other.d_[0][1], d_[0][2] - other.d_[0][2], d_[0][3] - other.d_[0][3]},
            {d_[1][0] - other.d_[1][0], d_[1][1] - other.d_[1][1], d_[1][2] - other.d_[1][2], d_[1][3] - other.d_[1][3]},
            {d_[2][0] - other.d_[2][0], d_[2][1] - other.d_[2][1], d_[2][2] - other.d_[2][2], d_[2][3] - other.d_[2][3]},
            {d_[3][0] - other.d_[3][0], d_[3][1] - other.d_[3][1], d_[3][2] - other.d_[3][2], d_[3][3] - other.d_[3][3]}
        };
    }
    constexpr Matrix4T<T> operator*(const Matrix4T<T>& other) const noexcept {
        return {
            {d_[0][0] * other.d_[0][0], d_[0][1] * other.d_[0][1], d_[0][2] * other.d_[0][2], d_[0][3] * other.d_[0][3]},
            {d_[1][0] * other.d_[1][0], d_[1][1] * other.d_[1][1], d_[1][2] * other.d_[1][2], d_[1][3] * other.d_[1][3]},
            {d_[2][0] * other.d_[2][0], d_[2][1] * other.d_[2][1], d_[2][2] * other.d_[2][2], d_[2][3] * other.d_[2][3]},
            {d_[3][0] * other.d_[3][0], d_[3][1] * other.d_[3][1], d_[3][2] * other.d_[3][2], d_[3][3] * other.d_[3][3]}
        };
    }
    constexpr Matrix4T<T> operator/(const Matrix4T<T>& other) const noexcept {
        return {
            {d_[0][0] / other.d_[0][0], d_[0][1] / other.d_[0][1], d_[0][2] / other.d_[0][2], d_[0][3] / other.d_[0][3]},
            {d_[1][0] / other.d_[1][0], d_[1][1] / other.d_[1][1], d_[1][2] / other.d_[1][2], d_[1][3] / other.d_[1][3]},
            {d_[2][0] / other.d_[2][0], d_[2][1] / other.d_[2][1], d_[2][2] / other.d_[2][2], d_[2][3] / other.d_[2][3]},
            {d_[3][0] / other.d_[3][0], d_[3][1] / other.d_[3][1], d_[3][2] / other.d_[3][2], d_[3][3] / other.d_[3][3]}
        };
    }
    constexpr Matrix4T<T> operator+(T scalar) const noexcept {
        return {
            {d_[0][0] + scalar, d_[0][1] + scalar, d_[0][2] + scalar, d_[0][3] + scalar},
            {d_[1][0] + scalar, d_[1][1] + scalar, d_[1][2] + scalar, d_[1][3] + scalar},
            {d_[2][0] + scalar, d_[2][1] + scalar, d_[2][2] + scalar, d_[2][3] + scalar},
            {d_[3][0] + scalar, d_[3][1] + scalar, d_[3][2] + scalar, d_[3][3] + scalar}
        };
    }
    constexpr Matrix4T<T> operator-(T scalar) const noexcept {
        return {
            {d_[0][0] - scalar, d_[0][1] - scalar, d_[0][2] - scalar, d_[0][3] - scalar},
            {d_[1][0] - scalar, d_[1][1] - scalar, d_[1][2] - scalar, d_[1][3] - scalar},
            {d_[2][0] - scalar, d_[2][1] - scalar, d_[2][2] - scalar, d_[2][3] - scalar},
            {d_[3][0] - scalar, d_[3][1] - scalar, d_[3][2] - scalar, d_[3][3] - scalar}
        };
    }
    constexpr Matrix4T<T> operator*(T scalar) const noexcept {
        return {
            {d_[0][0] * scalar, d_[0][1] * scalar, d_[0][2] * scalar, d_[0][3] * scalar},
            {d_[1][0] * scalar, d_[1][1] * scalar, d_[1][2] * scalar, d_[1][3] * scalar},
            {d_[2][0] * scalar, d_[2][1] * scalar, d_[2][2] * scalar, d_[2][3] * scalar},
            {d_[3][0] * scalar, d_[3][1] * scalar, d_[3][2] * scalar, d_[3][3] * scalar}
        };
    }
    constexpr Matrix4T<T> operator/(T scalar) const noexcept {
        return {
            {d_[0][0] / scalar, d_[0][1] / scalar, d_[0][2] / scalar, d_[0][3] / scalar},
            {d_[1][0] / scalar, d_[1][1] / scalar, d_[1][2] / scalar, d_[1][3] / scalar},
            {d_[2][0] / scalar, d_[2][1] / scalar, d_[2][2] / scalar, d_[2][3] / scalar},
            {d_[3][0] / scalar, d_[3][1] / scalar, d_[3][2] / scalar, d_[3][3] / scalar}
        };
    }
    // +=, -=, *=, /=
    constexpr Matrix4T<T>& operator+=(const Matrix4T<T>& other) noexcept {
        d_[0][0] += other.d_[0][0]; d_[0][1] += other.d_[0][1]; d_[0][2] += other.d_[0][2]; d_[0][3] += other.d_[0][3];
        d_[1][0] += other.d_[1][0]; d_[1][1] += other.d_[1][1]; d_[1][2] += other.d_[1][2]; d_[1][3] += other.d_[1][3];
        d_[2][0] += other.d_[2][0]; d_[2][1] += other.d_[2][1]; d_[2][2] += other.d_[2][2]; d_[2][3] += other.d_[2][3];
        d_[3][0] += other.d_[3][0]; d_[3][1] += other.d_[3][1]; d_[3][2] += other.d_[3][2]; d_[3][3] += other.d_[3][3];
        return *this;
    }
    constexpr Matrix4T<T>& operator-=(const Matrix4T<T>& other) noexcept {
        d_[0][0] -= other.d_[0][0]; d_[0][1] -= other.d_[0][1]; d_[0][2] -= other.d_[0][2]; d_[0][3] -= other.d_[0][3];
        d_[1][0] -= other.d_[1][0]; d_[1][1] -= other.d_[1][1]; d_[1][2] -= other.d_[1][2]; d_[1][3] -= other.d_[1][3];
        d_[2][0] -= other.d_[2][0]; d_[2][1] -= other.d_[2][1]; d_[2][2] -= other.d_[2][2]; d_[2][3] -= other.d_[2][3];
        d_[3][0] -= other.d_[3][0]; d_[3][1] -= other.d_[3][1]; d_[3][2] -= other.d_[3][2]; d_[3][3] -= other.d_[3][3];
        return *this;
    }
    constexpr Matrix4T<T>& operator*=(const Matrix4T<T>& other) noexcept {
        d_[0][0] *= other.d_[0][0]; d_[0][1] *= other.d_[0][1]; d_[0][2] *= other.d_[0][2]; d_[0][3] *= other.d_[0][3];
        d_[1][0] *= other.d_[1][0]; d_[1][1] *= other.d_[1][1]; d_[1][2] *= other.d_[1][2]; d_[1][3] *= other.d_[1][3];
        d_[2][0] *= other.d_[2][0]; d_[2][1] *= other.d_[2][1]; d_[2][2] *= other.d_[2][2]; d_[2][3] *= other.d_[2][3];
        d_[3][0] *= other.d_[3][0]; d_[3][1] *= other.d_[3][1]; d_[3][2] *= other.d_[3][2]; d_[3][3] *= other.d_[3][3];
        return *this;
    }
    constexpr Matrix4T<T>& operator/=(const Matrix4T<T>& other) noexcept {
        d_[0][0] /= other.d_[0][0]; d_[0][1] /= other.d_[0][1]; d_[0][2] /= other.d_[0][2]; d_[0][3] /= other.d_[0][3];
        d_[1][0] /= other.d_[1][0]; d_[1][1] /= other.d_[1][1]; d_[1][2] /= other.d_[1][2]; d_[1][3] /= other.d_[1][3];
        d_[2][0] /= other.d_[2][0]; d_[2][1] /= other.d_[2][1]; d_[2][2] /= other.d_[2][2]; d_[2][3] /= other.d_[2][3];
        d_[3][0] /= other.d_[3][0]; d_[3][1] /= other.d_[3][1]; d_[3][2] /= other.d_[3][2]; d_[3][3] /= other.d_[3][3];
        return *this;
    }
    constexpr Matrix4T<T>& operator+=(T scalar) noexcept {
        d_[0][0] += scalar; d_[0][1] += scalar; d_[0][2] += scalar; d_[0][3] += scalar;
        d_[1][0] += scalar; d_[1][1] += scalar; d_[1][2] += scalar; d_[1][3] += scalar;
        d_[2][0] += scalar; d_[2][1] += scalar; d_[2][2] += scalar; d_[2][3] += scalar;
        d_[3][0] += scalar; d_[3][1] += scalar; d_[3][2] += scalar; d_[3][3] += scalar;
        return *this;
    }
    constexpr Matrix4T<T>& operator-=(T scalar) noexcept {
        d_[0][0] -= scalar; d_[0][1] -= scalar; d_[0][2] -= scalar; d_[0][3] -= scalar;
        d_[1][0] -= scalar; d_[1][1] -= scalar; d_[1][2] -= scalar; d_[1][3] -= scalar;
        d_[2][0] -= scalar; d_[2][1] -= scalar; d_[2][2] -= scalar; d_[2][3] -= scalar;
        d_[3][0] -= scalar; d_[3][1] -= scalar; d_[3][2] -= scalar; d_[3][3] -= scalar;
        return *this;
    }
    constexpr Matrix4T<T>& operator*=(T scalar) noexcept {
        d_[0][0] *= scalar; d_[0][1] *= scalar; d_[0][2] *= scalar; d_[0][3] *= scalar;
        d_[1][0] *= scalar; d_[1][1] *= scalar; d_[1][2] *= scalar; d_[1][3] *= scalar;
        d_[2][0] *= scalar; d_[2][1] *= scalar; d_[2][2] *= scalar; d_[2][3] *= scalar;
        d_[3][0] *= scalar; d_[3][1] *= scalar; d_[3][2] *= scalar; d_[3][3] *= scalar;
        return *this;
    }
    constexpr Matrix4T<T>& operator/=(T scalar) noexcept {
        d_[0][0] /= scalar; d_[0][1] /= scalar; d_[0][2] /= scalar; d_[0][3] /= scalar;
        d_[1][0] /= scalar; d_[1][1] /= scalar; d_[1][2] /= scalar; d_[1][3] /= scalar;
        d_[2][0] /= scalar; d_[2][1] /= scalar; d_[2][2] /= scalar; d_[2][3] /= scalar;
        d_[3][0] /= scalar; d_[3][1] /= scalar; d_[3][2] /= scalar; d_[3][3] /= scalar;
        return *this;
    }
    template<typename U> friend constexpr Matrix4T<U> operator+(U scalar, const Matrix4T<U>& matrix) noexcept;
    template<typename U> friend constexpr Matrix4T<U> operator-(U scalar, const Matrix4T<U>& matrix) noexcept;
    template<typename U> friend constexpr Matrix4T<U> operator*(U scalar, const Matrix4T<U>& matrix) noexcept;
    template<typename U> friend constexpr Matrix4T<U> operator/(U scalar, const Matrix4T<U>& matrix) noexcept;
    
    // negation
    constexpr Matrix4T<T> operator-() const noexcept {
        return {
            {-d_[0][0], -d_[0][1], -d_[0][2], -d_[0][3]},
            {-d_[1][0], -d_[1][1], -d_[1][2], -d_[1][3]},
            {-d_[2][0], -d_[2][1], -d_[2][2], -d_[2][3]},
            {-d_[3][0], -d_[3][1], -d_[3][2], -d_[3][3]}
        };
    }
    // comparison
    constexpr bool operator==(const Matrix4T<T>& other) const noexcept {
        return d_[0][0] == other.d_[0][0] && d_[0][1] == other.d_[0][1] && d_[0][2] == other.d_[0][2] && d_[0][3] == other.d_[0][3] &&
               d_[1][0] == other.d_[1][0] && d_[1][1] == other.d_[1][1] && d_[1][2] == other.d_[1][2] && d_[1][3] == other.d_[1][3] &&
               d_[2][0] == other.d_[2][0] && d_[2][1] == other.d_[2][1] && d_[2][2] == other.d_[2][2] && d_[2][3] == other.d_[2][3] &&
               d_[3][0] == other.d_[3][0] && d_[3][1] == other.d_[3][1] && d_[3][2] == other.d_[3][2] && d_[3][3] == other.d_[3][3];
    }
    // dot product
    constexpr Matrix4T<T> dot(const Matrix4T<T>& other) const noexcept {
        Matrix4T<T> result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result[i][j] = d_[i][0] * other[0][j] + d_[i][1] * other[1][j] + d_[i][2] * other[2][j] + d_[i][3] * other[3][j];
        return result;
    }
    constexpr Vector4T<T> dot(const Vector4T<T>& other) const noexcept {
        return {
            d_[0][0] * other.position.x + d_[0][1] * other.position.y + d_[0][2] * other.position.z + d_[0][3] * other.w,
            d_[1][0] * other.position.x + d_[1][1] * other.position.y + d_[1][2] * other.position.z + d_[1][3] * other.w,
            d_[2][0] * other.position.x + d_[2][1] * other.position.y + d_[2][2] * other.position.z + d_[2][3] * other.w,
            d_[3][0] * other.position.x + d_[3][1] * other.position.y + d_[3][2] * other.position.z + d_[3][3] * other.w
        };
    }

private:
    T d_[4][4];
};

template<typename T> constexpr Matrix4T<T> operator+(T scalar, const Matrix4T<T>& matrix) noexcept { return matrix + scalar; }
template<typename T> constexpr Matrix4T<T> operator-(T scalar, const Matrix4T<T>& matrix) noexcept {
    return {
        {scalar - matrix[0][0], scalar - matrix[0][1], scalar - matrix[0][2], scalar - matrix[0][3]},
        {scalar - matrix[1][0], scalar - matrix[1][1], scalar - matrix[1][2], scalar - matrix[1][3]},
        {scalar - matrix[2][0], scalar - matrix[2][1], scalar - matrix[2][2], scalar - matrix[2][3]},
        {scalar - matrix[3][0], scalar - matrix[3][1], scalar - matrix[3][2], scalar - matrix[3][3]}
    };
}
template<typename T> constexpr Matrix4T<T> operator*(T scalar, const Matrix4T<T>& matrix) noexcept { return matrix * scalar; }
template<typename T> constexpr Matrix4T<T> operator/(T scalar, const Matrix4T<T>& matrix) noexcept {
    return {
        {scalar / matrix[0][0], scalar / matrix[0][1], scalar / matrix[0][2], scalar / matrix[0][3]},
        {scalar / matrix[1][0], scalar / matrix[1][1], scalar / matrix[1][2], scalar / matrix[1][3]},
        {scalar / matrix[2][0], scalar / matrix[2][1], scalar / matrix[2][2], scalar / matrix[2][3]},
        {scalar / matrix[3][0], scalar / matrix[3][1], scalar / matrix[3][2], scalar / matrix[3][3]}
    };
}

using Vector2 = Vector2T<float>;
using Vector3 = Vector3T<float>;
using Vector4 = Vector4T<float>;

using Vector2i = Vector2T<int32_t>;
using Vector3i = Vector3T<int32_t>;
using Vector4i = Vector4T<int32_t>;

using Matrix4 = Matrix4T<float>;

class Vertex : public Vector4 {
public:
    constexpr Vertex() noexcept : Vector4(0.0f, 0.0f, 0.0f, 1.0f) {}
    constexpr Vertex(float x, float y, float z, float w) noexcept : Vector4(x, y, z, w) {}
    constexpr Vertex(const Vector3& other, float w) noexcept : Vector4(other, w) {}
    constexpr Vertex(const Vector3& other) noexcept : Vector4(other, 1.0f) {}
    // conversion between Vertex and Vector4
    constexpr Vertex(const Vector4& other) noexcept : Vector4(other) {}
    constexpr Vertex& operator=(const Vector4& other) noexcept { position.x = other.position.x; position.y = other.position.y; position.z = other.position.z; w = other.w; return *this; }
};

constexpr float degToRad(float deg) {
    constexpr float PI = 3.14159265358979323846f;
    return deg * PI / 180.0f;
}

constexpr void createScaleMatrix(Matrix4& matrix, Vector3 scale) {
    matrix = {
        {scale.x, 0.0f, 0.0f, 0.0f},
        {0.0f, scale.y, 0.0f, 0.0f},
        {0.0f, 0.0f, scale.z, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
}
constexpr Matrix4 createScaleMatrix(Vector3 scale) {
    Matrix4 matrix;
    createScaleMatrix(matrix, scale);
    return matrix;
}

constexpr void createRotationXMatrix(Matrix4& matrix, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    matrix = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, c, -s, 0.0f},
        {0.0f, s, c, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
}
constexpr Matrix4 createRotationXMatrix(float angle) {
    Matrix4 matrix;
    createRotationXMatrix(matrix, angle);
    return matrix;
}

constexpr void createRotationYMatrix(Matrix4& matrix, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    matrix = {
        {c, 0.0f, s, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {-s, 0.0f, c, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
}
constexpr Matrix4 createRotationYMatrix(float angle) {
    Matrix4 matrix;
    createRotationYMatrix(matrix, angle);
    return matrix;
}

constexpr void createRotationZMatrix(Matrix4& matrix, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    matrix = {
        {c, -s, 0.0f, 0.0f},
        {s, c, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
}
constexpr Matrix4 createRotationZMatrix(float angle) {
    Matrix4 matrix;
    createRotationZMatrix(matrix, angle);
    return matrix;
}

constexpr void createRotationMatrix(Matrix4& matrix, float angle, Vector3 axis) {
    // normalize the axis
    float length = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    if (length != 1.0f) {
        float inv_length = 1.0f / length;
        axis.x *= inv_length;
        axis.y *= inv_length;
        axis.z *= inv_length;
    }
    float c = std::cos(angle);
    float s = std::sin(angle);
    float t = 1.0f - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    matrix = {
        {t * x * x + c, t * x * y - s * z, t * x * z + s * y, 0.0f},
        {t * x * y + s * z, t * y * y + c, t * y * z - s * x, 0.0f},
        {t * x * z - s * y, t * y * z + s * x, t * z * z + c, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
}
constexpr Matrix4 createRotationMatrix(float angle, Vector3 axis) {
    Matrix4 matrix;
    createRotationMatrix(matrix, angle, axis);
    return matrix;
}

constexpr void createTranslationMatrix(Matrix4& matrix, Vector3 translation) {
    matrix = {
        {1.0f, 0.0f, 0.0f, translation.x},
        {0.0f, 1.0f, 0.0f, translation.y},
        {0.0f, 0.0f, 1.0f, translation.z},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
}
constexpr Matrix4 createTranslationMatrix(Vector3 translation) {
    Matrix4 matrix;
    createTranslationMatrix(matrix, translation);
    return matrix;
}

constexpr void createPerspectiveProjectionMatrix(Matrix4& matrix, float fov, float aspect, float near, float far) {
    float tanhf = std::tan(fov / 2.0f);
    float range = far - near;
    matrix = {
        {1.0f / (aspect * tanhf), 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f / tanhf, 0.0f, 0.0f},
        {0.0f, 0.0f, -(far + near) / range, -2.0f * far * near / range},
        {0.0f, 0.0f, -1.0f, 0.0f}
    };
}
constexpr Matrix4 createPerspectiveProjectionMatrix(float fov, float aspect, float near, float far) {
    Matrix4 matrix;
    createPerspectiveProjectionMatrix(matrix, fov, aspect, near, far);
    return matrix;
}

constexpr void createOrthographicProjectionMatrix(Matrix4& matrix, float left, float right, float bottom, float top, float near, float far) {
    float rml = right - left;
    float tmb = top - bottom;
    float fmn = far - near;
    matrix = {
        {2.0f / rml, 0.0f, 0.0f, -(right + left) / rml},
        {0.0f, 2.0f / tmb, 0.0f, -(top + bottom) / tmb},
        {0.0f, 0.0f, -2.0f / fmn, -(far + near) / fmn},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
}
constexpr Matrix4 createOrthographicProjectionMatrix(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 matrix;
    createOrthographicProjectionMatrix(matrix, left, right, bottom, top, near, far);
    return matrix;
}

constexpr void createViewMatrix(Matrix4& matrix, const Vector3& eye, const Vector3& center, const Vector3& up) {
    Vector3 f = (center - eye).normalized();
    Vector3 s = f.cross(up).normalized();
    Vector3 u = s.cross(f);
    matrix = {
        {s.x, s.y, s.z, -s.dot(eye)},
        {u.x, u.y, u.z, -u.dot(eye)},
        {-f.x, -f.y, -f.z, f.dot(eye)},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
}
constexpr Matrix4 createViewMatrix(const Vector3& eye, const Vector3& center, const Vector3& up) {
    Matrix4 matrix;
    createViewMatrix(matrix, eye, center, up);
    return matrix;
}

struct Triangle {
    const Vector3& v0;
    const Vector3& v1;
    const Vector3& v2;
    // precomputed 1/w (reciprocal) for each vertex
    float v0_reciprocal_w;
    float v1_reciprocal_w;
    float v2_reciprocal_w;
};

struct Barycentric {
    float l0, l1, l2;
};

inline constexpr Barycentric calculateBarycentric(const Triangle& triangle, const Vector2& p) {
    Vector2 v0 = Vector2(triangle.v1 - triangle.v0);
    Vector2 v1 = Vector2(triangle.v2 - triangle.v0);
    Vector2 v2 = p - Vector2(triangle.v0);
    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);
    float denom = d00 * d11 - d01 * d01;
    if (denom < 1e-6f) // if the triangle is degenerate (i.e. area is zero < 1e-6f)
        return {-1.0f, -1.0f, -1.0f};
    float l1 = (d11 * d20 - d01 * d21) / denom;
    float l2 = (d00 * d21 - d01 * d20) / denom;
    float l0 = 1.0f - l1 - l2;
    return {l0, l1, l2};
}

}
