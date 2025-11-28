#include <optional>
#include <vector>
#include <gtest/gtest.h>
#include "dynamic_vector.hpp"

using RenderToy::dynamic_vector;

TEST(dynamic_vector, ZeroChunkBehaviors){
    dynamic_vector vec(0);
    EXPECT_EQ(vec.size(), 0u);
    EXPECT_EQ(vec.capacity(), 0u);
    vec.clear();
    EXPECT_EQ(vec.size(), 0u);

    EXPECT_DEATH(vec[0], "");
}

TEST(dynamic_vector, ReserveAndResize){
    dynamic_vector vec(sizeof(int));
    EXPECT_EQ(vec.size(), 0u);
    EXPECT_EQ(vec.capacity(), 0u);

    vec.reserve(4);
    EXPECT_GE(vec.capacity(), 4u);

    vec.resize(4);
    EXPECT_EQ(vec.size(), 4u);
    EXPECT_GE(vec.capacity(), 4u);
}

TEST(dynamic_vector, EmplaceAndAccess){
    dynamic_vector vec(sizeof(int));
    vec.emplace(10);
    vec.emplace(20);
    EXPECT_EQ(vec.size(), 2u);

    int* p0 = static_cast<int*>(vec[0]);
    int* p1 = static_cast<int*>(vec[1]);
    EXPECT_EQ(*p0, 10);
    EXPECT_EQ(*p1, 20);
}

TEST(dynamic_vector, SwapRemoveShrinksAndMoves){
    dynamic_vector vec(sizeof(int));
    vec.emplace(1);
    vec.emplace(2);
    vec.emplace(3);
    ASSERT_EQ(vec.size(), 3u);

    vec.swap_remove(1);
    EXPECT_EQ(vec.size(), 2u);

    int* p1 = static_cast<int*>(vec[1]);
    EXPECT_EQ(*p1, 3);
}

TEST(dynamic_vector, ClearResetsSize){
    dynamic_vector vec(sizeof(int));
    vec.emplace(5);
    vec.emplace(6);
    EXPECT_EQ(vec.size(), 2u);

    vec.clear();
    EXPECT_EQ(vec.size(), 0u);
    EXPECT_GE(vec.capacity(), 2u);
}

TEST(dynamic_vector, Iterator){
    dynamic_vector vec(sizeof(int));
    vec.emplace(7);
    vec.emplace(8);
    vec.emplace(9);
    ASSERT_EQ(vec.size(), 3u);

    std::vector<int> results;
    for (auto it = vec.begin(); it != vec.end(); ++it){
        void* raw = *it;
        int value = *static_cast<int*>(raw);
        results.push_back(value);
    }

    std::vector<int> expected = {7, 8, 9};
    EXPECT_EQ(results, expected);
}


// Composite chunk (int, float, char[4]) tests
TEST(dynamic_vector, CompositeChunkSingleElement){
    size_t chunkSize = sizeof(int) + sizeof(float) + 4 * sizeof(char);
    dynamic_vector vec(chunkSize);
    vec.emplace(42, 2.718f, 'h', 'e', 'l', 'o');
    ASSERT_EQ(vec.size(), 1u);

    void* raw = vec[0];
    int i = *static_cast<int*>(raw);
    float f = *reinterpret_cast<float*>(static_cast<char*>(raw) + sizeof(int));
    char* chars = static_cast<char*>(raw) + sizeof(int) + sizeof(float);

    EXPECT_EQ(i, 42);
    EXPECT_FLOAT_EQ(f, 2.718f);
    EXPECT_EQ(chars[0], 'h');
    EXPECT_EQ(chars[1], 'e');
    EXPECT_EQ(chars[2], 'l');
    EXPECT_EQ(chars[3], 'o');
}

