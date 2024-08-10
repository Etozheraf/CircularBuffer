#pragma once

#include "normal_iterator.h"

class FullBufferException : std::exception {
    const char* what() const noexcept override {
        return "Buffer is full";
    }
};

class EmptyBufferException : std::exception {
    const char* what() const noexcept override {
        return "Buffer is empty";
    }
};

template<typename T, typename Alloc = std::allocator<T>>
class CCircularBuffer {

public:
    using iterator = normal_iterator<T>;
    using const_iterator = normal_iterator<const T>;

    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using size_type = std::size_t;

    using Alloc_traits = std::allocator_traits<Alloc>;

    // Container

    constexpr CCircularBuffer() noexcept: allocator_(), capacity_(0), start_in_memory_(nullptr), head_(0),
                                          size_(0) {}

    explicit constexpr CCircularBuffer(size_t capacity)
            : allocator_(),
              capacity_(capacity),
              start_in_memory_(Alloc_traits::allocate(allocator_, capacity_)),
              head_(0), size_(0) {}


    explicit constexpr CCircularBuffer(const Alloc& allocator) noexcept
            : start_in_memory_(nullptr), head_(0), capacity_(0), size_(0),
              allocator_(allocator) {}

    explicit constexpr CCircularBuffer(size_t capacity, const Alloc& allocator)
            : start_in_memory_(Alloc_traits::allocate(allocator_, capacity_)), head_(0),
              capacity_(capacity), size_(0), allocator_(allocator) {}

    constexpr CCircularBuffer(const CCircularBuffer& other)
            : allocator_(),
              start_in_memory_(Alloc_traits::allocate(allocator_, capacity_)),
              head_(other.head_), size_(other.size_), capacity_(other.capacity_) {

        for (size_t i = head_; i < head_ + size_; i++) {
            Alloc_traits::construct(allocator_, start_in_memory_ + i % capacity_,
                                    other.start_in_memory_[i % capacity_]);
        }
    };

    constexpr CCircularBuffer(CCircularBuffer&& other) noexcept
            : start_in_memory_(0), head_(0), size_(0), capacity_(0), allocator_() {
        std::swap(start_in_memory_, other.start_in_memory_);
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(allocator_, other.allocator_);
    }

    CCircularBuffer(std::initializer_list<T> l)
            : capacity_(l.size()),
              start_in_memory_(Alloc_traits::allocate(allocator_, capacity_)), head_(0),
              size_(l.size()), allocator_() {
        size_t j = head_;
        for (auto i = l.begin(); i != l.end(); i++, j++) {
            Alloc_traits::construct(allocator_, start_in_memory_ + j % capacity_, *i);
        }
    }

    constexpr CCircularBuffer(std::initializer_list<T> l, const Alloc& allocator)
            : capacity_(l.size()),
              start_in_memory_(Alloc_traits::allocate(allocator_, capacity_)), head_(0),
              size_(l.size()), allocator_(allocator) {
        size_t j = head_;
        for (auto i = l.begin(); i != l.end(); i++, j++) {
            Alloc_traits::construct(allocator, start_in_memory_ + j % capacity_, *i);
        }
    }

    constexpr CCircularBuffer(size_t n, T t) : CCircularBuffer(n) {
        size_ = n;
        for (size_t i = head_; i < head_ + size_; i++) {
            Alloc_traits::construct(allocator_, start_in_memory_ + i, t);
        }
    }

    template<typename InputIterator,
            typename = std::_RequireInputIter<InputIterator>>
    CCircularBuffer(InputIterator begin, InputIterator end) {
        size_ = 0;
        head_ = 0;
        for (auto i = begin; i != end; ++i) {
            ++size_;
        }
        capacity_ = size_;

        start_in_memory_ = Alloc_traits::allocate(allocator_, capacity_);
        auto j = begin;
        for (auto i = 0; i < size_; ++i, ++j) {
            Alloc_traits::construct(allocator_, start_in_memory_ + i, *j);
        }
    }

    ~CCircularBuffer() {
        if (start_in_memory_ == nullptr) {
            return;
        }
        for (size_t i = head_; i < head_ + size_; i++) {
            Alloc_traits::destroy(allocator_, start_in_memory_ + i % capacity_);
        }

        Alloc_traits::deallocate(allocator_, start_in_memory_, capacity_);
    }

