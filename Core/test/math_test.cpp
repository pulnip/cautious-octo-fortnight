#include <gtest/gtest.h>
#include <cmath>
#include "math.hpp"

using namespace RenderToy;

namespace
{
    constexpr float kEpsilon = 1e-5f;

    inline constexpr bool nearEq(float a, float b, float eps = kEpsilon){
        return std::abs(a - b) < eps;
    }
    inline constexpr bool nearEq(Vec3 a, Vec3 b, float eps = kEpsilon){
        return nearEq(a.x, b.x, eps) && nearEq(a.y, b.y, eps) && nearEq(a.z, b.z, eps);
    }
    inline constexpr bool nearEq(Vec4 a, Vec4 b, float eps = kEpsilon){
        return nearEq(a.x, b.x, eps) && nearEq(a.y, b.y, eps) && 
               nearEq(a.z, b.z, eps) && nearEq(a.w, b.w, eps);
    }
}

//==========================================================================
// Vec2 Tests
//==========================================================================

static_assert(Vec2{1, 2} + Vec2{3, 4} == Vec2{4, 6});
static_assert(Vec2{5, 7} - Vec2{2, 3} == Vec2{3, 4});
static_assert(-Vec2{1, -2} == Vec2{-1, 2});
static_assert(Vec2{2, 3} * 2.0f == Vec2{4, 6});
static_assert(3.0f * Vec2{2, 3} == Vec2{6, 9});
static_assert(Vec2{6, 9} / 3.0f == Vec2{2, 3});
static_assert(dot(Vec2{1, 2}, Vec2{3, 4}) == 11.0f);
static_assert(norm_squared(Vec2{3, 4}) == 25.0f);
static_assert(cross(Vec2{1, 0}, Vec2{0, 1}) == 1.0f);
static_assert(cross(Vec2{0, 1}, Vec2{1, 0}) == -1.0f);

TEST(Vec2Test, Norm){
    EXPECT_NEAR(norm(Vec2{3, 4}), 5.0f, kEpsilon);
}

TEST(Vec2Test, Normalize){
    auto n = normalize(Vec2{3, 4});
    EXPECT_NEAR(n.x, 0.6f, kEpsilon);
    EXPECT_NEAR(n.y, 0.8f, kEpsilon);
}

//==========================================================================
// Vec3 Tests
//==========================================================================

static_assert(zeros() == Vec3{0, 0, 0});
static_assert(ones() == Vec3{1, 1, 1});
static_assert(unitX() == Vec3{1, 0, 0});
static_assert(unitY() == Vec3{0, 1, 0});
static_assert(unitZ() == Vec3{0, 0, 1});

static_assert(Vec3{1, 2, 3} + Vec3{4, 5, 6} == Vec3{5, 7, 9});
static_assert(Vec3{4, 5, 6} - Vec3{1, 2, 3} == Vec3{3, 3, 3});
static_assert(-Vec3{1, -2, 3} == Vec3{-1, 2, -3});
static_assert(Vec3{1, 2, 3} * Vec3{2, 3, 4} == Vec3{2, 6, 12});  // hadamard
static_assert(2.0f * Vec3{1, 2, 3} == Vec3{2, 4, 6});
static_assert(Vec3{1, 2, 3} * 2.0f == Vec3{2, 4, 6});
static_assert(Vec3{2, 4, 6} / 2.0f == Vec3{1, 2, 3});

static_assert(dot(Vec3{1, 2, 3}, Vec3{4, 5, 6}) == 32.0f);
static_assert(norm_squared(Vec3{1, 2, 2}) == 9.0f);
static_assert(cross(unitX(), unitY()) == unitZ());
static_assert(cross(unitY(), unitZ()) == unitX());
static_assert(cross(unitZ(), unitX()) == unitY());

static_assert(asVec3(Vec2{1, 2}, 3) == Vec3{1, 2, 3});
static_assert(asVec4(Vec3{1, 2, 3}, 4) == Vec4{1, 2, 3, 4});
static_assert(asVec3(Vec4{1, 2, 3, 4}) == Vec3{1, 2, 3});

TEST(Vec3Test, Norm){
    EXPECT_NEAR(norm(Vec3{1, 2, 2}), 3.0f, kEpsilon);
}

TEST(Vec3Test, Normalize){
    auto n = normalize(Vec3{0, 3, 4});
    EXPECT_TRUE(nearEq(n, Vec3{0, 0.6f, 0.8f}));
}

