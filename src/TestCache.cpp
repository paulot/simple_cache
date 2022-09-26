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

TEST(LRUCacheTest, TestConcurrentPut) {
  auto cache = Cache<int32_t, std::string>(5);
  boost::asio::thread_pool pool(5);

  // Keep the set of insertions from each thread, no need for locks
  // as each thread has it's own vector.
  std::unordered_map<std::thread::id, std::vector<int32_t>> insertions;
  for (int32_t i = 0; i < 100000; ++i) {
    boost::asio::post(pool, [&cache, &insertions, i](){
      cache.put(i, "foo");
      cache.get(i);

      std::thread::id tId = std::this_thread::get_id();
      insertions[tId].push_back(i);
    });
  }
  pool.join();
  
  int32_t numberFound = 0;
  for (const auto& [id, vec] : insertions) {
    auto size = vec.size();
    for (int32_t i = 0; i < 5; ++i) {
      if (cache.get(vec[size - 1 - i]) != boost::none) {
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
