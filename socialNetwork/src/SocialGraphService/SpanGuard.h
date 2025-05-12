#ifndef SOCIAL_NETWORK_MICROSERVICES_SPANGUARD_H
#define SOCIAL_NETWORK_MICROSERVICES_SPANGUARD_H

#include <memory>
#include <opentracing/span.h>

namespace social_network {

template<typename T = opentracing::Span>
class SpanGuard {
public:
  explicit SpanGuard(std::unique_ptr<T>&& s) : span_(std::move(s)) {}
  
  // Ensure span is finished in destructor
  ~SpanGuard() {
    if (span_) {
      span_->Finish();
    }
  }
  
  // Delete copy constructor/assignment to prevent double-frees
  SpanGuard(const SpanGuard&) = delete;
  SpanGuard& operator=(const SpanGuard&) = delete;
  
  // Move constructor - transfer ownership without finishing the original
  SpanGuard(SpanGuard&& other) noexcept : span_(nullptr) {
    span_ = std::move(other.span_);
  }
  
  // Move assignment - finish our span if we have one, then transfer ownership
  SpanGuard& operator=(SpanGuard&& other) noexcept {
    if (this != &other) {
      // Finish current span if it exists
      if (span_) {
        span_->Finish();
      }
      // Take ownership of the other span
      span_ = std::move(other.span_);
    }
    return *this;
  }
  
  // Pointer-like access
  T* operator->() { return span_.get(); }
  const T* operator->() const { return span_.get(); }
  
  // Reference access
  T& operator*() { return *span_; }
  const T& operator*() const { return *span_; }
  
  // Access to raw pointer (use with caution)
  T* get() { return span_.get(); }
  const T* get() const { return span_.get(); }
  
  // Check if span exists
  explicit operator bool() const { return span_ != nullptr; }

private:
  std::unique_ptr<T> span_;
};

} // namespace social_network

#endif // SOCIAL_NETWORK_MICROSERVICES_SPANGUARD_H