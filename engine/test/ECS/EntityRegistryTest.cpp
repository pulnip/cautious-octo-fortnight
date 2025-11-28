#include <gtest/gtest.h>
#include "ECS/EntityRegistry.hpp"

using namespace RenderToy;

TEST(ArchetypeView, TrivialSize){
    EntityRegistry registry;

    auto view = registry.query<Transform>();
    EXPECT_EQ(view.size(), 0);
}

TEST(ArchetypeView, ComplexSize){
    Vec4 testColors[] = {
        {0.1, 0.2, 0.3, 0.5},
        {0.4, 0.3, 0.9, 1.0},
        {0.5, 0.1, 0.0, 0.5}
    };
    EntityRegistry registry;
    for(size_t i=0; i<3; ++i){
        registry.createEntity(
            Transform{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .position = zeros(),
                .rotation = unitQuat(),
                .scale = ones()
            }
        );
        registry.createEntity(
            Transform{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .position = zeros(),
                .rotation = unitQuat(),
                .scale = ones()
            },
            Color{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .color = testColors[i]
            }
        );
        registry.createEntity(
            Transform{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .position = zeros(),
                .rotation = unitQuat(),
                .scale = ones()
            },
            Element{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .type = ElementType::WIND
            }
        );
    }

    auto t_view = registry.query<Transform>();
    auto c_view = registry.query<Color>();
    EXPECT_EQ(t_view.size(), 9);
    EXPECT_EQ(c_view.size(), 3);
}

TEST(ArchetypeView, SimpleQuery){
    EntityRegistry registry;
    Vec4 testColors[] = {
        {0.1, 0.2, 0.3, 0.5},
        {0.3, 0.7, 0.2, 0.1},
        {0.6, 0.9, 0.1, 0.2}
    };

    for(size_t i=0; i<3; ++i){
        registry.createEntity(
            Color{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .color = testColors[i]
            }
        );
    }

    size_t i=0;
    for(auto [id, bit, cc]: registry.query<Color>()){
        EXPECT_EQ(cc.color, testColors[i]);
        ++i;
    }
}

TEST(ArchetypeView, ComplexQuery){
    EntityRegistry registry;
    Vec4 testColors[] = {
        {0.1, 0.2, 0.3, 0.5},
        {0.4, 0.3, 0.9, 1.0},
        {0.5, 0.1, 0.0, 0.5}
    };

    for(size_t i=0; i<3; ++i){
        registry.createEntity(
            Transform{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .position = zeros(),
                .rotation = unitQuat(),
                .scale = ones()
            },
            Color{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .color = testColors[i]
            }
        );
    }

    size_t i=0;
    for(auto [id, bit, tc, cc]: registry.query<Transform, Color>()){
        EXPECT_EQ(tc.position,    zeros());
        EXPECT_EQ(tc.rotation, unitQuat());
        EXPECT_EQ(   tc.scale,     ones());

        EXPECT_EQ(cc.color, testColors[i]);
        ++i;
    }
}

TEST(ArchetypeView, EmplaceOrder){
    EntityRegistry registry;
    Vec4 testColors[] = {
        {0.1, 0.2, 0.3, 0.5},
        {0.4, 0.3, 0.9, 1.0},
        {0.5, 0.1, 0.0, 0.5}
    };

    for(size_t i=0; i<3; ++i){
        if(i % 2 == 1){
            registry.createEntity(
                Transform{
                    .entity = std::numeric_limits<EntityID>::max(),
                    .isActive = true,
                    .position = zeros(),
                    .rotation = unitQuat(),
                    .scale = ones()
                },
                Color{
                    .entity = std::numeric_limits<EntityID>::max(),
                    .isActive = true,
                    .color = testColors[i]
                }
            );
        }
        else{
            registry.createEntity(
                Color{
                    .entity = std::numeric_limits<EntityID>::max(),
                    .isActive = true,
                    .color = testColors[i]
                },
                Transform{
                    .entity = std::numeric_limits<EntityID>::max(),
                    .isActive = true,
                    .position = zeros(),
                    .rotation = unitQuat(),
                    .scale = ones()
                }
            );
        }
    }

    size_t i=0;
    for(auto [id, bit, tc, cc]: registry.query<Transform, Color>()){
        EXPECT_EQ(tc.position,    zeros());
        EXPECT_EQ(tc.rotation, unitQuat());
        EXPECT_EQ(   tc.scale,     ones());

        EXPECT_EQ(cc.color, testColors[i]);
        ++i;
    }
}

