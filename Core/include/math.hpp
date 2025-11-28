#pragma once

#include <array>
#include <cmath>
#include <type_traits>

namespace RenderToy
{
    union Vec2{
        float v[2];
        struct{ float x, y; };

        inline auto operator[](size_t i)->float&{ return v[i]; }
        inline const float& operator[](size_t i) const{ return v[i]; }
    }; static_assert(std::is_trivially_copyable_v<Vec2>);
    union Vec3{
        float v[3];
        struct{ float x, y, z; };
        struct{ float r, g, b; };

        inline auto operator[](size_t i)->float&{ return v[i]; }
        inline const float& operator[](size_t i) const{ return v[i]; }
    }; static_assert(std::is_trivially_copyable_v<Vec3>);
    union Vec4{
        float v[4];
        struct{ float x, y, z, w; };
        struct{ float r, g, b, a; };

        inline auto operator[](size_t i)->float&{ return v[i]; }
        inline const float& operator[](size_t i) const{ return v[i]; }
    }; static_assert(std::is_trivially_copyable_v<Vec4>);

    using Mat4 = std::array<Vec4, 4>;
    static_assert(std::is_trivially_copyable_v<Mat4>);

    constexpr auto asVec3(Vec2 v2, float z=0.0f){
        return Vec3{.x=v2.x, .y=v2.y, .z=z};
    }
    constexpr auto asVec4(Vec3 v3, float w=0.0f){
        return Vec4{.x=v3.x, .y=v3.y, .z=v3.z, .w=w};
    }
    constexpr auto asVec3(Vec4 v4){
        return Vec3{.x=v4.x, .y=v4.y, .z=v4.z};
    }

    constexpr auto operator+(Vec2 lhs, Vec2 rhs){
        return Vec2{.x=lhs.x+rhs.x, .y=lhs.y+rhs.y};
    }
    constexpr auto operator-(Vec2 lhs, Vec2 rhs){
        return Vec2{.x=lhs.x-rhs.x, .y=lhs.y-rhs.y};
    }
    constexpr auto operator-(Vec2 v){
        return Vec2{.x=-v.x, .y=-v.y};
    }
    constexpr auto operator*(Vec2 v, float f){
        return Vec2{.x=v.x*f, .y=v.y*f};
    }
    constexpr auto operator*(float f, Vec2 v){
        return Vec2{.x=f*v.x, .y=f*v.y};
    }
    constexpr auto operator/(Vec2 v, float f){
        return Vec2{.x=v.x/f, .y=v.y/f};
    }

    constexpr auto operator==(Vec2 lhs, Vec2 rhs){
        return lhs.x==rhs.x && lhs.y==rhs.y;
    }

    constexpr auto dot(Vec2 lhs, Vec2 rhs){
        return lhs.x*rhs.x + lhs.y*rhs.y;
    }
    constexpr auto norm_squared(Vec2 v){
        return dot(v, v);
    }
    inline auto norm(Vec2 v){
        return std::sqrt(norm_squared(v));
    }
    inline auto normalize(Vec2 v){
        return v / norm(v);
    }
    constexpr auto cross(Vec2 lhs, Vec2 rhs){
        return lhs.x*rhs.y - lhs.y*rhs.x;
    }

    constexpr auto zeros(){
        return Vec3{.x=0.0f, .y=0.0f, .z=0.0f}; }
    constexpr auto ones(){
        return Vec3{.x=1.0f, .y=1.0f, .z=1.0f}; }
    constexpr auto unitX(){
        return Vec3{.x=1.0f, .y=0.0f, .z=0.0f}; }
    constexpr auto unitY(){
        return Vec3{.x=0.0f, .y=1.0f, .z=0.0f}; }
    constexpr auto unitZ(){
        return Vec3{.x=0.0f, .y=0.0f, .z=1.0f}; }

    constexpr auto unitMat(){
        return Mat4{
            Vec4{.x=1.0f, .y=0.0f, .z=0.0f, .w=0.0f},
            Vec4{.x=0.0f, .y=1.0f, .z=0.0f, .w=0.0f},
            Vec4{.x=0.0f, .y=0.0f, .z=1.0f, .w=0.0f},
            Vec4{.x=0.0f, .y=0.0f, .z=0.0f, .w=1.0f}
        };
    }

    constexpr auto operator+(Vec3 lhs, Vec3 rhs){
        return Vec3{.x=lhs.x+rhs.x, .y=lhs.y+rhs.y, .z=lhs.z+rhs.z};
    }
    constexpr auto operator+=(Vec3& lhs, Vec3 rhs)->Vec3&{
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        return lhs;
    }
    constexpr auto operator-(Vec3 v){
        return Vec3{.x=-v.x, .y=-v.y, .z=-v.z};
    }
    constexpr auto operator-(Vec3 lhs, Vec3 rhs){
        return Vec3{.x=lhs.x-rhs.x, .y=lhs.y-rhs.y, .z=lhs.z-rhs.z};
    }
    constexpr auto operator-=(Vec3& lhs, Vec3 rhs)->Vec3&{
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        return lhs;
    }
    constexpr auto operator*(Vec3 lhs, Vec3 rhs){
        return Vec3{
            .x = lhs.x * rhs.x,
            .y = lhs.y * rhs.y,
            .z = lhs.z * rhs.z
        };
    }
    constexpr auto operator*(float f, Vec3 v){
        return Vec3{
            .x = f*v.x,
            .y = f*v.y,
            .z = f*v.z
        };
    }
    constexpr auto operator*(Vec3 v, float f){
        return f*v;
    }
    constexpr auto operator*=(Vec3& v, float f)->Vec3&{
        v.x *= f;
        v.y *= f;
        v.z *= f;
        return v;
    }
    constexpr auto operator/(Vec3 v, float f){
        return Vec3{
            .x = v.x/f,
            .y = v.y/f,
            .z = v.z/f
        };
    }

