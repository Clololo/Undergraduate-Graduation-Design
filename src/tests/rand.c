#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUMBERS 32
#define GROUPS 4
#define GROUP_SIZE 8

int main() {
    int numbers[NUMBERS];
    int groups[GROUPS][GROUP_SIZE];
    int i, j;
    
    // 初始化数字数组 1 到 32
    for (i = 0; i < NUMBERS; i++) {
        numbers[i] = i + 1;
    }

    // 随机打乱数字数组
    srand(time(NULL));
    for (i = NUMBERS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // 交换 numbers[i] 和 numbers[j]
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }

    // 将打乱后的数字分配到4组
    for (i = 0; i < NUMBERS; i++) {
        int group_index = i / GROUP_SIZE;
        groups[group_index][i % GROUP_SIZE] = numbers[i];
    }

    // 打印每组的数字
    for (i = 0; i < GROUPS; i++) {
        printf("Group %d: ", i + 1);
        for (j = 0; j < GROUP_SIZE; j++) {
            printf("%d ", groups[i][j]);
        }
        printf("\n");
    }

    return 0;
}
