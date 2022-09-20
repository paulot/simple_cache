#include <list>
#include <unordered_map>

#include "Optional.hpp"

template <typename Key, typename Value>
class Cache {
public:
    Cache(size_t size) : size_(size) {}

    void put(const Key& key, const Value& value);

    Optional<Value> get(const Key&) const;

private:
    size_t size_;
    std::list<Value> values_;
    std::unordered_map<Key, std::list<Value>::iterator> keys_; 
};
