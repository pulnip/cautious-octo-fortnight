#pragma once

#include <array>
#include <cmath>
#include <type_traits>

namespace RenderToy
{
    struct Vec2{
        float x, y;

        constexpr Vec2():x{0}, y{0}{}
        constexpr Vec2(float x, float y):x{x}, y{y} {}

        constexpr float& operator[](size_t i){ 
            if(i == 0) return x;
            return y;
        }
        constexpr const float& operator[](size_t i) const{ 
            if(i == 0) return x;
            return y;
        }
    }; static_assert(std::is_trivially_copyable_v<Vec2>);

    struct Vec3{
        float x, y, z;

        constexpr Vec3():x{0}, y{0}, z{0} {}
        constexpr Vec3(float x, float y, float z):x{x}, y{y}, z{z} {}

        constexpr float& operator[](size_t i){
            if(i == 0) return x;
            if(i == 1) return y;
            return z;
        }
        constexpr const float& operator[](size_t i) const{
            if(i == 0) return x;
            if(i == 1) return y;
            return z;
        }
    }; static_assert(std::is_trivially_copyable_v<Vec3>);

    struct Vec4{
        float x, y, z, w;

        constexpr Vec4():x{0}, y{0}, z{0}, w{0} {}
        constexpr Vec4(float x, float y, float z, float w) 
            :x{x}, y{y}, z{z}, w{w} {}

        constexpr float& operator[](size_t i){
            if(i == 0) return x;
            if(i == 1) return y;
            if(i == 2) return z;
            return w;
        }
        constexpr const float& operator[](size_t i) const{
            if(i == 0) return x;
            if(i == 1) return y;
            if(i == 2) return z;
            return w;
        }
    }; static_assert(std::is_trivially_copyable_v<Vec4>);

    // row major
    using Mat4 = std::array<Vec4, 4>;
    static_assert(std::is_trivially_copyable_v<Mat4>);

    inline constexpr auto asVec3(Vec2 v2, float z=0.0f){
        return Vec3{v2.x, v2.y, z};
    }
    inline constexpr auto asVec4(Vec3 v3, float w=0.0f){
        return Vec4{v3.x, v3.y, v3.z, w};
    }
    inline constexpr auto asVec3(Vec4 v4){
        return Vec3{v4.x, v4.y, v4.z};
    }

    inline constexpr auto operator+(Vec2 lhs, Vec2 rhs){
        return Vec2{lhs.x+rhs.x, lhs.y+rhs.y};
    }
    inline constexpr auto operator-(Vec2 lhs, Vec2 rhs){
        return Vec2{lhs.x-rhs.x, lhs.y-rhs.y};
    }
    inline constexpr auto operator-(Vec2 v){
        return Vec2{-v.x, -v.y};
    }
    inline constexpr auto operator*(Vec2 v, float f){
        return Vec2{v.x*f, v.y*f};
    }
    inline constexpr auto operator*(float f, Vec2 v){
        return Vec2{f*v.x, f*v.y};
    }
    inline constexpr auto operator/(Vec2 v, float f){
        return Vec2{v.x/f, v.y/f};
    }

    inline constexpr auto operator==(Vec2 lhs, Vec2 rhs){
        return lhs.x==rhs.x && lhs.y==rhs.y;
    }

    inline constexpr auto dot(Vec2 lhs, Vec2 rhs){
        return lhs.x*rhs.x + lhs.y*rhs.y;
    }
    inline constexpr auto norm_squared(Vec2 v){
        return dot(v, v);
    }
    inline auto norm(Vec2 v){
        return std::sqrt(norm_squared(v));
    }
    inline auto normalize(Vec2 v){
        return v / norm(v);
    }
    inline constexpr auto cross(Vec2 lhs, Vec2 rhs){
        return lhs.x*rhs.y - lhs.y*rhs.x;
    }

    inline constexpr auto zeros(){
        return Vec3{0.0f, 0.0f, 0.0f}; }
    inline constexpr auto ones(){
        return Vec3{1.0f, 1.0f, 1.0f}; }
    inline constexpr auto unitX(){
        return Vec3{1.0f, 0.0f, 0.0f}; }
    inline constexpr auto unitY(){
        return Vec3{0.0f, 1.0f, 0.0f}; }
    inline constexpr auto unitZ(){
        return Vec3{0.0f, 0.0f, 1.0f}; }