    constexpr CCircularBuffer& operator=(const CCircularBuffer& other) {

        if (start_in_memory_ != nullptr) {
            for (size_t i = head_; i < head_ + size_; i++) {
                Alloc_traits::destroy(allocator_, start_in_memory_ + i % capacity_);
            }

            Alloc_traits::deallocate(allocator_, start_in_memory_, capacity_);
        }

        capacity_ = other.capacity_;
        size_ = other.size_;
        head_ = other.head_;
        allocator_ = other.allocator_;
        start_in_memory_ = Alloc_traits::allocate(allocator_, capacity_);

        for (size_t i = head_; i < head_ + size_; i++) {
            Alloc_traits::construct(allocator_, start_in_memory_ + i % capacity_,
                                    other.start_in_memory_[i % capacity_]);
        }

        return *this;
    }

    constexpr CCircularBuffer& operator=(CCircularBuffer&& other) noexcept {
        std::swap(start_in_memory_, other.start_in_memory_);
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(allocator_, other.allocator_);
        return *this;
    }

    constexpr CCircularBuffer& operator=(std::initializer_list<T> l) {
        *this = CCircularBuffer(l);
        return *this;
    }

    template<typename InputIterator,
            typename = std::_RequireInputIter<InputIterator>>
    void assign(InputIterator b, InputIterator e) {
        CCircularBuffer c(b, e);
        *this = c;
    }

    void assign(std::initializer_list<T> il) {
        CCircularBuffer c(il);
        *this = c;
    }

    void assign(size_t n, const T& t) {
        CCircularBuffer c(n, t);
        *this = c;
    }

    constexpr iterator begin() noexcept {
        return iterator(head_, start_in_memory_, head_, size_, capacity_);
    }

    constexpr const_iterator begin() const noexcept {
        return const_iterator(head_, start_in_memory_, head_, size_, capacity_);
    }

    constexpr iterator end() noexcept {
        return iterator(head_ + size_, start_in_memory_, head_, size_, capacity_);
    }

    constexpr const_iterator end() const noexcept {
        return const_iterator(head_ + size_, start_in_memory_, head_, size_, capacity_);
    }

    constexpr const_iterator cbegin() const noexcept {
        return const_cast<const CCircularBuffer&>(*this).begin();
    }

    constexpr const_iterator cend() const noexcept {
        return const_cast<const CCircularBuffer&>(*this).end();
    }

    friend constexpr inline bool operator==(const CCircularBuffer& a, const CCircularBuffer& b) {
        if (&a == &b) return true;
        return (a.size_ == b.size_ &&
                std::equal(a.begin(), a.end(), b.begin()));
    }

    constexpr inline bool operator!=(const CCircularBuffer& other) { return !(*this == other); }

