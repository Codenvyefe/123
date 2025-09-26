
#include "revert_string.h"
#include <string.h>

void RevertString(char *str) {
    if (!str) return;
    size_t i = 0, j = strlen(str);
    if (j == 0) return;
    j--; // последний индекс
    while (i < j) {
        char t = str[i];
        str[i] = str[j];
        str[j] = t;
        i++; j--;
    }
}
