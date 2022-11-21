#include <array>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>
#include <boost/optional.hpp>
#include <boost/thread.hpp>



template <typename Key, typename Value, size_t HashSpace>
class ConcurrentHashMap {
public:
  ConcurrentHashMap() {}

  void erase(const Key& key);
  void insert(const Key& key, const Value& value);
  void insertAndEvict(const Key& keyToEvict, const Key& keyToInsert, const Value& valueToInsert);
  boost::optional<Value> at(const Key& key);


private:
  mutable std::array<boost::shared_mutex, HashSpace> locks_;
  std::array<std::unordered_map<Key, Value>, HashSpace> container_;

  size_t getBucket(const Key& key) const;
  void erase(const Key& key, size_t bucket);
};


template <typename Key, typename Value, size_t HashSpace>
size_t ConcurrentHashMap<Key, Value, HashSpace>::getBucket(const Key& key) const {
  std::hash<Key> func;
  return func(key) % HashSpace;
}

template <typename Key, typename Value, size_t HashSpace>
boost::optional<Value> ConcurrentHashMap<Key, Value, HashSpace>::at(const Key& key) {
  auto bucket = getBucket(key);
  if (container_[bucket].size() == 0) {
    return boost::none;
  }

  boost::shared_lock<boost::shared_mutex> lock(locks_[bucket]);
  auto it = container_[bucket].find(key);

  if (it == container_[bucket].end()) {
    return boost::none;
  } else {
    return (*it).second;
  }
}

template <typename Key, typename Value, size_t HashSpace>
void ConcurrentHashMap<Key, Value, HashSpace>::erase(const Key& key) {
  auto bucket = getBucket(key);
  erase(key, bucket);
}

template <typename Key, typename Value, size_t HashSpace>
void ConcurrentHashMap<Key, Value, HashSpace>::erase(const Key& key, size_t bucket) {
  if (container_[bucket].size() == 0) {
    // Nothing to do
    return;
  }

  // Try to find the key with just a read lock
  boost::upgrade_lock<boost::shared_mutex> lock(locks_[bucket]);
  auto it = container_[bucket].find(key);

  if (it == container_[bucket].end()) {
    // Nothing to erase
    return;
  } else {
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    container_[bucket].erase(it);
    return;
  }
}

template <typename Key, typename Value, size_t HashSpace>
void ConcurrentHashMap<Key, Value, HashSpace>::insert(const Key& key, const Value& value) {
  auto bucket = getBucket(key);

  boost::unique_lock<boost::shared_mutex> lock(locks_[bucket]);
  container_[bucket][key] = value;
}

template <typename Key, typename Value, size_t HashSpace>
void ConcurrentHashMap<Key, Value, HashSpace>::insertAndEvict(
    const Key& keyToEvict, const Key& keyToInsert, const Value& valueToInsert) {
  auto bucketToInsert = getBucket(keyToInsert);
  auto bucketToEvict = getBucket(keyToEvict);

  {
    // Get unique lock for that bucket
    boost::unique_lock<boost::shared_mutex> lock(locks_[bucketToInsert]);
    container_[bucketToInsert][keyToInsert] = valueToInsert;

    if (bucketToInsert == bucketToEvict) {
      container_[bucketToInsert].erase(keyToEvict);
      return;
    }
  }

  {
    // Still need to evict
    erase(keyToEvict, bucketToEvict);
  }
}

/*
template <typename Key, typename Value, size_t HashSpace>
bool ConcurrentHashMap<Key, Value, HashSpace>::isPresent(const Key& key) const {
  auto bucket = getBucket(key);
  if (container_[bucket].size() == 0) {
    return false;
  }
  return false;
}
*/



















// template <typename Key, typename Value>
// class ConcurrentMap {
// private:
//   struct LockedValue {
//     Value val_;
//     std::mutex valMutex_;
// 
//     const LockedValue& operator=(const LockedValue& other) {
//       std::lock_guard<std::mutex> guard(valMutex_);
//       val_ = other;
//     }
//   };
// 
// public:
//   ConcurrentMap(size_t hashSpace) : 
// 
//   bool erase(const Key& key);
//   bool insert(const Key& key, const Value& value);
// 
// 
// private:
//   std::vector<std::mutex> locks_;
//   std::unordered_map<Key, LockedValue> container_;
//   std::mutex globalLock_;
//   
// 
// 
//   // Types of locks:
//   //  Global write lock -> on erase, lock the whole container
//   //  Record lock -> on modifying a value we need to lock that value alone
//   //  Reads -> no lock needed (https://en.cppreference.com/w/cpp/container#Thread_safety) 
// 
//   // What we need in the cache: erase, (find) isMember, end, size, []
//   // we can re-write to have erase and insert only
// };
// 
// template <typename Key, typename Value>
// bool ConcurrentMap<Key, Value>::erase(const Key& key) {
//   /**
//   * References and iterators to the erased elements are invalidated. Other
//   * iterators and references are not invalidated.
//   *
//   * The iterator pos must be valid and dereferenceable. Thus the end()
//   * iterator (which is valid, but is not dereferenceable) cannot be used as a
//   * value for pos.
//   *
//   * The order of the elements that are not erased is preserved. (This makes it
//   * possible to erase individual elements while iterating through the container.)
//   */
// 
//   auto &it = container_.find(key);
// 
//   if (it == container_.end()) {
//     return false;
//   }
// 
//   std::lock_guard<std::mutex> guard(it->valMutex_);
//   // At this point the element at 'it' could have been deleted by another
//   // thread. No problem, erase should not throw in such case.
//   container_.erase(it);
//   return true;
// }
// 
// template <typename Key, typename Value>
// bool ConcurrentMap<Key, Value>::insert(const Key& key, const Value& value) {
// 
// }
// 

