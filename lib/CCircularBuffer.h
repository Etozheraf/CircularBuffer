#pragma once

#include "normal_iterator.h"

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

    constexpr CCircularBuffer() noexcept : allocator_(), capacity_(0), start_in_memory_(nullptr), head_(0),
        size_(0) {}

    explicit constexpr CCircularBuffer(size_type capacity)
        : allocator_(),
        start_in_memory_(Alloc_traits::allocate(allocator_, capacity)),
        head_(0), capacity_(capacity), size_(0) {}

    explicit constexpr CCircularBuffer(const Alloc& allocator) noexcept
        : start_in_memory_(nullptr), head_(0), capacity_(0), size_(0),
        allocator_(allocator) {}

    explicit constexpr CCircularBuffer(size_type capacity, const Alloc& allocator)
        : start_in_memory_(Alloc_traits::allocate(allocator, capacity)), head_(0),
        capacity_(capacity), size_(0), allocator_(allocator) {}

    constexpr CCircularBuffer(const CCircularBuffer& other)
        : allocator_(),
        start_in_memory_(Alloc_traits::allocate(allocator_, other.capacity_)),
        head_(other.head_), size_(other.size_), capacity_(other.capacity_) {

        for (size_type i = head_; i < head_ + size_; i++) {
            Alloc_traits::construct(allocator_, start_in_memory_ + i % capacity_,
                other.start_in_memory_[i % capacity_]);
        }
    };

    constexpr CCircularBuffer(CCircularBuffer&& other) noexcept
        : start_in_memory_(0), head_(0), size_(0), capacity_(0) {
        std::swap(start_in_memory_, other.start_in_memory_);
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    CCircularBuffer(std::initializer_list<value_type> l)
        : CCircularBuffer(l.begin(), l.end()) {}

    constexpr CCircularBuffer(std::initializer_list<value_type> l, const Alloc& allocator)
        : capacity_(2 * l.size()),
        start_in_memory_(Alloc_traits::allocate(allocator, l.size())), head_(0),
        size_(l.size()), allocator_(allocator) {

        for (auto i = l.begin(), j = head_; i != l.end(); i++, j++) {
            Alloc_traits::construct(allocator, start_in_memory_ + j % capacity_, *i);
        }
    }

    constexpr CCircularBuffer(size_type n, value_type t) : CCircularBuffer(2 * n) {
        size_ = n;
        for (size_type i = head_; i < head_ + size_; i++) {
            Alloc_traits::construct(allocator_, start_in_memory_ + i, t);
        }
    }

    template<typename InputIterator,
        typename = std::_RequireInputIter<InputIterator>>
        CCircularBuffer(InputIterator begin, InputIterator end)
        : CCircularBuffer(2 * (end - begin)) {
        size_ = end - begin;
        for (auto i = 0, j = begin; i < size_; ++i, ++j) {
            Alloc_traits::construct(allocator_, start_in_memory_ + i, *j);
        }
    }

    ~CCircularBuffer() {
        if (start_in_memory_ != nullptr) {
            for (size_t i = head_; i < head_ + size_; i++) {
                Alloc_traits::destroy(allocator_, start_in_memory_ + i % capacity_);
            }

            Alloc_traits::deallocate(allocator_, start_in_memory_, capacity_);
        }
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

        for (size_type i = head_; i < head_ + size_; i++) {
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

    constexpr CCircularBuffer& operator=(std::initializer_list<value_type> l) {
        *this = CCircularBuffer(l);
        return *this;
    }

    template<typename InputIterator,
        typename = std::_RequireInputIter<InputIterator>>
        void assign(InputIterator b, InputIterator e) {
        CCircularBuffer c(b, e);
        *this = c;
    }

    void assign(std::initializer_list<value_type> il) {
        CCircularBuffer c(il);
        *this = c;
    }

    void assign(size_t n, const value_type& t) {
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
        return iterator(capacity_, start_in_memory_, head_, size_, capacity_);
    }

    constexpr const_iterator end() const noexcept {
        return const_iterator(capacity_, start_in_memory_, head_, size_, capacity_);
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

    [[nodiscard]] constexpr size_type size() const noexcept { return size_; }

    [[nodiscard]] constexpr size_type capacity() const noexcept { return capacity_; }

    [[nodiscard]] constexpr size_type max_size() const noexcept {
        return std::allocator_traits<Alloc>::max_size(allocator_);
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

    // Sequence container

    template<typename... Args>
    constexpr iterator emplace(const_iterator cp, Args&& ...args) {
        return insert(cp, std::forward<Args>(args)...);
    }

    virtual iterator insert(const_iterator cp, const value_type& t) {
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
            pointer ptr = &(*i);
            Alloc_traits::destroy(allocator_, ptr);
            Alloc_traits::construct(allocator_, ptr, *(i - 1));
            --i;
        }
        Alloc_traits::construct(allocator_, &(*i), t);
        return i;
    }

    virtual iterator insert(const_iterator cp, value_type&& t) {
        if (cp != cend()) {
            ++size_;

            int n = cp - cbegin();
            iterator p = begin() + n;
            iterator i = end() - 1;
            Alloc_traits::construct(allocator_, &(*i), *(i - 1));
            --i;
            while (i != p) {
                pointer ptr = &(*i);
                Alloc_traits::destroy(allocator_, ptr);
                Alloc_traits::construct(allocator_, ptr, *(i - 1));
                --i;
            }
            Alloc_traits::construct(allocator_, &(*i), t);
            return i;
        }
        ++size_;
        Alloc_traits::construct(allocator_, &*(end() - 1), t);
        return (end() - 1);
    }

    constexpr iterator insert(const_iterator cp, size_type n, value_type t) {
        return insert(cp, CCircularBuffer(n, t));
    }

    template<typename InputIterator,
        typename = std::_RequireInputIter<InputIterator>>
        iterator insert(const_iterator cp, InputIterator b, InputIterator e) {
        return insert(cp, CCircularBuffer(b, e));
    }

    iterator insert(const_iterator cp, std::initializer_list<value_type> il) {
        return insert(cp, CCircularBuffer(il.begin(), il.end()));
    }

    virtual iterator insert(const_iterator cp,
        const CCircularBuffer<value_type>& a) {
        int n = a.size_;
        if (cp != cend()) {
            int distance = cp - cbegin();

            size_ += n;
            if (size_ > capacity_) {
                size_ %= capacity_;
            }

            iterator p = begin() + distance;
            iterator i = end() - 1;
            while (i != p) {
                pointer ptr = &(*i);
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

    virtual iterator insert(const_iterator cp, CCircularBuffer<value_type>&& a) {
        int n = a.size_;
        if (cp != cend()) {
            int distance = cp - cbegin();

            size_ += n;
            if (size_ > capacity_) {
                size_ %= capacity_;
            }

            iterator p = begin() + distance;
            iterator i = end() - 1;
            while (i != p) {
                pointer ptr = &(*i);
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

    constexpr iterator erase(const_iterator cp) {
        iterator p = begin() + (cp - cbegin());
        if (cp != cend() && size_ > 0) {
            pointer ptr;
            iterator i = p;

            while (i != end() - 1) {
                ptr = &*i;

                Alloc_traits::destroy(allocator_, ptr);
                Alloc_traits::construct(allocator_, ptr, *(i + 1));
                ++i;
            }
            Alloc_traits::destroy(allocator_, &*i);

            --size_;
        }
        return p;
    }

    constexpr iterator erase(const_iterator cq1, const_iterator cq2) {
        iterator q1 = begin() + (cq1 - cbegin());
        iterator q2 = begin() + (cq2 - cbegin());
        if (q1 != end() && q1 < q2 && size_ > q2 - q1) {
            pointer ptr;
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
        }
        return q1;
    }

    void clear() {
        for (iterator i = begin(); i != end(); ++i) {
            Alloc_traits::destroy(allocator_, &(*i));
        }
        size_ = 0;
    }

    constexpr reference front() noexcept { return start_in_memory_[head_]; }

    constexpr const_reference front() const noexcept { return start_in_memory_[head_]; }

    constexpr reference back() noexcept { return start_in_memory_[(head_ + size_ - 1) % capacity_]; }

    constexpr const_reference back() const noexcept {
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

    virtual void push_front(const value_type& value) {

        if (head_ > 0) {
            --head_;
        }
        else {
            head_ = capacity_ - 1;
        }
        Alloc_traits::construct(allocator_, start_in_memory_ + head_, value);

        ++size_;
        if (size_ > capacity_)
            size_ %= capacity_;
    }

    virtual void push_front(value_type&& value) {

        if (head_ > 0) {
            --head_;
        }
        else {
            head_ = capacity_ - 1;
        }
        Alloc_traits::construct(allocator_, start_in_memory_ + head_,
            std::move(value));

        ++size_;
        if (size_ > capacity_)
            size_ %= capacity_;
    }

    virtual void push_back(const value_type& value) {

        Alloc_traits::construct(
            allocator_, start_in_memory_ + ((head_ + size_) % capacity_), value);

        ++size_;
        if (size_ > capacity_)
            size_ %= capacity_;
    }

    virtual void push_back(value_type&& value) {

        Alloc_traits::construct(allocator_,
            start_in_memory_ + ((head_ + size_) % capacity_),
            std::move(value));

        ++size_;
        if (size_ > capacity_)
            size_ %= capacity_;
    }

    void pop_front() {
        if (size_ > 0) {
            Alloc_traits::destroy(allocator_, start_in_memory_ + head_);

            ++head_;
            head_ %= capacity_;

            --size_;
        }
        else throw "Trying to extract an element from an empty buffer";
    }

    void pop_back() {
        if (size_ > 0) {
            Alloc_traits::destroy(allocator_,
                start_in_memory_ + (head_ + size_ - 1) % capacity_);

            --size_;
        }
        else throw "Trying to extract an element from an empty buffer";
    }

    constexpr reference operator[](size_type n) {
        return start_in_memory_[head_ + n % capacity_];
    }

    constexpr const_reference operator[](size_type n) const {
        return start_in_memory_[head_ + n % capacity_];
    }

    constexpr reference at(size_type n) { return start_in_memory_[head_ + n % capacity_]; }

    constexpr const_reference at(size_type n) const {
        return start_in_memory_[head_ + n % capacity_];
    }

protected:
    T* start_in_memory_;
    size_t head_;
    size_t size_;
    size_t capacity_;
    Alloc allocator_;
};

