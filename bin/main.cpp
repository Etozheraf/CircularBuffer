#include<iostream>
#include<vector>
#include"lib/CCircularBuffer.h"

int main() {
    CCircularBuffer c = {1, 2, 3, 4, 5};
    std::vector<int> v;

    for (int i : c)
        std::cout << i << " ";

    return 0;
}