    constexpr auto operator==(Vec3 lhs, Vec3 rhs){
        return lhs.x==rhs.x && lhs.y==rhs.y && lhs.z==rhs.z;
    }

    constexpr auto dot(Vec3 lhs, Vec3 rhs){
        return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
    }
    constexpr auto norm_squared(Vec3 v){
        return dot(v, v);
    }
    inline auto norm(Vec3 v){
        return std::sqrt(norm_squared(v));
    }
    inline auto normalize(Vec3 v){
        return v / norm(v);
    }
    constexpr auto cross(Vec3 lhs, Vec3 rhs){
        return Vec3{
            .x = lhs.y*rhs.z - lhs.z*rhs.y,
            .y = lhs.z*rhs.x - lhs.x*rhs.z,
            .z = lhs.x*rhs.y - lhs.y*rhs.x
        };
    }

    constexpr auto unitQuat(){
        return Vec4{.x=0, .y=0, .z=0, .w=1};
    }
    constexpr auto conjugate(Vec4 quat){
        return Vec4{
            .x = -quat.x,
            .y = -quat.y,
            .z = -quat.z,
            .w =  quat.w
        };
    }

    constexpr auto operator*(Vec4 lhs, Vec4 rhs){
        return Vec4{
            .x = lhs.w*rhs.x + lhs.x*rhs.w + lhs.y*rhs.z - lhs.z*rhs.y,
            .y = lhs.w*rhs.y - lhs.x*rhs.z + lhs.y*rhs.w + lhs.z*rhs.x,
            .z = lhs.w*rhs.z + lhs.x*rhs.y - lhs.y*rhs.x + lhs.z*rhs.w,
            .w = lhs.w*rhs.w - lhs.x*rhs.x - lhs.y*rhs.y - lhs.z*rhs.z
        };
    }
    constexpr auto operator/(Vec4 lhs, float rhs){
        return Vec4{
            .x = lhs.x/rhs,
            .y = lhs.y/rhs,
            .z = lhs.z/rhs,
            .w = lhs.w/rhs,
        };
    }
    constexpr auto dot(Vec4 lhs, Vec4 rhs){
        return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + rhs.w*rhs.w;
    }
    constexpr auto norm_squared(Vec4 v){
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
            .x = std::sinf(half),
            .y = 0.0f,
            .z = 0.0f,
            .w = std::cosf(half)
        };
    }
    inline auto rotateY(float theta){
        float half = theta * 0.5f;
        return Vec4{
            .x = 0.0f,
            .y = std::sinf(half),
            .z = 0.0f,
            .w = std::cosf(half)
        };
    }
    inline auto rotateZ(float theta){
        float half = theta * 0.5f;
        return Vec4{
            .x = 0.0f,
            .y = 0.0f,
            .z = std::sinf(half),
            .w = std::cosf(half)
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
        float s = std::sinf(half);
        return Vec4{
            .x = axis.x * s,
            .y = axis.y * s,
            .z = axis.z * s,
            .w = std::cosf(half)
        };
    }
    inline auto rotate(Vec3 v, Vec4 quat){
        return asVec3(quat * asVec4(v) * conjugate(quat));
    }

    constexpr auto right(Vec4 quat){
        auto e_x = Vec4{.x=1, .y=0, .z=0, .w=0};
        auto vec = quat * e_x * conjugate(quat);
        return Vec3{.x=vec.x, .y=vec.y, .z=vec.z };
    }
    constexpr auto ground_right(Vec4 quat){
        auto f = right(quat);
        return f - dot(f, unitY())*unitY();
    }
    constexpr auto up(Vec4 quat){
        auto e_y = Vec4{.x=0, .y=1, .z=0, .w=0};
        auto vec = quat * e_y * conjugate(quat);
        return Vec3{.x=vec.x, .y=vec.y, .z=vec.z };
    }
    constexpr auto forward(Vec4 quat){
        auto e_z = Vec4{.x=0, .y=0, .z=1, .w=0};
        auto vec = quat * e_z * conjugate(quat);
        return Vec3{.x=vec.x, .y=vec.y, .z=vec.z };
    }
    constexpr auto ground_forward(Vec4 quat){
        auto f = forward(quat);
        return f - dot(f, unitY())*unitY();
    }

    constexpr auto operator==(Vec4 lhs, Vec4 rhs){
        return lhs.x==rhs.x && lhs.y==rhs.y &&
               lhs.z==rhs.z && lhs.w==rhs.w;
    }
}
