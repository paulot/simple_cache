#include "ConcurrentList.hpp"

#include <typeinfo>
#include <thread>
#include <boost/optional/optional_io.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <gtest/gtest.h>

TEST(ConcurrentListTest, TestIsInList) {
  ConcurrentList<int> list;
  auto nodePtr1 = ConcurrentList<int>::newListNode(1);
  auto nodePtr2 = ConcurrentList<int>::newListNode(2);
  auto nodePtr3 = ConcurrentList<int>::newListNode(3);
  
  list.pushFront(nodePtr1);
  list.pushFront(nodePtr2);
  list.pushFront(nodePtr3);

  EXPECT_EQ(nodePtr1->isInList(), true);
  EXPECT_EQ(nodePtr2->isInList(), true);
  EXPECT_EQ(nodePtr3->isInList(), true);
}

TEST(ConcurrentListTest, TestBasic) {
  ConcurrentList<int> list;
  auto nodePtr1 = ConcurrentList<int>::newListNode(1);
  auto nodePtr2 = ConcurrentList<int>::newListNode(2);
  auto nodePtr3 = ConcurrentList<int>::newListNode(3);
  auto nodePtr4 = ConcurrentList<int>::newListNode(4);
  auto nodePtr5 = ConcurrentList<int>::newListNode(5);
  
  list.pushFront(nodePtr1);
  list.pushFront(nodePtr2);
  list.pushFront(nodePtr3);

  auto pop1 = list.popBack();
  auto pop2 = list.popBack();

  EXPECT_EQ(pop1, nodePtr1);
  EXPECT_EQ(pop2, nodePtr2);

  auto vectorFwd = list.toVector();
  auto vectorRev = list.toVectorReverse();
  EXPECT_EQ(vectorFwd.size(), 1);
  EXPECT_EQ(vectorFwd.size(), vectorRev.size());

  list.pushFront(nodePtr4);
  list.pushFront(nodePtr5);
  list.moveToFront(nodePtr3);

  vectorFwd = list.toVector();
  vectorRev = list.toVectorReverse();
  std::vector<int> expectFwd({3, 5, 4});
  std::vector<int> expectRev({4, 5, 3});
  EXPECT_EQ(vectorFwd, expectFwd);
  EXPECT_EQ(vectorRev, expectRev);
  EXPECT_EQ(vectorFwd.size(), vectorRev.size());

  auto pop4 = list.popBack();
  auto pop5 = list.popBack();
  auto pop3 = list.popBack();

  EXPECT_EQ(pop4, nodePtr4);
  EXPECT_EQ(pop5, nodePtr5);
  EXPECT_EQ(pop3, nodePtr3);

  vectorFwd = list.toVector();
  vectorRev = list.toVectorReverse();
  EXPECT_EQ(vectorFwd.size(), 0);
  EXPECT_EQ(vectorFwd.size(), vectorRev.size());

  delete nodePtr1;
  delete nodePtr2;
  delete nodePtr3;
  delete nodePtr4;
  delete nodePtr5;
}

TEST(ConcurrentListTest, TestPopBackEmpty) {
  ConcurrentList<int> list;
  auto ptr = list.popBack();
  EXPECT_EQ(ptr, nullptr);
}

TEST(ConcurrentListTest, TestPushFrontSame) {
  ConcurrentList<int> list;
  auto nodePtr1 = ConcurrentList<int>::newListNode(1);

  for (int i = 0; i < 10; ++i) {
    list.pushFront(nodePtr1);
  }

  for (int i = 0; i < 10; ++i) {
    list.moveToFront(nodePtr1);
  }

  auto ptr = list.popBack();
  EXPECT_EQ(ptr, nodePtr1);
  EXPECT_EQ(nullptr, list.popBack());

  delete nodePtr1;
}

TEST(ConcurrentListTest, TestConcurrentOperations100k) {
  /*
  uint32_t nThreads = 50;
  uint32_t nOperations = 2000;
  uint32_t nItems = 2000;
  */

  uint32_t nThreads = 10;
  uint32_t nOperations = 100;
  uint32_t nItems = 100;
  ConcurrentList<int> list;
  std::vector items{ConcurrentList<int>::newListNode(0)};

  for (uint32_t i = 1; i < nItems; ++i) {
    items.emplace_back(ConcurrentList<int>::newListNode(i));
    list.pushFront(items[i]);
  }

  boost::asio::thread_pool pool(nThreads);

  for (int32_t i = 0; i < nThreads; ++i) {

    boost::asio::post(pool, [i, nItems, nThreads, &items, &list](){
      // Insert the items for this thread
      /*
      int start = (nItems / nThreads) * i;
      int end = (nItems / nThreads) * (i + 1);
      for (int j = start; j < end; ++j) {
        list.pushFront(items[j]);
      }

      for (int j = start; j < end; ++j) {
        list.moveToFront(items[j]);
      }
      */
    });

    boost::asio::post(pool, [nItems, nThreads, nOperations, &list](){
      // Remove items from here
      for (int j = 0; j < nOperations; ++j) {
        auto back = list.popBack();
        if (back != nullptr) {
          //list.pushFront(back);
        }
      }
    });
  }
  pool.join();

  auto vec = list.toVector();
  EXPECT_EQ(vec.size(), nItems);
}