TEST(Vec3Test, CompoundAssignment){
    Vec3 v{1, 2, 3};
    v += Vec3{1, 1, 1};
    EXPECT_EQ(v, (Vec3{2, 3, 4}));

    v -= Vec3{1, 1, 1};
    EXPECT_EQ(v, (Vec3{1, 2, 3}));

    v *= 2.0f;
    EXPECT_EQ(v, (Vec3{2, 4, 6}));
}

//==========================================================================
// Vec4 / Quaternion Tests
//==========================================================================

static_assert(unitQuat() == Vec4{0, 0, 0, 1});
static_assert(conjugate(Vec4{1, 2, 3, 4}) == Vec4{-1, -2, -3, 4});
static_assert(Vec4{1, 2, 3, 4} / 2.0f == Vec4{0.5f, 1, 1.5f, 2});
static_assert(dot(Vec4{1, 2, 3, 4}, Vec4{1, 1, 1, 1}) == 10.0f);
static_assert(norm_squared(Vec4{1, 2, 2, 4}) == 25.0f);

// quaternion multiplication: i*i = j*j = k*k = ijk = -1
static_assert(Vec4{1,0,0,0} * Vec4{1,0,0,0} == Vec4{0,0,0,-1});  // i*i = -1
static_assert(Vec4{0,1,0,0} * Vec4{0,1,0,0} == Vec4{0,0,0,-1});  // j*j = -1
static_assert(Vec4{0,0,1,0} * Vec4{0,0,1,0} == Vec4{0,0,0,-1});  // k*k = -1
static_assert(Vec4{1,0,0,0} * Vec4{0,1,0,0} == Vec4{0,0,1,0});   // i*j = k
static_assert(Vec4{0,1,0,0} * Vec4{0,0,1,0} == Vec4{1,0,0,0});   // j*k = i
static_assert(Vec4{0,0,1,0} * Vec4{1,0,0,0} == Vec4{0,1,0,0});   // k*i = j

// q * conjugate(q) = |q|^2
static_assert(Vec4{0,0,0,1} * conjugate(Vec4{0,0,0,1}) == Vec4{0,0,0,1});

// identity quaternion rotation
static_assert(right(unitQuat()) == unitX());
static_assert(up(unitQuat()) == unitY());
static_assert(forward(unitQuat()) == unitZ());

TEST(Vec4Test, Norm){
    EXPECT_NEAR(norm(Vec4{1, 2, 2, 4}), 5.0f, kEpsilon);
}

TEST(Vec4Test, Normalize){
    auto n = normalize(Vec4{0, 0, 3, 4});
    EXPECT_TRUE(nearEq(n, Vec4{0, 0, 0.6f, 0.8f}));
}

TEST(QuaternionTest, RotateX90){
    constexpr float pi = 3.14159265358979f;
    auto q = rotateX(pi / 2);

    // X축 90도 회전: Y -> Z, Z -> -Y
    auto rotatedY = rotate(unitY(), q);
    auto rotatedZ = rotate(unitZ(), q);

    EXPECT_TRUE(nearEq(rotatedY, unitZ()));
    EXPECT_TRUE(nearEq(rotatedZ, -unitY()));
}

TEST(QuaternionTest, RotateY90){
    constexpr float pi = 3.14159265358979f;
    auto q = rotateY(pi / 2);

    // Y축 90도 회전: Z -> X, X -> -Z
    auto rotatedZ = rotate(unitZ(), q);
    auto rotatedX = rotate(unitX(), q);

    EXPECT_TRUE(nearEq(rotatedZ, unitX()));
    EXPECT_TRUE(nearEq(rotatedX, -unitZ()));
}

TEST(QuaternionTest, RotateZ90){
    constexpr float pi = 3.14159265358979f;
    auto q = rotateZ(pi / 2);

    // Z축 90도 회전: X -> Y, Y -> -X
    auto rotatedX = rotate(unitX(), q);
    auto rotatedY = rotate(unitY(), q);

    EXPECT_TRUE(nearEq(rotatedX, unitY()));
    EXPECT_TRUE(nearEq(rotatedY, -unitX()));
}

TEST(QuaternionTest, AxisAngle){
    constexpr float pi = 3.14159265358979f;
    auto q = axisAngle(unitY(), pi);

    // Y축 180도 회전: X -> -X, Z -> -Z
    auto rotatedX = rotate(unitX(), q);
    auto rotatedZ = rotate(unitZ(), q);

    EXPECT_TRUE(nearEq(rotatedX, -unitX()));
    EXPECT_TRUE(nearEq(rotatedZ, -unitZ()));
}

