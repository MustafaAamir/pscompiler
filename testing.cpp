#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <vector>
#include <variant>
#include <memory>
using namespace std;

using Value = std::variant<int*, std::string, char, int>;
typedef struct ValueArray {
    int lb;
    int ub;
    Value *arr;
    ValueArray() = default;
    ValueArray(int lb, int ub, Value *arr) : lb(lb), ub(ub), arr(arr) {}
} ValueArray;

void init_array() {
    int ub = 10;
    int uby = 10;
    Value *arr = (Value *) malloc(sizeof(Value) * ub * uby);
    auto array = new ValueArray(1, 2, arr);
    for (int i = 0; i <= 100; i++) {
        array->arr[i] = i;
    }
    for (int i = 0; i <= 100; i++) {
        printf("[%d]", get<int>(array->arr[i]));
    }

    printf("\n\n\\n");
    printf("[%d]", get<int>(array->arr[10 * 1]));
    printf("[%d]", get<int>(array->arr[10 * 2]));
    printf("[%d]", get<int>(array->arr[10 * 3]));
    printf("[%d]", get<int>(array->arr[10 * 4]));
    printf("[%d]", get<int>(array->arr[10 * 5]));
    free(array->arr);
}

int main(void)  {
    init_array();
    return 0;
}
