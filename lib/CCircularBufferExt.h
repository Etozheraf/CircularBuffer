#pragma once

#include "CCircularBuffer.h"


template<typename T, typename Alloc = std::allocator<T>>
class CCircularBufferExt : public CCircularBuffer<T, Alloc> {
public:
    using B = CCircularBuffer<T, Alloc>; // B - Base

    using iterator = normal_iterator<T>;
    using const_iterator = normal_iterator<const T>;

    using value_type = typename B::value_type;

    using Alloc_traits = typename B::Alloc_traits;

    using CCircularBuffer<T, Alloc>::CCircularBuffer;
    using CCircularBuffer<T, Alloc>::insert;

    constexpr iterator insert(const_iterator cp, const value_type& t) override {
        if (B::size_ == B::capacity_) {
            double_up();
        }
        return B::insert(cp, t);
    }

    constexpr iterator insert(const_iterator cp, value_type&& t) override {
        if (B::size_ == B::capacity_) {
            double_up();
        }
        return B::insert(cp, t);
    }

    constexpr iterator insert(const_iterator cp,
                              const CCircularBuffer<value_type>& a) override {
        while ((B::size_ + a.size()) > B::capacity_) {
            double_up();
        }
        return B::insert(cp, a);
    }

    constexpr iterator insert(const_iterator cp, CCircularBuffer<value_type>&& a) override {
        while (B::size_ + a.size() > B::capacity_) {
            double_up();
        }
        return B::insert(cp, a);
    }

    constexpr void push_front(const value_type& value) override {
        if (B::size_ == B::capacity_) {
            double_up();
        }
        return B::push_front(value);
    }

    constexpr void push_front(value_type&& value) override {
        if (B::size_ == B::capacity_) {
            double_up();
        }
        return B::push_front(value);
    }

    constexpr void push_back(const value_type& value) override {
        if (B::size_ == B::capacity_) {
            double_up();
        }
        return B::push_back(value);
    }

    constexpr void push_back(value_type&& value) override {
        if (B::size_ == B::capacity_) {
            double_up();
        }
        return B::push_back(value);
    }

private:
    constexpr inline void double_up() {
        if (!B::start_in_memory_) {
            B::capacity_ = 1;
            B::start_in_memory_ = Alloc_traits::allocate(B::allocator_, B::capacity_);
            return;
        }

        value_type* t = Alloc_traits::allocate(B::allocator_, 2 * B::capacity_);
        for (size_t i = 0; i < B::capacity_; i++) {
            Alloc_traits::construct(B::allocator_, t + i, std::move(B::start_in_memory_[i]));
            Alloc_traits::destroy(B::allocator_, B::start_in_memory_ + i);
        }
        Alloc_traits::deallocate(B::allocator_, B::start_in_memory_, B::capacity_);
        B::capacity_ *= 2;
        B::start_in_memory_ = t;
    }
};