TEST(ArchetypeView, AppendComponent){
    EntityRegistry registry;
    Vec4 testColors[] = {
        {0.1, 0.2, 0.3, 0.5},
        {0.4, 0.3, 0.9, 1.0},
        {0.5, 0.1, 0.0, 0.5}
    };
    auto colorTest = [&testColors](Vec4 color){
        for(size_t i=0; i<3; ++i){
            if(testColors[i] == color)
                return i;
        }
        return size_t(10000);
    };
    EntityID entities[3];

    for(size_t i=0; i<3; ++i){
        entities[i] = registry.createEntity(
            Color{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .color = testColors[i]
            },
            Transform{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .position = zeros(),
                .rotation = unitQuat(),
                .scale = ones()
            }
        );
    }

    registry.appendComponent(entities[1], Element{
        .entity = entities[1],
        .isActive = true,
        .type = ElementType::FIRE
    });

    auto testVal = 0;
    auto count = 0;
    for(auto [id, bit, tc, cc]: registry.query<Transform, Color>()){
        EXPECT_EQ(tc.position,    zeros());
        EXPECT_EQ(tc.rotation, unitQuat());
        EXPECT_EQ(   tc.scale,     ones());

        // cannot predict query order.
        testVal += colorTest(cc.color);
        ++count;
    }
    EXPECT_EQ(testVal, 0+1+2);
    EXPECT_EQ(count, 3);

    count = 0;
    for(auto [id, bit, ec]: registry.query<Element>()){
        EXPECT_EQ(ec.type, ElementType::FIRE);
        ++count;
    }
    EXPECT_EQ(count, 1);
}

TEST(ArchetypeView, RemoveComponent){
    EntityRegistry registry;
    Vec4 testColors[] = {
        {0.1, 0.2, 0.3, 0.5},
        {0.4, 0.3, 0.9, 1.0},
        {0.5, 0.1, 0.0, 0.5}
    };
    auto colorTest = [&testColors](Vec4 color){
        for(size_t i=0; i<3; ++i){
            if(testColors[i] == color)
                return i;
        }
        return size_t(10000);
    };
    EntityID entities[3];

    for(size_t i=0; i<3; ++i){
        entities[i] = registry.createEntity(
            Color{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .color = testColors[i]
            },
            Transform{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .position = zeros(),
                .rotation = unitQuat(),
                .scale = ones()
            },
            Element{
                .entity = std::numeric_limits<EntityID>::max(),
                .isActive = true,
                .type = ElementType::WIND
            }
        );
    }

    registry.removeComponent<Color>(entities[1]);

    auto testVal = 0;
    auto count = 0;
    for(auto [id, bit, tc, cc, ec]: registry.query<Transform, Color, Element>()){
        EXPECT_EQ(tc.position,    zeros());
        EXPECT_EQ(tc.rotation, unitQuat());
        EXPECT_EQ(   tc.scale,     ones());

        EXPECT_EQ(ec.type, ElementType::WIND);

        // cannot predict query order.
        testVal += colorTest(cc.color);
        ++count;
    }
    EXPECT_EQ(testVal, 0+2);
    EXPECT_EQ(count, 2);

    count = 0;
    for(auto [id, bit, tc, ec]: registry.query<Transform, Element>()){
        EXPECT_EQ(tc.position,    zeros());
        EXPECT_EQ(tc.rotation, unitQuat());
        EXPECT_EQ(   tc.scale,     ones());

        EXPECT_EQ(ec.type, ElementType::WIND);

        ++count;
    }
    EXPECT_EQ(count, 3);
}