    inline constexpr auto unitMat(){
        return Mat4{
            Vec4{1.0f, 0.0f, 0.0f, 0.0f},
            Vec4{0.0f, 1.0f, 0.0f, 0.0f},
            Vec4{0.0f, 0.0f, 1.0f, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline constexpr auto operator+(Vec3 lhs, Vec3 rhs){
        return Vec3{lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z};
    }
    inline constexpr Vec3& operator+=(Vec3& lhs, Vec3 rhs){
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        return lhs;
    }
    inline constexpr auto operator-(Vec3 v){
        return Vec3{-v.x, -v.y, -v.z};
    }
    inline constexpr auto operator-(Vec3 lhs, Vec3 rhs){
        return Vec3{lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z};
    }
    inline constexpr Vec3& operator-=(Vec3& lhs, Vec3 rhs){
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        return lhs;
    }
    inline constexpr auto operator*(Vec3 lhs, Vec3 rhs){
        return Vec3{
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z
        };
    }
    inline constexpr auto operator*(float f, Vec3 v){
        return Vec3{
            f*v.x,
            f*v.y,
            f*v.z
        };
    }
    inline constexpr auto operator*(Vec3 v, float f){
        return f*v;
    }
    inline constexpr auto operator*=(Vec3& v, float f)->Vec3&{
        v.x *= f;
        v.y *= f;
        v.z *= f;
        return v;
    }
    inline constexpr auto operator/(Vec3 v, float f){
        return Vec3{
            v.x/f,
            v.y/f,
            v.z/f
        };
    }

    inline constexpr auto operator==(Vec3 lhs, Vec3 rhs){
        return lhs.x==rhs.x && lhs.y==rhs.y && lhs.z==rhs.z;
    }

    inline constexpr auto dot(Vec3 lhs, Vec3 rhs){
        return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
    }
    inline constexpr auto norm_squared(Vec3 v){
        return dot(v, v);
    }
    inline auto norm(Vec3 v){
        return std::sqrt(norm_squared(v));
    }
    inline auto normalize(Vec3 v){
        return v / norm(v);
    }
    inline constexpr auto cross(Vec3 lhs, Vec3 rhs){
        return Vec3{
            lhs.y*rhs.z - lhs.z*rhs.y,
            lhs.z*rhs.x - lhs.x*rhs.z,
            lhs.x*rhs.y - lhs.y*rhs.x
        };
    }

    inline constexpr auto unitQuat(){
        return Vec4{0.0f, 0.0f, 0.0f, 1.0f};
    }
    inline constexpr auto transpose(const Mat4 mat){
        return Mat4{
            Vec4{mat[0][0], mat[1][0], mat[2][0], mat[3][0]},
            Vec4{mat[0][1], mat[1][1], mat[2][1], mat[3][1]},
            Vec4{mat[0][2], mat[1][2], mat[2][2], mat[3][2]},
            Vec4{mat[0][3], mat[1][3], mat[2][3], mat[3][3]}
        };
    }

    inline constexpr auto conjugate(Vec4 quat){
        return Vec4{-quat.x, -quat.y, -quat.z, quat.w};
    }

    inline constexpr auto operator*(Vec4 lhs, Vec4 rhs){
        return Vec4{
            lhs.w*rhs.x + lhs.x*rhs.w + lhs.y*rhs.z - lhs.z*rhs.y,
            lhs.w*rhs.y - lhs.x*rhs.z + lhs.y*rhs.w + lhs.z*rhs.x,
            lhs.w*rhs.z + lhs.x*rhs.y - lhs.y*rhs.x + lhs.z*rhs.w,
            lhs.w*rhs.w - lhs.x*rhs.x - lhs.y*rhs.y - lhs.z*rhs.z
        };
    }
    inline constexpr auto operator/(Vec4 lhs, float rhs){
        return Vec4{
            lhs.x/rhs,
            lhs.y/rhs,
            lhs.z/rhs,
            lhs.w/rhs,
        };
    }
    inline constexpr auto dot(Vec4 lhs, Vec4 rhs){
        return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + lhs.w*rhs.w;
    }
    inline constexpr auto norm_squared(Vec4 v){
        return dot(v, v);
    }
    inline auto norm(Vec4 v){
        return std::sqrt(norm_squared(v));
    }
    inline auto normalize(Vec4 v){
        return v / norm(v);
    }
        inline auto quat(Vec3 r, Vec3 u, Vec3 f){
        float m00 = r.x, m01 = u.x, m02 = f.x;
        float m10 = r.y, m11 = u.y, m12 = f.y;
        float m20 = r.z, m21 = u.z, m22 = f.z;

        float trace = m00 + m11 + m22;
        Vec4 q;

        if(trace > 0.0f){
            float s = std::sqrt(trace + 1.0f) * 2.0f;
            q.x = (m21 - m12) / s;
            q.y = (m02 - m20) / s;
            q.z = (m10 - m01) / s;
            q.w = 0.25f * s;
        } else if((m00 > m11) && (m00 > m22)){
            float s = std::sqrt(1.0f + m00 - m11 - m22) * 2.0f;
            q.x = 0.25f * s;
            q.y = (m01 + m10) / s;
            q.z = (m02 + m20) / s;
            q.w = (m21 - m12) / s;
        } else if(m11 > m22){
            float s = std::sqrt(1.0f + m11 - m00 - m22) * 2.0f;
            q.x = (m01 + m10) / s;
            q.y = 0.25f * s;
            q.z = (m12 + m21) / s;
            q.w = (m02 - m20) / s;
        } else {
            float s = std::sqrt(1.0f + m22 - m00 - m11) * 2.0f; // s=4*q.z
            q.x = (m02 + m20) / s;
            q.y = (m12 + m21) / s;
            q.z = 0.25f * s;
            q.w = (m10 - m01) / s;
        }

        return normalize(q);
    }

    inline auto rotateX(float theta){
        float half = theta * 0.5f;
        return Vec4{
            std::sin(half),
            0.0f,
            0.0f,
            std::cos(half)
        };
    }
    inline auto rotateY(float theta){
        float half = theta * 0.5f;
        return Vec4{
            0.0f,
            std::sin(half),
            0.0f,
            std::cos(half)
        };
    }
    inline auto rotateZ(float theta){
        float half = theta * 0.5f;
        return Vec4{
            0.0f,
            0.0f,
            std::sin(half),
            std::cos(half)
        };
    }
    inline auto yaw(Vec4 quat){
        float siny_cosp = 2*(quat.w*quat.y + quat.x*quat.z);
        float cosy_cosp = 1 - 2*(quat.y*quat.y + quat.x*quat.x);
        float theta = std::atan2(siny_cosp, cosy_cosp);
        return rotateY(theta);
    }
    inline auto axisAngle(Vec3 axis, float radian){
        auto half = radian / 2;
        float s = std::sin(half);
        return Vec4{
            axis.x * s,
            axis.y * s,
            axis.z * s,
            std::cos(half)
        };
    }
    inline auto rotate(Vec3 v, Vec4 quat){
        return asVec3(quat * asVec4(v) * conjugate(quat));
    }

    inline constexpr auto right(Vec4 quat){
        auto e_x = Vec4{1.0f, 0.0f, 0.0f, 0.0f};
        auto vec = quat * e_x * conjugate(quat);
        return asVec3(vec);
    }
    inline constexpr auto ground_right(Vec4 quat){
        auto f = right(quat);
        return f - dot(f, unitY())*unitY();
    }
    inline constexpr auto up(Vec4 quat){
        auto e_y = Vec4{0.0f, 1.0f, 0.0f, 0.0f};
        auto vec = quat * e_y * conjugate(quat);
        return asVec3(vec);
    }
    inline constexpr auto forward(Vec4 quat){
        auto e_z = Vec4{0.0f, 0.0f, 1.0f, 0.0f};
        auto vec = quat * e_z * conjugate(quat);
        return asVec3(vec);
    }
    inline constexpr auto ground_forward(Vec4 quat){
        auto f = forward(quat);
        return f - dot(f, unitY())*unitY();
    }

    inline constexpr auto operator==(Vec4 lhs, Vec4 rhs){
        return lhs.x==rhs.x && lhs.y==rhs.y &&
               lhs.z==rhs.z && lhs.w==rhs.w;
    }

    inline constexpr auto operator*(const Mat4& lhs, const Mat4& rhs){
        auto rhs_t = transpose(rhs);

        return Mat4{
            Vec4{dot(lhs[0], rhs_t[0]), dot(lhs[0], rhs_t[1]), dot(lhs[0], rhs_t[2]), dot(lhs[0], rhs_t[3])},
            Vec4{dot(lhs[1], rhs_t[0]), dot(lhs[1], rhs_t[1]), dot(lhs[1], rhs_t[2]), dot(lhs[1], rhs_t[3])},
            Vec4{dot(lhs[2], rhs_t[0]), dot(lhs[2], rhs_t[1]), dot(lhs[2], rhs_t[2]), dot(lhs[2], rhs_t[3])},
            Vec4{dot(lhs[3], rhs_t[0]), dot(lhs[3], rhs_t[1]), dot(lhs[3], rhs_t[2]), dot(lhs[3], rhs_t[3])},
        };
    }

    // expected multiplication form
    inline constexpr auto operator*(const Mat4& lhs, const Vec4& rhs){
        return Vec4{
            dot(lhs[0], rhs),
            dot(lhs[1], rhs),
            dot(lhs[2], rhs),
            dot(lhs[3], rhs),
        };
    }

    inline constexpr auto perspective(
        float fovY, float aspect, float nearZ, float farZ
    ){
        auto tanHalfFovY = std::tan(0.5f * fovY);
        auto dz = nearZ - farZ;

        auto e00 = 1.0f / (aspect*tanHalfFovY);
        auto e11 = 1.0f / tanHalfFovY;
        auto e22 = farZ / dz;
        auto e23 = (farZ*nearZ) / dz;

        return Mat4{
            Vec4{ e00, 0.0f,  0.0f, 0.0f},
            Vec4{0.0f,  e11,  0.0f, 0.0f},
            Vec4{0.0f, 0.0f,   e22,  e23},
            Vec4{0.0f, 0.0f, -1.0f, 0.0f}
        };
    }

    inline auto lookAt(Vec3 eye, Vec3 target, Vec3 up){
        auto f = normalize(target - eye);
        auto r = normalize(cross(f, up));
        auto u = cross(r, f);

        return Mat4{
            Vec4{ r.x,  r.y,  r.z, -dot(r, eye)},
            Vec4{ u.x,  u.y,  u.z, -dot(u, eye)},
            Vec4{-f.x, -f.y, -f.z,  dot(f, eye)},
            Vec4{0.0f, 0.0f, 0.0f,         1.0f}
        };
    }

    inline auto rotateXMat(float theta){
        float c = std::cos(theta);
        float s = std::sin(theta);
        return Mat4{
            Vec4{1.0f, 0.0f, 0.0f, 0.0f},
            Vec4{0.0f,    c,   -s, 0.0f},
            Vec4{0.0f,    s,    c, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline auto rotateYMat(float theta){
        float c = std::cos(theta);
        float s = std::sin(theta);
        return Mat4{
            Vec4{   c, 0.0f,    s, 0.0f},
            Vec4{0.0f, 1.0f, 0.0f, 0.0f},
            Vec4{  -s, 0.0f,    c, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline auto rotateZMat(float theta){
        float c = std::cos(theta);
        float s = std::sin(theta);
        return Mat4{
            Vec4{   c,   -s, 0.0f, 0.0f},
            Vec4{   s,    c, 0.0f, 0.0f},
            Vec4{0.0f, 0.0f, 1.0f, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline constexpr auto translateMat(Vec3 t){
        return Mat4{
            Vec4{1.0f, 0.0f, 0.0f, t.x},
            Vec4{0.0f, 1.0f, 0.0f, t.y},
            Vec4{0.0f, 0.0f, 1.0f, t.z},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }

    inline constexpr auto scaleMat(Vec3 s){
        return Mat4{
            Vec4{ s.x, 0.0f, 0.0f, 0.0f},
            Vec4{0.0f,  s.y, 0.0f, 0.0f},
            Vec4{0.0f, 0.0f,  s.z, 0.0f},
            Vec4{0.0f, 0.0f, 0.0f, 1.0f}
        };
    }
}
