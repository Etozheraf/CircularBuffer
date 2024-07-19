#pragma once

#include <iostream>
#include <iterator>


template<typename T>
class normal_iterator {
public:
    using iterator_type =
            typename std::iterator<std::random_access_iterator_tag, T>::iterator;
    using iterator_category =
            typename std::iterator<std::random_access_iterator_tag,
                    T>::iterator_category;
    using value_type =
            typename std::iterator<std::random_access_iterator_tag, T>::value_type;
    using difference_type =
            typename std::iterator<std::random_access_iterator_tag,
                    T>::difference_type;
    using reference =
            typename std::iterator<std::random_access_iterator_tag, T>::reference;
    using pointer =
            typename std::iterator<std::random_access_iterator_tag, T>::pointer;
    using size_type = size_t;

    constexpr normal_iterator(size_type index, pointer start_in_memory, size_type head,
                              size_type size, size_type length)

    noexcept
            : index_(index), start_in_memory_(start_in_memory), head_(head),
    size_(size), capacity_(length) {}

    constexpr normal_iterator(const normal_iterator& it)

    noexcept
            : index_(it.index_), start_in_memory_(it
    .start_in_memory_),
    head_(it
    .head_),
    size_(it
    .size_),
    capacity_(it
    .capacity_) {}

    constexpr normal_iterator(normal_iterator&& it)

    noexcept {
        std::swap(index_, it.index_);
        std::swap(start_in_memory_, it.start_in_memory_);
        std::swap(head_, it.head_);
        std::swap(size_, it.size_);
        std::swap(capacity_, it.capacity_);
    }

    constexpr normal_iterator
    &

    operator=(const normal_iterator& it) {
        index_ = it.index_;
        start_in_memory_ = it.start_in_memory_;
        head_ = it.head_;
        size_ = it.size_;
        capacity_ = it.capacity_;
        return *this;
    }

    constexpr normal_iterator
    &

    operator=(normal_iterator&& it)

    noexcept {
        std::swap(index_, it.index_);
        std::swap(start_in_memory_, it.start_in_memory_);
        std::swap(head_, it.head_);
        std::swap(size_, it.size_);
        std::swap(capacity_, it.capacity_);
        return *this;
    }

    constexpr reference

    operator*() { return start_in_memory_[index_]; };

    constexpr normal_iterator
    &

    operator++()

    noexcept {
        if (index_ == (head_ + size_ - 1) % capacity_) {
            index_ = capacity_;
        } else if (index_ == capacity_ - 1) {
            index_ = 0;
        } else {
            ++index_;
        }
        return *this;
    }

    constexpr normal_iterator

    operator++(int)

    noexcept {
        normal_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    constexpr normal_iterator
    &

    operator--()

    noexcept {
        if (index_ == capacity_) {
            index_ = (head_ + size_ - 1) % capacity_;
        } else if (index_ == 0) {
            index_ = capacity_ - 1;
        } else {
            --index_;
        }
        return *this;
    }

    constexpr normal_iterator

    operator--(int)

    noexcept {
        normal_iterator tmp = *this;
        --(*this);
        return tmp;
    };

    constexpr normal_iterator
    &

    operator+=(difference_type n)

    noexcept {
        size_type offset = 0;
        if (index_ < head_) {
            offset = capacity_;
        }
        if (index_ + n + offset >= head_ + size_) {
            index_ = capacity_ + index_ + n + offset - head_ - size_;
        } else {
            index_ += n;
            index_ %= capacity_;
        }
        return *this;
    }

    constexpr normal_iterator
    &

    operator-=(difference_type n)

    noexcept {
        size_type offset = 0;
        if (index_ < head_) {
            offset = capacity_;
        }
        if (index_ == capacity_ && size_ > n) {
            index_ = (head_ + size_ - n) % capacity_;
        } else if (index_ + offset <= head_ + n || size_ < n) {
            index_ = index_ + offset - n;
        } else {
            index_ = (index_ + offset - n) % capacity_;
        }
        return *this;
    }


    constexpr normal_iterator

    operator+(difference_type n) const

    noexcept {
        size_type offset = 0;
        if (index_ < head_) {
            offset = capacity_;
        }
        if (index_ + n + offset >= head_ + size_) {
            return normal_iterator(capacity_ + index_ + n + offset - head_ - size_,
                                   start_in_memory_, head_, size_, capacity_);
        } else {
            return normal_iterator((index_ + n) % capacity_, start_in_memory_, head_,
                                   size_, capacity_);
        }
    }

    friend constexpr normal_iterator

    operator+(difference_type n, normal_iterator i)

    noexcept {
        return i + n;
    }


    constexpr normal_iterator

    operator-(difference_type n) const

    noexcept {
        size_type offset = 0;
        if (index_ < head_) {
            offset = capacity_;
        }
        if (index_ >= capacity_ && size_ >= n - index_ + capacity_) {
            return normal_iterator((head_ + size_ - n + index_ - capacity_) % capacity_, start_in_memory_,
                                   head_, size_, capacity_);
        } else if (index_ + offset <= head_ + n || size_ < n) {
            if (index_ >= capacity_) {
                return normal_iterator(head_ + size_ - n, start_in_memory_, head_,
                                       size_, capacity_);
            } else {
                return normal_iterator(index_ + offset - n, start_in_memory_, head_,
                                       size_, capacity_);
            }
        } else {
            return normal_iterator((index_ + offset - n) % capacity_,
                                   start_in_memory_, head_, size_, capacity_);
        }
    }


    constexpr difference_type

    operator-(const normal_iterator& a) const

    noexcept {
        size_type index = index_;
        if (index_ == capacity_) {
            index = head_ + size_;
        }
        size_type index_of_a = a.index_;
        if (a.index_ == a.capacity_) {
            index_of_a = a.head_ + a.size_;
        }
        size_type offset = 0;
        if (index_ < head_) {
            offset = capacity_;
        }
        size_type offset_of_a = 0;
        if (a.index_ < a.head_) {
            offset_of_a = a.capacity_;
        }
        return index + offset - index_of_a - offset_of_a;
    }


    constexpr reference

    operator[](difference_type n) const { return *(*this + n); }


private:
    size_t index_;
    pointer start_in_memory_;
    size_t head_;
    size_t size_;
    size_t capacity_;
};

template<typename T>
constexpr inline bool operator<(const normal_iterator<T>& a, const normal_iterator<T>& b)

noexcept {
if (b - a > 0)
return true;
else
return false;
}

template<typename T>
constexpr inline bool operator>(const normal_iterator<T>& a, const normal_iterator<T>& b)

noexcept {
return
b<a;
}

template<typename T>
constexpr inline bool operator>=(const normal_iterator<T>& a, const normal_iterator<T>& b)

noexcept {
return !(a<b); }

template<typename T>
constexpr inline bool operator<=(const normal_iterator<T>& a, const normal_iterator<T>& b)

noexcept {
return !(a > b); }

template<typename T>
constexpr inline bool operator==(const normal_iterator<T>& a, const normal_iterator<T>& b)

noexcept {
return ((a - b) == 0);
}

template<typename T>
constexpr inline bool operator!=(const normal_iterator<T>& a, const normal_iterator<T>& b)

noexcept {
return !(a == b);
}