TEST(dynamic_vector, CompositeChunkMultipleElements){
    size_t chunkSize = sizeof(int) + sizeof(float) + 4 * sizeof(char);
    dynamic_vector vec(chunkSize);
    vec.reserve(3);
    vec.emplace(1, 1.1f, 'a', 'b', 'c', 'd');
    vec.emplace(2, 2.2f, 'e', 'f', 'g', 'h');
    vec.emplace(3, 3.3f, 'i', 'j', 'k', 'l');
    ASSERT_EQ(vec.size(), 3u);

    for(size_t idx = 0; idx < vec.size(); ++idx){
        void* raw = vec[idx];
        int expected_i = static_cast<int>(idx) + 1;
        float expected_f = expected_i * 1.1f;
        EXPECT_EQ(*static_cast<int*>(raw), expected_i);
        EXPECT_FLOAT_EQ(
            *reinterpret_cast<float*>(static_cast<char*>(raw) + sizeof(int)),
            expected_f
        );
        char* chars = static_cast<char*>(raw) + sizeof(int) + sizeof(float);
        EXPECT_EQ(chars[0], static_cast<char>('a' + idx * 4));
        EXPECT_EQ(chars[1], static_cast<char>('b' + idx * 4));
        EXPECT_EQ(chars[2], static_cast<char>('c' + idx * 4));
        EXPECT_EQ(chars[3], static_cast<char>('d' + idx * 4));
    }
}

TEST(dynamic_vector, CompositeChunkSwapRemove){
    size_t chunkSize = sizeof(int) + sizeof(float) + 4 * sizeof(char);
    dynamic_vector vec(chunkSize);
    vec.emplace(10, 10.1f, 'x', 'y', 'z', 'w');
    vec.emplace(20, 20.2f, 'u', 'v', 'w', 'x');
    vec.emplace(30, 30.3f, 'q', 'r', 's', 't');
    ASSERT_EQ(vec.size(), 3u);

    vec.swap_remove(1);
    ASSERT_EQ(vec.size(), 2u);

    void* raw = vec[1];
    EXPECT_EQ(*static_cast<int*>(raw), 30);
    EXPECT_FLOAT_EQ(
        *reinterpret_cast<float*>(static_cast<char*>(raw) + sizeof(int)),
        30.3f
    );
    char* chars = static_cast<char*>(raw) + sizeof(int) + sizeof(float);
    EXPECT_EQ(chars[0], 'q');
    EXPECT_EQ(chars[1], 'r');
    EXPECT_EQ(chars[2], 's');
    EXPECT_EQ(chars[3], 't');
}

TEST(dynamic_vector, PointerEmplaceAllNonNull){
    size_t chunkSize = sizeof(int) * 2;
    dynamic_vector vec(chunkSize);
    int a = 100, b = 200;
    vec.emplace(&a, &b);
    ASSERT_EQ(vec.size(), 1u);

    int* data = static_cast<int*>(vec[0]);
    EXPECT_EQ(data[0], a);
    EXPECT_EQ(data[1], b);
}

TEST(dynamic_vector, PointerEmplaceSkipFirst){
    size_t chunkSize = sizeof(int) * 2;
    dynamic_vector vec(chunkSize);
    int a = 100, b = 200;
    vec.emplace(&a, nullptr, &b, nullptr);
    ASSERT_EQ(vec.size(), 1u);

    int* data = static_cast<int*>(vec[0]);
    EXPECT_EQ(data[0], a);
    EXPECT_EQ(data[1], b);
}

TEST(dynamic_vector, OptionalEmplaceAllPresent){
    size_t chunkSize = sizeof(int) * 2;
    dynamic_vector vec(chunkSize);
    std::optional<int> a = 300, b = 400;
    vec.emplace(a, b);
    ASSERT_EQ(vec.size(), 1u);

    int* data = static_cast<int*>(vec[0]);
    EXPECT_EQ(data[0], *a);
    EXPECT_EQ(data[1], *b);
}

TEST(dynamic_vector, OptionalEmplaceSkipSecond){
    size_t chunkSize = sizeof(int) * 2;
    dynamic_vector vec(chunkSize);
    std::optional<int> a = 300, b = 400, x = std::nullopt;
    vec.emplace(x, a, x, b);
    ASSERT_EQ(vec.size(), 1u);

    int* data = static_cast<int*>(vec[0]);
    EXPECT_EQ(data[0], *a);
}