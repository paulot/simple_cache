#pragma once
#include <list>
#include <unordered_map>
#include <utility>
#include <mutex>
#include <boost/optional.hpp>
#include <glog/logging.h>


template <typename Key, typename Value>
class Cache {
  public:
    Cache(size_t capacity) : capacity_(capacity) {}

    void put(const Key& key, const Value& value);
    boost::optional<Value> get(const Key&);

  private:
    size_t capacity_;
    std::list<std::pair<Key, Value>> values_;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> keys_; 
    std::mutex globalMutex_;

    void moveToFront(typename std::list<std::pair<Key, Value>>::iterator element);
    void evict();
};

template <typename Key, typename Value>
void Cache<Key, Value>::moveToFront(typename std::list<std::pair<Key, Value>>::iterator element) {
  if (element != this->values_.begin()) {
    this->values_.splice(this->values_.begin(), this->values_, element, std::next(element));
  }
}

template <typename Key, typename Value>
void Cache<Key, Value>::evict() {
  // Evict the item on the back
  auto [key, value] = this->values_.back();
  VLOG(2) << "evict::Evicting kv " << key << ' ' << value;
  this->values_.pop_back();
  this->keys_.erase(key);
}

template <typename Key, typename Value>
void Cache<Key, Value>::put(const Key& key, const Value& value) {
  VLOG(2) << "put::Putting k v " << key << ' ' << value;

  // Global lock
  std::lock_guard<std::mutex> guard(this->globalMutex_);
  const auto& it = this->keys_.find(key);
  if (it == this->keys_.end()) {
    VLOG(2) << "put::Item not found";
    // Did not find the value, add it to the list and to the map to the front
    if (this->keys_.size() >= this->capacity_) {
      VLOG(2) << "put::Evicting, capacity, size " << this->keys_.size() << ' ' << this->capacity_;
      evict();
    }

    this->values_.emplace_front(key, value);
    this->keys_[key] = this->values_.begin();
  } else {
    // Found the value, no need to evict. Update the value and move to front
    VLOG(2) << "put::Item found moving to front";
    (*this->keys_[key]).second = value;
    moveToFront(this->keys_[key]);
    this->keys_[key] = this->values_.begin();
  }
}

template <typename Key, typename Value>
boost::optional<Value> Cache<Key, Value>::get(const Key& key) {
  VLOG(2) << "get::getting key " << key;

  // Global lock
  std::lock_guard<std::mutex> guard(this->globalMutex_);
  // Access the item and move to front
  const auto& it = this->keys_.find(key);
  if (it == this->keys_.end()) {
    VLOG(2) << "get::Item not found";
    // Not found return none
    return boost::none; 
  }

  moveToFront(this->keys_[key]);
  this->keys_[key] = this->values_.begin();
  VLOG(2) << "get:;Item found k v " << this->values_.front().first << ' ' << this->values_.front().second;
  return this->values_.front().second;
}
