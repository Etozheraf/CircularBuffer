#include "lib/CCircularBufferExt.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <algorithm>

TEST(CircularContainer, EmptyConstructorTest) {
    CCircularBuffer<std::string> b;
    EXPECT_EQ(true, b.empty());
}

TEST(CircularContainer, EmptyBufferAddTest) {
    CCircularBufferExt<std::string> b;
    b.push_back("a");
    EXPECT_EQ(true, b.size() == 1);
}

TEST(CircularContainer, CopyConstructorTest) {
//    CCircularBuffer<std::string> a = {"a", "b", "c", "d", "e", "f", "g"};
    CCircularBuffer<int> a = {1, 2, 3, 4, 5, 6, 7};
    CCircularBuffer<int> b(a);
    EXPECT_EQ(a, b);
}

TEST(CircularContainer, MoveConstructorTest) {
    CCircularBuffer<std::string> a = {"a", "b", "c", "d", "e", "f", "g"};
    CCircularBuffer<std::string> copy_of_a = {"a", "b", "c", "d", "e", "f", "g"};
    CCircularBuffer<std::string> b(std::move(a));
    EXPECT_EQ(true, b == copy_of_a);
}

TEST(CircularContainer, AssigmentTest) {
    CCircularBuffer<std::string> a = {"a", "b", "c", "d", "e", "f", "g"};
    CCircularBuffer<std::string> b;
    b = a;
    EXPECT_EQ(true, b == a);
}

TEST(CircularContainer, AssigmentRvalueTest) {
    CCircularBuffer<std::string> a = {"a", "b", "c", "d", "e", "f", "g"};
    CCircularBuffer<std::string> copy_of_a = {"a", "b", "c", "d", "e", "f", "g"};
    CCircularBuffer<std::string> b;
    b = std::move(a);
    EXPECT_EQ(true, b == copy_of_a);
}

TEST(CircularContainer, BeginTest) {
    CCircularBuffer<std::string> a = {"a", "b", "c"};
    EXPECT_EQ("a", *a.begin());
    a.pop_front();
    a.pop_front();
    EXPECT_EQ("c", *a.begin());
    a.push_back("g");
    EXPECT_EQ("c", *a.begin());
    a.pop_front();
    EXPECT_EQ("g", *a.begin());
}

TEST(CircularContainer, EndTest) {
    CCircularBuffer<std::string> a = {"a", "b", "c"};
    EXPECT_EQ("c", *(a.end() - 1));
    a.pop_back();
    a.push_back("g");
    EXPECT_EQ("g", *(a.end() - 1));
}

TEST(CircularContainer, CBeginCendTest) {
    const CCircularBuffer<std::string> a = {"a", "b", "c"};
    CCircularBuffer<std::string> b(a);
    EXPECT_EQ(b.cbegin(), a.begin());
    EXPECT_EQ(b.cend(), a.end());
}

TEST(CircularContainer, SwapTest) {
    CCircularBuffer<std::string> a = {"a", "b", "c"};
    CCircularBuffer<std::string> copy_of_a(a);
    CCircularBuffer<std::string> b{"d", "e", "f", "g"};
    CCircularBuffer<std::string> copy_of_b(b);
    EXPECT_EQ(a, copy_of_a);
    EXPECT_EQ(b, copy_of_b);
    swap(a, b);
    EXPECT_EQ(a, copy_of_b);
    EXPECT_EQ(b, copy_of_a);
}

TEST(CircularSequenceContainer, ConstructorNCopyTTest) {
    CCircularBuffer<std::string> a(5, "abcd");
    CCircularBuffer<std::string> b{"abcd", "abcd", "abcd", "abcd", "abcd"};
    EXPECT_EQ(a, b);
    EXPECT_EQ(5, std::distance(a.begin(), a.end()));
}

TEST(CircularSequenceContainer, ConstructorFromIteratorsTest) {
    std::vector<std::string> v{"a", "b", "c", "d", "e", "f"};
    CCircularBuffer<std::string> a{"c", "d", "e"};
    CCircularBuffer<std::string> b(v.begin() + 2, v.end() - 1);
    EXPECT_EQ(a, b);
    EXPECT_EQ(std::distance(v.begin() + 2, v.end() - 1),
              std::distance(b.begin(), b.end()));
}

TEST(CircularSequenceContainer, AssigmentWithInitilizerListTest) {
    CCircularBuffer<std::string> a{"c", "d", "e"};
    CCircularBuffer<std::string> b;
    b = a;
    EXPECT_EQ(a, b);
}

TEST(CircularSequenceContainer, AssigmentWithInitiListTest) {
    CCircularBuffer<std::string> a{"c", "d", "e"};
    CCircularBuffer<std::string> b;
    b = {"c", "d", "e"};
    EXPECT_EQ(a, b);
}

TEST(CircularSequenceContainer, EmplaceTest) {
    const size_t kcapacity_of_a = 8;
    CCircularBuffer<std::string> a{"c", "k", "e", "f"};
    CCircularBuffer<std::string> b(kcapacity_of_a);
    b.emplace(b.cbegin(), "c");
    b.emplace(b.cbegin() + 1, "k");
    b.emplace(b.cbegin() + 2, "e");
    b.emplace(b.cbegin() + 3, "f");
    EXPECT_EQ(a, b);
}

