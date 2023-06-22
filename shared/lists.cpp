#include "lists.h"

std::vector<Ball *> Lists::balls;

void Lists::clearBalls() {
    int i = 0;
    
    while(i < balls.size()) {
        delete balls[i];
        i++;
    }

    balls.clear();
    
    return;
}