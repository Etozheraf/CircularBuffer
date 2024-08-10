#include<iostream>
#include<vector>
#include"lib/CCircularBuffer.h"

//class Print {
//public:
//    Print(char* s) {
//        std::cout << s;
//    }
//};
//
//class Check {
//public:
//    Check() : i("Hello"), d(" I'm"), v(" working"), c(" norm") { std::cout << "\nCheck()"; }
//
//private:
//    Print i;
//    Print d;
//    Print v;
//    Print c;
//};

//template<typename T>
//class Check {
//public:
//    void check(const T& t) {
//        print(std::forward<T>(t));
//    }
//    void check(T&& t) {
//        print(std::move(t));
//    }
//
//
//    void print(const T& t) { std::cout << "Copy called\n"; }
//
//    void print(T&& t) { std::cout << "Move called\n"; }
//};

int main() {
    std::vector<int> v;
    v.pop_back();
    return 0;
}