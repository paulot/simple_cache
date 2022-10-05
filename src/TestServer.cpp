#include <unordered_map>
#include <boost/optional/optional_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <gtest/gtest.h>

#include "Cache.hpp"
#include "Client.hpp"
#include "TcpServer.hpp"

namespace {

class TestServer : public ::testing::Test {
protected:
  TestServer() : 
    capacity_(10),
    concurrency_(10),
    port_(100),
    serverIoService_(concurrency_),
    clientIoService_(),
    server_(nullptr),
    client_(nullptr) {}

  virtual void SetUp() {
    // Spin up the server
    auto cache = std::make_shared<Cache<std::string, std::string>>(capacity_);
    server_ = std::make_unique<AsyncTcpServer>(serverIoService_, port_, std::move(cache));
    runner_ = std::thread([&]() {
      serverIoService_.run();
    });

    // Spin up the client
    client_ = std::make_unique<cache::CacheClient>(clientIoService_, "127.0.0.1", port_);
    client_->connect();
  }

  virtual void TearDown() {
    serverIoService_.stop();
    runner_.join();
  }

  uint32_t capacity_;
  uint32_t concurrency_;
  uint32_t port_;
  boost::asio::io_service serverIoService_;
  boost::asio::io_service clientIoService_;
  std::unique_ptr<AsyncTcpServer> server_;
  std::unique_ptr<cache::CacheClient> client_;
  std::thread runner_;
};

TEST_F(TestServer, TestGetPut) {
  auto bla = client_->get("1");
  EXPECT_EQ(client_->get("1"), boost::none);

  client_->put("1", "foo");
  client_->put("2", "bar");
  client_->put("3", "baz");
  client_->put("4", "bla");
  client_->put("5", "blu");

  EXPECT_EQ(*client_->get("1"), "foo");
  EXPECT_EQ(*client_->get("2"), "bar");
  EXPECT_EQ(*client_->get("3"), "baz");
  EXPECT_EQ(*client_->get("4"), "bla");
  EXPECT_EQ(*client_->get("5"), "blu");
}

TEST_F(TestServer, TestGetPut100k) {
  uint32_t nThreads = 50;
  uint32_t nOperations = 2000;
  boost::asio::thread_pool pool(nThreads);

  
  for (int32_t i = 0; i < nThreads; ++i) {
    boost::asio::post(pool, [&, i, nOperations](){
      auto client = std::make_unique<cache::CacheClient>(clientIoService_, "127.0.0.1", port_);
      client->connect();
      
      for (int32_t j = 0; j < nOperations; ++j) {
        std::string key = std::to_string((i << 15) + j);
        client->put(key, "foo");
        client->get(key);
      }
    });
  }
  pool.join();

  int32_t numberFound = 0;
  auto client = std::make_unique<cache::CacheClient>(clientIoService_, "127.0.0.1", port_);
  client->connect();
  for (int32_t i = 0; i < nThreads; ++i) {
    for (int32_t j = nOperations - 5; j < nOperations; ++j) {
      std::string key = std::to_string((i << 15) + j);
      if (client->get(key) != boost::none) {
        numberFound++;
      }
    }
  }
  EXPECT_EQ(numberFound, capacity_);
}

} // namespace
