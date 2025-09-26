#include "find_min_max.h"
#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
    struct MinMax mm;
    mm.min = INT_MAX;
    mm.max = INT_MIN;

    if (!array || begin >= end) return mm;

    for (unsigned int i = begin; i < end; i++) {
        if (array[i] < mm.min) mm.min = array[i];
        if (array[i] > mm.max) mm.max = array[i];
    }
    return mm;
}
