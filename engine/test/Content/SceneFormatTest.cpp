#include <gtest/gtest.h>
#include "Content/SceneFormat.hpp"

using namespace RenderToy;

// ============================================================================
// TransformDescriptor Tests
// ============================================================================

TEST(SceneFormat, TransformDescriptor_DefaultValues) {
    TransformDescriptor t;

    EXPECT_EQ(t.position, (Vec3{0.0f, 0.0f, 0.0f}));
    EXPECT_EQ(t.rotation, unitQuat());
    EXPECT_EQ(t.scale, (Vec3{1.0f, 1.0f, 1.0f}));
}

TEST(SceneFormat, TransformDescriptor_CustomValues) {
    TransformDescriptor t{
        .position = Vec3{1.0f, 2.0f, 3.0f},
        .rotation = Vec4{0.0f, 0.707f, 0.0f, 0.707f},  // 90 degrees around Y
        .scale = Vec3{2.0f, 2.0f, 2.0f}
    };

    EXPECT_EQ(t.position, (Vec3{1.0f, 2.0f, 3.0f}));
    EXPECT_EQ(t.scale, (Vec3{2.0f, 2.0f, 2.0f}));
}

// ============================================================================
// EntityDescriptor Tests
// ============================================================================

TEST(SceneFormat, EntityDescriptor_Empty){
    EntityDescriptor entity{
        .name = "EmptyEntity"
    };

    EXPECT_EQ(entity.name, "EmptyEntity");
    EXPECT_FALSE(entity.hasTransform());
    EXPECT_FALSE(entity.hasRenderObject());
    EXPECT_FALSE(entity.hasRigidbody());
}

TEST(SceneFormat, EntityDescriptor_WithTransform){
    EntityDescriptor entity{
        .name = "Cube",
        .transform = TransformDescriptor{
            .position = Vec3{1.0f, 2.0f, 3.0f}
        }
    };

    EXPECT_EQ(entity.name, "Cube");
    EXPECT_TRUE(entity.hasTransform());
    EXPECT_EQ(entity.transform->position, (Vec3{1.0f, 2.0f, 3.0f}));
}

// ============================================================================
// SceneDescriptor Tests
// ============================================================================

TEST(SceneFormat, SceneDescriptor_Empty) {
    SceneDescriptor scene;

    EXPECT_TRUE(scene.entities.empty());
}

TEST(SceneFormat, SceneDescriptor_WithEntities) {
    SceneDescriptor scene{
        .entities = {
            EntityDescriptor{
                .name = "Cube1",
                .transform = TransformDescriptor{}
            },
            EntityDescriptor{
                .name = "Cube2",
                .transform = TransformDescriptor{
                    .position = Vec3{5.0f, 0.0f, 0.0f}
                }
            }
        }
    };

    EXPECT_EQ(scene.entities.size(), 2);
    EXPECT_EQ(scene.entities[0].name, "Cube1");
    EXPECT_EQ(scene.entities[1].name, "Cube2");
}

TEST(SceneFormat, SceneDescriptor_FindEntity) {
    SceneDescriptor scene{
        .entities = {
            EntityDescriptor{.name = "Player"},
            EntityDescriptor{.name = "Enemy"}
        }
    };

    auto* player = scene.findEntity("Player");
    ASSERT_NE(player, nullptr);
    EXPECT_EQ(player->name, "Player");

    auto* notFound = scene.findEntity("DoesNotExist");
    EXPECT_EQ(notFound, nullptr);
}