TEST(QuaternionTest, DirectionVectors){
    constexpr float pi = 3.14159265358979f;
    auto q = rotateY(pi / 2);  // Y축으로 90도

    // right: X -> -Z
    // forward: Z -> X
    EXPECT_TRUE(nearEq(right(q), -unitZ()));
    EXPECT_TRUE(nearEq(forward(q), unitX()));
    EXPECT_TRUE(nearEq(up(q), unitY()));  // Y축 회전이라 up은 그대로
}

TEST(QuaternionTest, FromBasis){
    // identity basis -> identity quat
    auto q = quat(unitX(), unitY(), unitZ());
    EXPECT_TRUE(nearEq(q, unitQuat()) || nearEq(q, Vec4{0,0,0,-1}));  // sign ambiguity
}

//==========================================================================
// Mat4 Tests
//==========================================================================

static_assert(unitMat()[0] == Vec4{1, 0, 0, 0});
static_assert(unitMat()[1] == Vec4{0, 1, 0, 0});
static_assert(unitMat()[2] == Vec4{0, 0, 1, 0});
static_assert(unitMat()[3] == Vec4{0, 0, 0, 1});

static_assert(transpose(unitMat()) == unitMat());

// transpose 검증
constexpr Mat4 testMat{
    Vec4{1, 2, 3, 4},
    Vec4{5, 6, 7, 8},
    Vec4{9, 10, 11, 12},
    Vec4{13, 14, 15, 16}
};
constexpr Mat4 testMatT{
    Vec4{1, 5, 9, 13},
    Vec4{2, 6, 10, 14},
    Vec4{3, 7, 11, 15},
    Vec4{4, 8, 12, 16}
};
static_assert(transpose(testMat) == testMatT);
static_assert(transpose(transpose(testMat)) == testMat);

// M * I = M
static_assert(unitMat() * unitMat() == unitMat());

// M * v with identity
static_assert(unitMat() * Vec4{1, 2, 3, 4} == Vec4{1, 2, 3, 4});

TEST(Mat4Test, MatrixMultiplication){
    // 간단한 scale matrix
    Mat4 scale2x{
        Vec4{2, 0, 0, 0},
        Vec4{0, 2, 0, 0},
        Vec4{0, 0, 2, 0},
        Vec4{0, 0, 0, 1}
    };

    auto result = scale2x * scale2x;
    Mat4 expected{
        Vec4{4, 0, 0, 0},
        Vec4{0, 4, 0, 0},
        Vec4{0, 0, 4, 0},
        Vec4{0, 0, 0, 1}
    };

    EXPECT_EQ(result, expected);
}

TEST(Mat4Test, MatrixVectorMultiply){
    Mat4 scale{
        Vec4{2, 0, 0, 0},
        Vec4{0, 3, 0, 0},
        Vec4{0, 0, 4, 0},
        Vec4{0, 0, 0, 1}
    };

    auto v = scale * Vec4{1, 1, 1, 1};
    EXPECT_TRUE(nearEq(v, Vec4{2, 3, 4, 1}));
}

//==========================================================================
// Perspective Tests
//==========================================================================

TEST(PerspectiveTest, BasicProperties){
    constexpr float pi = 3.14159265358979f;
    auto proj = perspective(pi / 2, 16.0f / 9.0f, 0.1f, 100.0f);

    // fovY = 90도면 tan(45도) = 1 이므로 e11 = 1
    EXPECT_NEAR(proj[1][1], 1.0f, kEpsilon);

    // aspect ratio 반영
    EXPECT_NEAR(proj[0][0], 9.0f / 16.0f, kEpsilon);

    // w' = -z를 위한 -1
    EXPECT_NEAR(proj[3][2], -1.0f, kEpsilon);

    // 나머지 0 확인
    EXPECT_NEAR(proj[3][3], 0.0f, kEpsilon);
}

TEST(PerspectiveTest, NearFarMapping){
    constexpr float pi = 3.14159265358979f;
    float nearZ = 0.1f;
    float farZ = 100.0f;
    auto proj = perspective(pi / 4, 1.0f, nearZ, farZ);

    // near plane (z = -nearZ) -> NDC z = 0 (Metal/DX convention)
    Vec4 nearPoint{0, 0, -nearZ, 1};
    auto nearClip = proj * nearPoint;
    float nearNDC = nearClip.z / nearClip.w;
    EXPECT_NEAR(nearNDC, 0.0f, kEpsilon);

    // far plane (z = -farZ) -> NDC z = 1
    Vec4 farPoint{0, 0, -farZ, 1};
    auto farClip = proj * farPoint;
    float farNDC = farClip.z / farClip.w;
    EXPECT_NEAR(farNDC, 1.0f, kEpsilon);
}

