#include "swap.h"

void Swap(char *left, char *right) {
    if (!left || !right) return;  // защита от NULL
    char t = *left;
    *left = *right;
    *right = t;
}