TEST(CircularSequenceContainer, InsertTest) {
    const size_t kcapacity_of_a = 10;
    std::vector<std::string> v{"f", "a"};
    CCircularBuffer<std::string> a{"c", "k", "k", "f", "a"};
    CCircularBuffer<std::string> b(kcapacity_of_a);
    b.insert(b.cbegin(), "c");
    b.insert(b.cbegin() + 1, 2, "k");
    EXPECT_EQ("f", *b.insert(b.cbegin() + 3, v.begin(), v.end()));
    EXPECT_EQ(a, b);
}

TEST(CircularSequenceContainer, EraseTest) {
    const size_t kcapacity_of_a = 10;
    CCircularBuffer<std::string> a{"c", "k", "k", "f", "a"};
    CCircularBuffer<std::string> b(kcapacity_of_a);
    b.push_back("c");
    b.push_back("a");
    a.erase(a.cbegin() + 1);
    a.erase(a.cbegin() + 1, a.cend() - 1);
    EXPECT_EQ(a, b);
}

TEST(CircularSequenceContainer, ClearTest) {
    const size_t kcapacity_of_a = 10;
    CCircularBuffer<std::string> a{"c", "k", "k", "f", "a"};
    CCircularBuffer<std::string> b(kcapacity_of_a);
    a.clear();
    EXPECT_EQ(a, b);
}

TEST(CircularSequenceContainer, AssignTest) {
    const size_t kcapacity_of_a = 10;
    std::vector<std::string> v{"c", "k", "k", "f", "a"};

    CCircularBuffer<std::string> a{"a", "b", "c", "d", "e"};
    CCircularBuffer<std::string> b{"c", "k", "k", "f", "a"};
    CCircularBuffer<std::string> c(13, "a");

    a.assign(v.begin(), v.end());
    EXPECT_EQ(a, b);

    a.assign(13, "a");
    EXPECT_EQ(a, c);
}

TEST(CircularSequenceContainer, FrontBackTest) {
    const size_t kcapacity_of_a = 10;
    CCircularBuffer<std::string> a{"c", "k", "k", "f", "a"};

    EXPECT_EQ(a.front(), *(a.begin()));
    EXPECT_EQ(a.back(), *(a.end() - 1));
}

TEST(CircularSequenceContainer, EmplaceFrontBackTest) {
    const size_t kcapacity_of_a = 4;
    CCircularBuffer<std::string> a{"c", "k"};
    CCircularBuffer<std::string> b(kcapacity_of_a);
    b.emplace_front("c");
    b.emplace_back("k");

    EXPECT_EQ(a, b);
}

TEST(CircularSequenceContainer, PushFrontBackTest) {
    const size_t kcapacity_of_a = 8;
    CCircularBuffer<std::string> a{"c", "k", "a", "b"};
    CCircularBuffer<std::string> b(kcapacity_of_a);
    std::string s = "k";
    b.push_front(s);
    b.push_front("c");
    b.push_back("a");
    s = "b";
    b.push_back(s);

    EXPECT_EQ(a, b);
}

TEST(CircularSequenceContainer, PopFrontBackTest) {
    const size_t kcapacity_of_a = 8;
    CCircularBuffer<std::string> a{"c", "k", "a", "b"};
    CCircularBuffer<std::string> b(kcapacity_of_a);

    b.push_back("k");
    b.push_back("a");

    a.pop_front();
    a.pop_back();

    EXPECT_EQ(a, b);
}

TEST(CircularIterator, SortTest) {
    CCircularBuffer<int> a{1, 1, 2, 3, 4, 5, 6, 7, 7};
    CCircularBuffer<int> b{7, 6, 5, 7, 1, 4, 3, 2, 1};

    std::sort(b.begin(), b.end(), std::less_equal<int>());
    EXPECT_EQ(a, b);
}

TEST(CircularIterator, OperatorPlusMinusTest) {
    CCircularBuffer<int> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    EXPECT_EQ(true, (a.begin() + 20) - 15 == a.begin() + 5);
    EXPECT_EQ(true, (a.end() - 20) + 15 == a.end() - 5);
}

TEST(CircularIterator, OperatorMinusTest) {
    CCircularBuffer<int> a{1, 1, 2, 3, 4, 5, 6, 7, 7};

    EXPECT_EQ(9, a.end() - a.begin());
    EXPECT_EQ(8, a.end() - 1 - a.begin());
    EXPECT_EQ(7, a.end() - 2 - a.begin());
    EXPECT_EQ(6, a.begin() + 6 - a.begin());
    EXPECT_EQ(-9, a.begin() - a.end());
    EXPECT_EQ(-6, a.begin() - (a.begin() + 6));
}

TEST(CircularIterator, OperatorPlusAsignTest) {
    CCircularBuffer<int> a{1, 1, 2, 3, 4, 5, 6, 7, 7};
    CCircularBuffer<int>::iterator i = a.begin();
    i += 4;

    EXPECT_EQ(i, a.end() - 5);
}

TEST(ExtendedCircularSequenceContainer, DoubleUpTest) {
    CCircularBufferExt<std::string> a{"c", "k", "a", "b"};
    CCircularBufferExt<std::string> b{"c", "k", "a", "b", "a", "s", "d", "f"};

    a.insert(a.cend(), {"a", "s", "d", "f", "f"});
    b.push_back("f");

    EXPECT_EQ(a, b);
}
