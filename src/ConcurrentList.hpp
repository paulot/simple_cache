#include <list>
#include <boost/thread.hpp>
#include <glog/logging.h>



template <typename Value>
class ConcurrentList {
private:
  struct ListNode {
    ListNode() : prev_(OutOfListMarker), next_(nullptr) {}
    explicit ListNode(const Value& value) :
      value_(value), prev_(OutOfListMarker), next_(nullptr) {}
    
    bool isInList() const {
      return prev_ != OutOfListMarker;
    }

    Value value_;
    ListNode* prev_;
    ListNode* next_;
    mutable boost::shared_mutex itemMutex_;
  };

  static ListNode* const OutOfListMarker;

public:
  ConcurrentList();
  ConcurrentList(const std::initializer_list<ListNode*>& list);
  ~ConcurrentList();

  void clear();
  void pushFront(ListNode* node);
  void moveToFront(ListNode* node);
  ListNode* popBack();
  std::vector<Value> toVector();
  std::vector<Value> toVectorReverse();

  static ListNode* newListNode(const Value& value) {
    return new ListNode(value);
  }

private:
  ListNode head_;
  ListNode tail_;
  std::atomic<uint32_t> size_;

  void unsafeDelink(ListNode* node);
  void unsafePushFront(ListNode* node);
};

template <typename Value>
ConcurrentList<Value>::ConcurrentList() : head_(), tail_(), size_(0) {
  head_.prev_ = nullptr;
  head_.next_ = &tail_;
  tail_.prev_ = &head_;
}

template <typename Value>
ConcurrentList<Value>::ConcurrentList(const std::initializer_list<ListNode*>& list) :
    head_(), tail_(), size_(0) {
  head_.prev_ = nullptr;
  head_.next_ = &tail_;
  tail_.prev_ = &head_;

  for (auto it : list) {
    pushFront(*it);
  }
}

template <typename Value>
ConcurrentList<Value>::~ConcurrentList() {
  clear();
}

template <typename Value>
typename ConcurrentList<Value>::ListNode* const
ConcurrentList<Value>::OutOfListMarker = (ListNode*)-1;

template <typename Value>
void ConcurrentList<Value>::clear() {
  // boost::unique_lock<boost::shared_mutex> listLock(listMutex_);

  ListNode* node = head_.next_;
  ListNode* next;
  while (node != &tail_) {
    next = node->next_;
    delete node;
    node = next;
  }
  head_.next_ = &tail_;
  tail_.prev_ = &head_;
}

/*
 *
 *               Head    1   2   Tail
 *  PopBack              l   l    l
 *  PushFront     l      l    
 */

template <typename Value>
std::vector<Value> ConcurrentList<Value>::toVector() {
  // boost::unique_lock<boost::shared_mutex> listLock(listMutex_);
  boost::unique_lock<boost::shared_mutex> headLock(head_.itemMutex_);
  ListNode* current = head_.next_;
  std::vector<Value> result;

  while (current != &tail_) {
    boost::unique_lock<boost::shared_mutex> itemLock(current->itemMutex_);
    result.push_back(current->value_);
    current = current->next_;
  }
  return result;
}

template <typename Value>
std::vector<Value> ConcurrentList<Value>::toVectorReverse() {
  boost::unique_lock<boost::shared_mutex> tailLock(tail_.itemMutex_);
  ListNode* current = tail_.prev_;
  std::vector<Value> result;

  while (current != &head_) {
    boost::unique_lock<boost::shared_mutex> itemLock(current->itemMutex_);
    result.push_back(current->value_);
    current = current->prev_;
  }
  return result;
}

template <typename Value>
void ConcurrentList<Value>::unsafeDelink(ListNode* node) {
  ListNode* prev = node->prev_;
  ListNode* next = node->next_;
  prev->next_ = next;
  next->prev_= prev;
  node->prev_ = OutOfListMarker;
}

template <typename Value>
void ConcurrentList<Value>::unsafePushFront(ListNode* node) {
  ListNode* oldRealHead = head_.next_;
  node->prev_ = &head_;
  node->next_ = oldRealHead;
  oldRealHead->prev_ = node;
  head_.next_ = node;
}

template <typename Value>
void ConcurrentList<Value>::pushFront(ListNode* node) {
  if (node == nullptr) {
    return;
  }

  VLOG(2) << "Calling push front " << node << " size " << size_ << " trying " << &head_;
  boost::unique_lock<boost::shared_mutex> headLock(head_.itemMutex_);
  VLOG(2) << "pushFront: Acquired head lock " << &head_ << " trying " << node;
  boost::unique_lock<boost::shared_mutex> itemLock(node->itemMutex_);
  VLOG(2) << "pushFront: Acquired item lock " << node << " trying " << head_.next_;

  if (node->isInList()) {
    return;
  }

  boost::unique_lock<boost::shared_mutex> oldRealHeadLock(head_.next_->itemMutex_);
  VLOG(2) << "pushFront: Acquired next lock " << head_.next_;
  unsafePushFront(node);

  size_++;
}