//==========================================================================
// Ground projection tests
//==========================================================================

TEST(QuaternionTest, GroundForward){
    constexpr float pi = 3.14159265358979f;

    // 45도 아래를 보는 회전
    auto q = rotateX(pi / 4);
    auto gf = ground_forward(q);

    // ground_forward는 Y 성분 제거
    EXPECT_NEAR(gf.y, 0.0f, kEpsilon);
    // Z 방향 성분은 남아있어야 함
    EXPECT_GT(gf.z, 0.0f);
}

TEST(QuaternionTest, GroundRight){
    constexpr float pi = 3.14159265358979f;

    // Y축 회전은 ground_right에 영향 없음 (수평 회전)
    auto q = rotateY(pi / 4);
    auto gr = ground_right(q);

    EXPECT_NEAR(gr.y, 0.0f, kEpsilon);
}

//==========================================================================
// LookAt tests
//==========================================================================

TEST(LookAtTest, Basic){
    auto eye = Vec3{0, 0, 5};
    auto target = Vec3{0, 0, 0};
    auto up = Vec3{0, 1, 0};

    auto view = lookAt(eye, target, up);

    // eye 위치를 변환하면 원점이 돼야 함
    auto eyeH = Vec4{eye.x, eye.y, eye.z, 1.0f};
    auto result = view * eyeH;

    EXPECT_NEAR(result.x, 0.0f, kEpsilon);
    EXPECT_NEAR(result.y, 0.0f, kEpsilon);
    EXPECT_NEAR(result.z, 0.0f, kEpsilon);
    EXPECT_NEAR(result.w, 1.0f, kEpsilon);

    // target은 -Z 방향에 있어야 함
    auto targetH = Vec4{target.x, target.y, target.z, 1.0f};
    auto targetView = view * targetH;

    EXPECT_NEAR(targetView.x, 0.0f, kEpsilon);
    EXPECT_NEAR(targetView.y, 0.0f, kEpsilon);
    EXPECT_LT(targetView.z, 0.0f);  // -Z 방향
}

//==========================================================================
// Transform Matrix Tests
//==========================================================================

static_assert(translateMat(Vec3{0, 0, 0}) == unitMat());
static_assert(translateMat(Vec3{1, 2, 3})[0] == Vec4{1, 0, 0, 1});
static_assert(translateMat(Vec3{1, 2, 3})[1] == Vec4{0, 1, 0, 2});
static_assert(translateMat(Vec3{1, 2, 3})[2] == Vec4{0, 0, 1, 3});
static_assert(translateMat(Vec3{1, 2, 3})[3] == Vec4{0, 0, 0, 1});

static_assert(scaleMat(Vec3{1, 1, 1}) == unitMat());
static_assert(scaleMat(Vec3{2, 3, 4})[0] == Vec4{2, 0, 0, 0});
static_assert(scaleMat(Vec3{2, 3, 4})[1] == Vec4{0, 3, 0, 0});
static_assert(scaleMat(Vec3{2, 3, 4})[2] == Vec4{0, 0, 4, 0});

static_assert(translateMat(Vec3{1, 2, 3}) * Vec4{0, 0, 0, 1} == Vec4{1, 2, 3, 1});
static_assert(scaleMat(Vec3{2, 3, 4}) * Vec4{1, 1, 1, 1} == Vec4{2, 3, 4, 1});

TEST(TranslateMatTest, Basic){
    auto t = translateMat(Vec3{3, 4, 5});
    auto v = Vec4{1, 2, 3, 1};

    auto result = t * v;

    EXPECT_TRUE(nearEq(result, Vec4{4, 6, 8, 1}));
}

TEST(TranslateMatTest, Composition){
    auto t1 = translateMat(Vec3{1, 0, 0});
    auto t2 = translateMat(Vec3{0, 2, 0});

    auto combined = t1 * t2;
    auto v = Vec4{0, 0, 0, 1};

    auto result = combined * v;

    EXPECT_TRUE(nearEq(result, Vec4{1, 2, 0, 1}));
}

TEST(ScaleMatTest, Basic){
    auto s = scaleMat(Vec3{2, 3, 4});
    auto v = Vec4{1, 1, 1, 1};

    auto result = s * v;

    EXPECT_TRUE(nearEq(result, Vec4{2, 3, 4, 1}));
}

