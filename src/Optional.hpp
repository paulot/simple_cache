#pragma once

#include <utility>
#include <stdexcept>

template <class Value>
class Optional {
public:
  Optional() : hasValue_(false) {}
  Optional(Value&& value) : hasValue_(true), value_(std::move(value)) {}
  static Optional<Value> none() { return Optional(); }

  bool hasValue() const { return hasValue_; }

  const Value& value() const& {
    require_value();
    return value();
  }

  Value& value() & {
    require_value();
    return value();
  }

  Value&& value() const&& {
    require_value();
    hasValue_ = false;
    return std::move(value_);
  }

  Value&& value() && {
    require_value();
    hasValue_ = false;
    return std::move(value_);
  }

  friend bool operator==(const Optional<Value>& lhs, const Optional<Value>& rhs) {
    if (!lhs.hasValue() || !rhs.hasValue()) {
      // If either has no value
      return lhs.hasValue() == rhs.hasValue();
    }
    // Both have a value
    return lhs.value() == rhs.value();
  }

private:
  bool hasValue_;
  Value value_;

  void require_value() const {
    if (!this->hasValue()) {
      throw std::runtime_error("No value");
    }
  }
};
