#include "ConcurrentHashMap.hpp"

#include <unordered_map>
#include <thread>
#include <boost/optional/optional_io.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <gtest/gtest.h>

TEST(ConcurrentHashMapTest, TestBasic) {
  auto hm = ConcurrentHashMap<int, std::string, 5>();

  EXPECT_EQ(hm.at(1), boost::none);

  hm.insert(1, "foo");
  hm.insert(2, "bar");
  hm.insert(3, "baz");
  hm.insert(4, "bla");
  hm.insert(5, "blu");
  hm.insert(6, "bla");
  hm.insert(7, "blu");
  hm.insert(8, "blu");
  hm.insert(9, "bla");
  hm.insert(10, "blu");

  EXPECT_EQ(*hm.at(1), "foo");
  EXPECT_EQ(*hm.at(2), "bar");
  EXPECT_EQ(*hm.at(3), "baz");
  EXPECT_EQ(*hm.at(4), "bla");
  EXPECT_EQ(*hm.at(5), "blu");
  EXPECT_EQ(*hm.at(6), "bla");
  EXPECT_EQ(*hm.at(7), "blu");
  EXPECT_EQ(*hm.at(8), "blu");
  EXPECT_EQ(*hm.at(9), "bla");
  EXPECT_EQ(*hm.at(10), "blu");
}

TEST(ConcurrentHashMapTest, TestConcurrentPut100k) {
  constexpr uint32_t hashSpace = 5;
  uint32_t nThreads = 50;
  // uint32_t nThreads = 5;
  uint32_t nOperations = 2000;
  // uint32_t nOperations = 50;
  auto hm = ConcurrentHashMap<int32_t, std::string, hashSpace>();
  boost::asio::thread_pool pool(nThreads);

  for (int32_t i = 0; i < nThreads; ++i) {
    // Add
    boost::asio::post(pool, [i, nOperations, &hm](){
      for (int32_t j = 0; j < nOperations; ++j) {
        int32_t key = (i << 15) + j;
        hm.insert(key, "foo");
      }
    });

    // Access
    boost::asio::post(pool, [i, nOperations, &hm](){
      for (int32_t j = 0; j < nOperations; ++j) {
        int32_t key = (i << 15) + j;
        hm.at(key);
      }
    });

    // Erase keys 0 -> nOperations / 4
    boost::asio::post(pool, [i, nOperations, &hm](){
      for (int32_t j = 0; j < nOperations / 4; ++j) {
        int32_t key = (i << 15) + j;
        hm.erase(key);
      }
    });

    // Erase keys nOperations / 4 -> nOperations / 2
    // Insert keys - -> nOperations/4
    boost::asio::post(pool, [i, nOperations, &hm](){
      for (int32_t j = 0; j < nOperations / 4; ++j) {
        int32_t keyToInsert = (i << 15) + j;
        int32_t keyToErase = keyToInsert + (nOperations / 4);
        hm.insertAndEvict(keyToErase, keyToInsert, "bar");
      }
    });
  }
  pool.join();
  
  int32_t numberFound = 0;
  for (int32_t i = 0; i < nThreads; ++i) {
    for (int32_t j = nOperations / 2; j < nOperations; ++j) {
      int32_t key = (i << 15) + j;
      if (hm.at(key) != boost::none) {
        numberFound++;
      }
    }
  }

  EXPECT_EQ(numberFound, (nOperations / 2) * nThreads);
}
