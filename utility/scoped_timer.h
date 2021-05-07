//
// Created by swqd on 5/7/21.
//

#ifndef LEXER_TEST_SCOPED_TIMER_H
#define LEXER_TEST_SCOPED_TIMER_H

#include <chrono>

using namespace std::chrono;

class [[maybe_unused]] scoped_timer {
    std::chrono::time_point<steady_clock> start;
    const char *message;

public:
    scoped_timer(const char *message) : message(message) {
        start = std::chrono::steady_clock::now();
    }
    ~scoped_timer() {
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << message << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "ms" <<  std::endl;
    }
};

#endif //LEXER_TEST_SCOPED_TIMER_H