TEST(RotateXMatTest, Basic){
    constexpr float pi = 3.14159265358979f;
    auto r = rotateXMat(pi / 2);  // 90도

    // Y -> Z
    auto y = Vec4{0, 1, 0, 1};
    auto result = r * y;
    EXPECT_TRUE(nearEq(result, Vec4{0, 0, 1, 1}));

    // Z -> -Y
    auto z = Vec4{0, 0, 1, 1};
    result = r * z;
    EXPECT_TRUE(nearEq(result, Vec4{0, -1, 0, 1}));
}

TEST(RotateYMatTest, Basic){
    constexpr float pi = 3.14159265358979f;
    auto r = rotateYMat(pi / 2);  // 90도

    // Z -> X
    auto z = Vec4{0, 0, 1, 1};
    auto result = r * z;
    EXPECT_TRUE(nearEq(result, Vec4{1, 0, 0, 1}));

    // X -> -Z
    auto x = Vec4{1, 0, 0, 1};
    result = r * x;
    EXPECT_TRUE(nearEq(result, Vec4{0, 0, -1, 1}));
}

TEST(RotateZMatTest, Basic){
    constexpr float pi = 3.14159265358979f;
    auto r = rotateZMat(pi / 2);  // 90도

    // X -> Y
    auto x = Vec4{1, 0, 0, 1};
    auto result = r * x;
    EXPECT_TRUE(nearEq(result, Vec4{0, 1, 0, 1}));

    // Y -> -X
    auto y = Vec4{0, 1, 0, 1};
    result = r * y;
    EXPECT_TRUE(nearEq(result, Vec4{-1, 0, 0, 1}));
}

TEST(RotateMatTest, ConsistentWithQuat){
    constexpr float pi = 3.14159265358979f;
    float theta = pi / 3;  // 60도

    // 행렬 회전
    auto rMat = rotateYMat(theta);
    auto v = Vec4{1, 0, 0, 1};
    auto matResult = asVec3(rMat * v);

    // 쿼터니언 회전
    auto q = rotateY(theta);
    auto quatResult = rotate(Vec3{1, 0, 0}, q);

    EXPECT_TRUE(nearEq(matResult, quatResult));
}

TEST(TransformTest, TRS){
    // 일반적인 TRS 순서: Translation * Rotation * Scale
    constexpr float pi = 3.14159265358979f;

    auto T = translateMat(Vec3{10, 0, 0});
    auto R = rotateZMat(pi / 2);
    auto S = scaleMat(Vec3{2, 2, 2});

    auto TRS = T * R * S;

    // (1,0,0) -> scale -> (2,0,0) -> rotZ 90 -> (0,2,0) -> translate -> (10,2,0)
    auto v = Vec4{1, 0, 0, 1};
    auto result = TRS * v;

    EXPECT_TRUE(nearEq(result, Vec4{10, 2, 0, 1}));
}

TEST(LookAtTest, Eye){
    auto eye = Vec3{0, 0, 5};
    auto target = Vec3{0, 0, 0};
    auto up = Vec3{0, 1, 0};

    auto view = lookAt(eye, target, up);

    auto eyeH = Vec4{eye.x, eye.y, eye.z, 1.0f};
    auto result = view * eyeH;

    EXPECT_NEAR(result.x, 0.0f, kEpsilon);
    EXPECT_NEAR(result.y, 0.0f, kEpsilon);
    EXPECT_NEAR(result.z, 0.0f, kEpsilon);
    EXPECT_NEAR(result.w, 1.0f, kEpsilon);
}

TEST(LookAtTest, TargetInNegZ){
    auto eye = Vec3{0, 0, 5};
    auto target = Vec3{0, 0, 0};
    auto up = Vec3{0, 1, 0};

    auto view = lookAt(eye, target, up);

    auto targetH = Vec4{target.x, target.y, target.z, 1.0f};
    auto result = view * targetH;

    EXPECT_NEAR(result.x, 0.0f, kEpsilon);
    EXPECT_NEAR(result.y, 0.0f, kEpsilon);
    EXPECT_LT(result.z, 0.0f);  // -Z 방향
}

TEST(LookAtTest, UpPreserved){
    auto eye = Vec3{0, 0, 5};
    auto target = Vec3{0, 0, 0};
    auto up = Vec3{0, 1, 0};

    auto view = lookAt(eye, target, up);

    // eye 위 방향 점
    auto aboveEye = Vec4{0, 1, 5, 1};
    auto result = view * aboveEye;

    // view space에서도 +Y 방향이어야 함
    EXPECT_NEAR(result.x, 0.0f, kEpsilon);
    EXPECT_GT(result.y, 0.0f);
}