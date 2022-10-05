#include "Cache.hpp"

#include <unordered_map>
#include <thread>
#include <boost/optional/optional_io.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <gtest/gtest.h>

TEST(LRUCacheTest, TestBasic) {
  auto cache = Cache<int32_t, std::string>(5);

  EXPECT_EQ(cache.get(1), boost::none);

  cache.put(1, "foo");
  cache.put(2, "bar");
  cache.put(3, "baz");
  cache.put(4, "bla");
  cache.put(5, "blu");

  EXPECT_EQ(*cache.get(1), "foo");
  EXPECT_EQ(*cache.get(2), "bar");
  EXPECT_EQ(*cache.get(3), "baz");
  EXPECT_EQ(*cache.get(4), "bla");
  EXPECT_EQ(*cache.get(5), "blu");

  // Any new addition should now be evicted
  cache.put(10, "foobar");
  EXPECT_EQ(*cache.get(10), "foobar");
  EXPECT_EQ(cache.get(1), boost::none);

  cache.put(11, "blabla");
  EXPECT_EQ(*cache.get(11), "blabla");
  EXPECT_EQ(cache.get(2), boost::none);

  cache.put(12, "foofoo");
  EXPECT_EQ(*cache.get(12), "foofoo");
  EXPECT_EQ(cache.get(3), boost::none);
}

TEST(LRUCacheTest, TestPutEvict) {
  auto cache = Cache<int32_t, std::string>(5);
  std::string items[5] = {"foo", "bar", "baz", "bla", "blu"};

  // Seed the cache
  for (int32_t i = 0; i < 5; ++i) {
    cache.put(i, items[i]);
  }

  for (int32_t i = 5; i < 100; ++i) {
    cache.put(i, items[i % 5]);
    EXPECT_EQ(cache.get(i - 5), boost::none);

    for (int32_t j = 4; j > 0; --j) {
      EXPECT_EQ(cache.get(i - j), items[(i - j) % 5]);
    }
    EXPECT_EQ(cache.get(i), items[i % 5]);
  }
}

TEST(LRUCacheTest, TestConcurrentPut100k) {
  uint32_t capacity = 5;
  uint32_t nThreads = 50;
  uint32_t nOperations = 2000;
  auto cache = Cache<int32_t, std::string>(capacity);
  boost::asio::thread_pool pool(nThreads);

  for (int32_t i = 0; i < nThreads; ++i) {
    boost::asio::post(pool, [i, nOperations, &cache](){
      for (int32_t j = 0; j < nOperations; ++j) {
        int32_t key = (i << 15) + j;
        cache.put(key, "foo");
        cache.get(key);
      }
    });
  }
  pool.join();
  
  int32_t numberFound = 0;
  for (int32_t i = 0; i < nThreads; ++i) {
    for (int32_t j = nOperations - 5; j < nOperations; ++j) {
      int32_t key = (i << 15) + j;
      if (cache.get(key) != boost::none) {
        numberFound++;
      }
    }
  }

  EXPECT_EQ(numberFound, 5);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
