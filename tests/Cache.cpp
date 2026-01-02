//
// Created by TheDaChicken on 7/24/2025.
//

#include <gtest/gtest.h>
#include "../src/gui/utils/Cache.h"

TEST(Cache, FilledUp)
{
	Cache<std::string, int> cache(3);
	cache.TryInsert("one",1, std::make_shared<int>(1));
	cache.TryInsert("two", 1, std::make_shared<int>(2));
	cache.TryInsert("three", 1, std::make_shared<int>(3));
	EXPECT_EQ(*cache.Get("one"), 1);
	EXPECT_EQ(*cache.Get("two"), 2);
	EXPECT_EQ(*cache.Get("three"), 3);
	EXPECT_EQ(cache.Get("four"), nullptr);
	cache.TryInsert("four", 1, std::make_shared<int>(4));
	EXPECT_EQ(cache.Get("one"), nullptr);
}

TEST(Cache, Scope)
{
	Cache<std::string, int> cache(3);
	cache.TryInsert("one", 1, std::make_shared<int>(1));
	cache.TryInsert("two", 1, std::make_shared<int>(2));
	cache.TryInsert("three", 1, std::make_shared<int>(3));
	const auto ptr = cache.Get("one");
	EXPECT_EQ(*ptr, 1);

	cache.TryInsert("four", 1, std::make_shared<int>(4));
	EXPECT_EQ(*ptr, 1);
	EXPECT_EQ(cache.Get("two"), nullptr);
}

TEST(Cache, Duplicate)
{
	Cache<std::string, int> cache(3);
	cache.TryInsert("one", 1, std::make_shared<int>(1));
	cache.TryInsert("two", 1, std::make_shared<int>(2));
	cache.TryInsert("three", 1, std::make_shared<int>(3));
}