template <typename Value>
void ConcurrentList<Value>::moveToFront(ListNode* node) {
  // boost::unique_lock<boost::shared_mutex> listLock(listMutex_);
  // // TODO: Fix the is in list call
  VLOG(2) << "MOVE TO FRONT!! Calling move to front " << node;

  if (node == nullptr) {
    return;
  }

  if (node->isInList()) {
    // Delink
    boost::unique_lock<boost::shared_mutex> prevLock(node->prev_->itemMutex_);
    boost::unique_lock<boost::shared_mutex> itemLock(node->itemMutex_);
    boost::unique_lock<boost::shared_mutex> nextLock(node->next_->itemMutex_);

    unsafeDelink(node);

    prevLock.unlock();
    nextLock.unlock();

    // Push front
    boost::unique_lock<boost::shared_mutex> headLock(head_.itemMutex_);
    boost::unique_lock<boost::shared_mutex> oldRealHeadLock(head_.next_->itemMutex_);
    unsafePushFront(node);
  } else {
    // Push front
    boost::unique_lock<boost::shared_mutex> headLock(head_.itemMutex_);
    boost::unique_lock<boost::shared_mutex> itemLock(node->itemMutex_);
    boost::unique_lock<boost::shared_mutex> oldRealHeadLock(head_.next_->itemMutex_);
    unsafePushFront(node);
  }
}


template <typename Value>
typename ConcurrentList<Value>::ListNode* ConcurrentList<Value>::popBack() {
  if (tail_.prev_ == &head_) {
    // List is empty, can't evict
    return nullptr;
  }
  VLOG(2) << "Calling popBack size: " << size_ << " trying " << tail_.prev_->prev_;

  boost::unique_lock<boost::shared_mutex> prevLock(tail_.prev_->prev_->itemMutex_);
  VLOG(2) << "popBack: Acquired prev lock " << tail_.prev_->prev_ << " trying " << tail_.prev_;
  boost::unique_lock<boost::shared_mutex> itemLock(tail_.prev_->itemMutex_);
  VLOG(2) << "popBack: Acquired item lock " << tail_.prev_ << " trying " << &tail_;
  boost::unique_lock<boost::shared_mutex> nextLock(tail_.itemMutex_);
  VLOG(2) << "popBack: Acquired next lock" << &tail_;

  ListNode* moribund = tail_.prev_;
  VLOG(2) << "moribund " << moribund << " head " << &head_  << " tail " << &tail_;
  unsafeDelink(moribund);
  size_--;
  return moribund;;
}













































/*
template <typename Value>
class ConcurrentImmutableValueList {
private:
  struct ListNode {
    Value value_;
    mutable boost::shared_mutex valueLock_;
    mutable boost::shared_mutex prevMutex_;
    mutable boost::shared_mutex nextMutex_;
  };

public:
  void moveToFront(const LockedConstIterator& it);
  LockedConstIterator pushFront(const Value& value);
  void popBack();

private:
  std::list<ListNode> list_;
  mutable boost::shared_mutex frontMutex_;
  mutable boost::shared_mutex endMutex_;

  void erase(typename std::list<Value>::iterator& it);
};

template <typename Value>
void ConcurrentImmutableValueList<Value>::erase(typename std::list<Value>::iterator& it) {
  if (it == list_.end()) {
    // Nothing to do
    return;
  }

  auto prev = it;
  // Not the first item, go back and pick fwd lock
  if (it != list_.begin()) {
    prev--;
  }
  auto prevItemNextLock = (it != list_.begin()) ?
    boost::unique_lock<boost::shared_mutex>((*prev).nextLock_) :
    boost::unique_lock<boost::shared_mutex>();

  // Pick this item's locks
  auto item = *it;
  boost::unique_lock<boost::shared_mutex> itemLock(item.valueLock_);
  boost::unique_lock<boost::shared_mutex> nextLock(item.nextLock_);
  boost::unique_lock<boost::shared_mutex> prevLock(item.prevLock_);

  auto next = it;
  next++;
  // Not the last item, go forward and pick prevLock
  auto nextItemPrevLock = (next != list_.end()) ?
    boost::unique_lock<boost::shared_mutex>((*next).prevLock_) :
    boost::unique_lock<boost::shared_mutex>();

  // Got all 5 locks needed, let's erase
  list_.erase(it);
}

template <typename Value>
void ConcurrentImmutableValueList<Value>::popBack() {
  if (list_.begin() == list_.end()) {
    // Nothing to do
    return;
  }

  // Lock the back
  boost::unique_lock<boost::shared_mutex> endLock(endMutex_);
  if (list_.begin() == list_.end()) {
    // Nothing to do
    return;
  }

  auto it = list_.end();
  it--;
  erase(it);
}

template<typename Value>
LockedConstIterator ConcurrentImmutableValueList<Value>::pushFront(const Value& value) {

}


  void moveToFront(const LockedConstIterator& it);
  */

