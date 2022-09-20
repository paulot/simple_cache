#include <utility>
#include <stdexcept>

template <class Value>
class Optional {
public:
    Optional() : hasValue_(false) {}
    Optional(Value&& value) : hasValue_(true), value_(std::move(value)) {}

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

private:
    bool hasValue_;
    Value value_;

    void require_value() const {
        if (!this->hasValue()) {
            throw std::runtime_error("No value");
        }
    }
};
