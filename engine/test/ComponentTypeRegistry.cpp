#include <gtest/gtest.h>
#include "ECS/ComponentTypeRegistry.hpp"

using namespace RenderToy;

namespace{
    struct TestComponent1{
        int foo;
    };
    struct TestComponent2{
        double bar;
        char baz;
    };
}

TEST(ComponentTypeRegistry, registerSingleComponent){
    ComponentTypeRegistry typeRegistry;

    typeRegistry.registerComponent<TestComponent1>();

    auto t_bit = typeRegistry.bit_of<TestComponent1>();
    auto chunkSize = typeRegistry.size_of(t_bit);
    auto offset = typeRegistry.offset_of<TestComponent1>(t_bit);

    EXPECT_EQ(t_bit, ArchetypeBit(1)<<NUM_ARCHETYPES);
    EXPECT_EQ(chunkSize, sizeof(EntityID) + sizeof(TestComponent1));
    EXPECT_EQ(offset, sizeof(EntityID));
}

TEST(ComponentTypeRegistry, registerMultipleComponent){
    ComponentTypeRegistry typeRegistry;

    typeRegistry.registerComponent<TestComponent1>();
    typeRegistry.registerComponent<TestComponent2>();

    auto t1_bit = typeRegistry.bit_of<TestComponent1>();
    auto t2_bit = typeRegistry.bit_of<TestComponent2>();
    auto t_bit = t1_bit | t2_bit;
    auto chunkSize = typeRegistry.size_of(t_bit);
    auto t1_offset = typeRegistry.offset_of<TestComponent1>(t_bit);
    auto t2_offset = typeRegistry.offset_of<TestComponent1>(t_bit);

    EXPECT_TRUE(t1_bit==ArchetypeBit(0b01)<<NUM_ARCHETYPES || t2_bit==ArchetypeBit(0b01)<<NUM_ARCHETYPES);
    EXPECT_TRUE(t1_bit==ArchetypeBit(0b10)<<NUM_ARCHETYPES || t2_bit==ArchetypeBit(0b10)<<NUM_ARCHETYPES);
    EXPECT_EQ(t_bit, ArchetypeBit(0b11)<<NUM_ARCHETYPES);
    EXPECT_EQ(chunkSize, sizeof(EntityID) + sizeof(TestComponent1) + sizeof(TestComponent2));
    EXPECT_TRUE(t1_offset==sizeof(EntityID) || t2_offset==sizeof(EntityID));
    EXPECT_TRUE(t1_offset==sizeof(TestComponent2) || t2_offset==sizeof(TestComponent1));
}

TEST(ComponentTypeRegistry, checkBuiltInComponent){
    ComponentTypeRegistry typeRegistry;

    auto t_bit = typeRegistry.bit_of<Transform>();
    auto m_bit = typeRegistry.bit_of<Mesh>();
    auto bit = t_bit | m_bit;
    auto chunkSize = typeRegistry.size_of(bit);
    auto t_offset = typeRegistry.offset_of<Transform>(bit);
    auto m_offset = typeRegistry.offset_of<Mesh>(bit);

    EXPECT_EQ(t_bit, bit_of<Transform>());
    EXPECT_EQ(m_bit, bit_of<Mesh>());
    EXPECT_EQ(bit, (bits_of<Transform, Mesh>()));
    EXPECT_EQ(chunkSize, sizeof(EntityID) + sizeof(Transform) + sizeof(Mesh));
    EXPECT_EQ(t_offset, offset_of<Transform>(bit));
    EXPECT_EQ(m_offset, offset_of<Mesh>(bit));
}

TEST(ComponentTypeRegistry, registerBuiltInComponent){
    ComponentTypeRegistry typeRegistry;

    typeRegistry.registerComponent<Transform>();
    typeRegistry.registerComponent<Mesh>();

    auto t_bit = typeRegistry.bit_of<Transform>();
    auto m_bit = typeRegistry.bit_of<Mesh>();
    auto bit = t_bit | m_bit;
    auto chunkSize = typeRegistry.size_of(bit);
    auto t_offset = typeRegistry.offset_of<Transform>(bit);
    auto m_offset = typeRegistry.offset_of<Mesh>(bit);

    EXPECT_EQ(t_bit, TRANSFORM_BIT);
    EXPECT_EQ(m_bit, MESH_BIT);
    EXPECT_EQ(bit, TRANSFORM_BIT | MESH_BIT);
    EXPECT_EQ(chunkSize, sizeof(EntityID) + sizeof(Transform) + sizeof(Mesh));
    EXPECT_EQ(t_offset, offset_of<Transform>(bit));
    EXPECT_EQ(m_offset, offset_of<Mesh>(bit));
}
