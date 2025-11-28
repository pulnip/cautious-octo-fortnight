#include <gtest/gtest.h>
#include "Resource/ResourceManager.hpp"

using RenderToy::ResourceManager;

namespace{
    struct MockResource{ int i; };
    struct MockResourceRequest{};
    struct MockResourceKey{
        auto operator<=>(const MockResourceKey&) const = default;
    };
    struct MockResourceKeyHash{
        inline size_t operator()(const MockResourceKey& key) const noexcept{
            return 0;
        }
    };
}

template<>
struct RenderToy::ResourceTraits<MockResource>{
    using Request = MockResourceRequest;
    using Key     = MockResourceKey;
    using KeyHash = MockResourceKeyHash;

    inline static Key makeKey(const Request& request){
        return Key{};
    }

    inline static MockResource load(const Request& request){
        return MockResource{};
    }
};

TEST(ResourceManager, Trivial){
    ResourceManager<MockResource> manager;
    MockResourceRequest request;

    auto handle1 = manager.getOrLoad(request);
    auto handle2 = manager.getOrLoad(request);

    EXPECT_EQ(handle1, handle2);
}