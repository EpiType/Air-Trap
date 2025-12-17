#include <gtest/gtest.h>
#include "RType/ECS/SparseArray.hpp"
#include "RType/ECS/Entity.hpp"

using namespace rtp::ecs;

struct DummyComponent {
    int value{0};
};

TEST(SparseArrayTest, EraseNonExistingIsNoop) {
    SparseArray<DummyComponent> arr;
    Entity e1{1, 0};
    EXPECT_NO_THROW(arr.erase(e1));
    EXPECT_TRUE(arr.empty());
    EXPECT_EQ(arr.size(), 0u);
}

TEST(SparseArrayTest, OverwriteKeepsSizeOne) {
    SparseArray<DummyComponent> arr;
    Entity e{0, 0};
    arr.emplace(e, DummyComponent{10});
    arr.emplace(e, DummyComponent{20});
    EXPECT_EQ(arr.size(), 1u);
    EXPECT_EQ(arr[e].value, 20);
}

TEST(SparseArrayTest, EraseSwapBackMaintainsMapping) {
    SparseArray<DummyComponent> arr;
    Entity e1{0, 0};
    Entity e2{1, 0};
    Entity e3{2, 0};

    arr.emplace(e1, DummyComponent{1});
    arr.emplace(e2, DummyComponent{2});
    arr.emplace(e3, DummyComponent{3});

    arr.erase(e2);

    EXPECT_EQ(arr.size(), 2u);
    EXPECT_FALSE(arr.has(e2));

    ASSERT_TRUE(arr.has(e1));
    ASSERT_TRUE(arr.has(e3));

    EXPECT_EQ(arr[e1].value, 1);
    EXPECT_EQ(arr[e3].value, 3);
}

TEST(SparseArrayTest, EntitiesAndDataSizesStayConsistent) {
    SparseArray<DummyComponent> arr;
    Entity e1{0, 0};
    Entity e2{1, 0};

    arr.emplace(e1, DummyComponent{5});
    arr.emplace(e2, DummyComponent{6});

    EXPECT_EQ(arr.getEntities().size(), arr.getData().size());

    arr.erase(e1);

    EXPECT_EQ(arr.getEntities().size(), arr.getData().size());
}