    void swap(CCircularBuffer& other) noexcept {
        std::swap(start_in_memory_, other.start_in_memory_);
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    friend void swap(CCircularBuffer& a, CCircularBuffer& b) noexcept { a.swap(b); }

    [[nodiscard]] constexpr size_t size() const noexcept { return size_; }

    [[nodiscard]] constexpr size_t capacity() const noexcept { return capacity_; }

    [[nodiscard]] constexpr size_t max_size() const noexcept {
        return std::allocator_traits<Alloc>::max_size(allocator_);
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

    // Sequence container

    template<typename... Args>
    constexpr iterator emplace(const_iterator cp, Args&& ...args) {
        return insert(cp, std::forward<Args>(args)...);
    }

    virtual iterator insert(const_iterator cp, const T& t) {
        if (size_ == capacity_) {
            throw FullBufferException();
        }

        if (cp == cend()) {
            ++size_;
            Alloc_traits::construct(allocator_, &*(end() - 1), t);
            return (end() - 1);
        }
        ++size_;
        int n = cp - cbegin();
        iterator p = begin() + n;
        iterator i = end() - 1;
        Alloc_traits::construct(allocator_, &(*i), *(i - 1));
        --i;
        while (i != p) {
            T* ptr = &(*i);
            Alloc_traits::destroy(allocator_, ptr);
            Alloc_traits::construct(allocator_, ptr, *(i - 1));
            --i;
        }
        Alloc_traits::construct(allocator_, &(*i), t);
        return i;
    }

    virtual iterator insert(const_iterator cp, T&& t) {
        if (size_ == capacity_) {
            throw FullBufferException();
        }

        if (cp == cend()) {
            ++size_;
            Alloc_traits::construct(allocator_, &*(end() - 1), t);
            return (end() - 1);
        }

        ++size_;
        int n = cp - cbegin();
        iterator p = begin() + n;
        iterator i = end() - 1;
        Alloc_traits::construct(allocator_, &(*i), *(i - 1));
        --i;
        while (i != p) {
            T* ptr = &(*i);
            Alloc_traits::destroy(allocator_, ptr);
            Alloc_traits::construct(allocator_, ptr, *(i - 1));
            --i;
        }
        Alloc_traits::construct(allocator_, &(*i), t);
        return i;
    }

    constexpr iterator insert(const_iterator cp, size_t n, T t) {
        return insert(cp, CCircularBuffer(n, t));
    }

    template<typename InputIterator,
            typename = std::_RequireInputIter<InputIterator>>
    iterator insert(const_iterator cp, InputIterator b, InputIterator e) {
        return insert(cp, CCircularBuffer(b, e));
    }

    iterator insert(const_iterator cp, std::initializer_list<T> il) {
        return insert(cp, CCircularBuffer(il.begin(), il.end()));
    }

    virtual iterator insert(const_iterator cp,
                            const CCircularBuffer<T>& a) {
        if (size_ + a.size_ > capacity_) {
            throw FullBufferException();
        }

        int n = a.size_;
        if (cp == cend()) {
            size_ += n;
            if (size_ > capacity_)
                size_ %= capacity_;
            auto t = (end() - n);
            for (auto i = end() - 1, k = a.end() - 1; i != (end() - n); --i, --k) {
                Alloc_traits::construct(allocator_, &(*i), *k);
            }
            Alloc_traits::construct(allocator_, &*(end() - n), *a.begin());

            return (end() - n);
        }
        int distance = cp - cbegin();

        size_ += n;
        if (size_ > capacity_) {
            size_ %= capacity_;
        }

        iterator p = begin() + distance;
        iterator i = end() - 1;
        while (i != p) {
            T* ptr = &(*i);
            if (i <= end() - n) {
                Alloc_traits::destroy(allocator_, ptr);
            }

            Alloc_traits::construct(allocator_, ptr, *(i - n));
            --i;
        }

        for (auto j = i, k = a.cbegin(); j < i + n; ++j, ++k) {
            Alloc_traits::construct(allocator_, &(*j), *k);
        }
        return i;
    }

    virtual iterator insert(const_iterator cp, CCircularBuffer<T>&& a) {
        if (size_ + a.size_ > capacity_) {
            throw FullBufferException();
        }

        int n = a.size_;
        if (cp == cend()) {
            size_ += n;
            if (size_ > capacity_)
                size_ %= capacity_;
            auto t = (end() - n);
            for (auto i = end() - 1, k = a.end() - 1; i != (end() - n); --i, --k) {
                Alloc_traits::construct(allocator_, &(*i), *k);
            }
            Alloc_traits::construct(allocator_, &*(end() - n), *a.begin());

            return (end() - n);
        }
        int distance = cp - cbegin();

        size_ += n;
        if (size_ > capacity_) {
            size_ %= capacity_;
        }

        iterator p = begin() + distance;
        iterator i = end() - 1;
        while (i != p) {
            T* ptr = &(*i);
            if (i <= end() - n) {
                Alloc_traits::destroy(allocator_, ptr);
            }

            Alloc_traits::construct(allocator_, ptr, *(i - n));
            --i;
        }

        for (auto j = i, k = a.begin(); j < i + n; ++j, ++k) {
            Alloc_traits::construct(allocator_, &(*j), *k);
        }
        return i;
    }

    constexpr iterator erase(const_iterator cp) {

        iterator p = begin() + (cp - cbegin());
        if (cp == cend() || size_ == 0) {
            return p;
        }
        T* ptr;
        iterator i = p;

        while (i != end() - 1) {
            ptr = &*i;

            Alloc_traits::destroy(allocator_, ptr);
            Alloc_traits::construct(allocator_, ptr, *(i + 1));
            ++i;
        }
        Alloc_traits::destroy(allocator_, &*i);

        --size_;
        return p;
    }

    constexpr iterator erase(const_iterator cq1, const_iterator cq2) {

        iterator q1 = begin() + (cq1 - cbegin());
        iterator q2 = begin() + (cq2 - cbegin());
        if (q1 == end() || q1 >= q2 || size_ <= q2 - q1) {
            return q1;
        }
        T* ptr;
        iterator i = q1;
        size_t n = q2 - q1;

        while (i != end() - n) {
            ptr = &*i;

            Alloc_traits::destroy(allocator_, ptr);
            Alloc_traits::construct(allocator_, ptr, *(i + n));
            ++i;
        }
        Alloc_traits::destroy(allocator_, &*i);

        size_ -= n;
        return q1;
    }

    void clear() {
        for (iterator i = begin(); i != end(); ++i) {
            Alloc_traits::destroy(allocator_, &(*i));
        }
        size_ = 0;
    }

    constexpr T& front() noexcept { return start_in_memory_[head_]; }

    constexpr const T& front() const noexcept { return start_in_memory_[head_]; }

    constexpr T& back() noexcept { return start_in_memory_[(head_ + size_ - 1) % capacity_]; }

    constexpr const T& back() const noexcept {
        return start_in_memory_[(head_ + size_) % capacity_];
    }

    template<typename... Args>
    void emplace_front(Args&& ...args) {
        push_front(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void emplace_back(Args&& ...args) {
        push_back(std::forward<Args>(args)...);
    }

    virtual void push_front(const T& value) {
        if (size_ == capacity_) {
            throw FullBufferException();
        }

        if (head_ > 0) {
            --head_;
        } else {
            head_ = capacity_ - 1;
        }
        Alloc_traits::construct(allocator_, start_in_memory_ + head_, value);

        ++size_;
        if (size_ > capacity_)
            size_ %= capacity_;
    }

    virtual void push_front(T&& value) {
        if (size_ == capacity_) {
            throw FullBufferException();
        }

        if (head_ > 0) {
            --head_;
        } else {
            head_ = capacity_ - 1;
        }
        Alloc_traits::construct(allocator_, start_in_memory_ + head_,
                                std::move(value));

        ++size_;
        if (size_ > capacity_)
            size_ %= capacity_;
    }

    virtual void push_back(const T& value) {
        if (size_ == capacity_) {
            throw FullBufferException();
        }

        Alloc_traits::construct(
                allocator_, start_in_memory_ + ((head_ + size_) % capacity_), value);

        ++size_;
        if (size_ > capacity_)
            size_ %= capacity_;
    }

    virtual void push_back(T&& value) {
        if (size_ == capacity_) {
            throw FullBufferException();
        }

        Alloc_traits::construct(allocator_,
                                start_in_memory_ + ((head_ + size_) % capacity_),
                                std::move(value));

        ++size_;
        if (size_ > capacity_)
            size_ %= capacity_;
    }

    void pop_front() {
        if (size_ == 0) {
            throw EmptyBufferException();
        }
        Alloc_traits::destroy(allocator_, start_in_memory_ + head_);

        ++head_;
        head_ %= capacity_;

        --size_;
    }

    void pop_back() {
        if (size_ == 0) {
            throw EmptyBufferException();
        }
        Alloc_traits::destroy(allocator_,
                              start_in_memory_ + (head_ + size_ - 1) % capacity_);

        --size_;
    }

    constexpr T& operator[](size_t n) {
        return start_in_memory_[head_ + n % capacity_];
    }

    constexpr const T& operator[](size_t n) const {
        return start_in_memory_[head_ + n % capacity_];
    }

    constexpr T& at(size_t n) { return start_in_memory_[head_ + n % capacity_]; }

    constexpr const T& at(size_t n) const {
        return start_in_memory_[head_ + n % capacity_];
    }

protected:
    Alloc allocator_;
    size_t capacity_;
    T* start_in_memory_;
    size_t head_;
    size_t size_;
};

