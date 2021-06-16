//
// Created by Egor Chunaev on 16.06.2021.
//

#include <iostream>

int main(int argc, char *argv[]) {
    int x;
    while (true) {
        std::cin >> x;
        if (x == -1) {
            break;
        }
        std::cout << 0 << std::endl;
    }

    return 0;
}