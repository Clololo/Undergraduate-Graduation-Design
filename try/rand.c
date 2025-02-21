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
    
    // ��ʼ���������� 1 �� 32
    for (i = 0; i < NUMBERS; i++) {
        numbers[i] = i + 1;
    }

    // ���������������
    srand(time(NULL));
    for (i = NUMBERS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // ���� numbers[i] �� numbers[j]
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }

    // �����Һ�����ַ��䵽4��
    for (i = 0; i < NUMBERS; i++) {
        int group_index = i / GROUP_SIZE;
        groups[group_index][i % GROUP_SIZE] = numbers[i];
    }

    // ��ӡÿ�������
    for (i = 0; i < GROUPS; i++) {
        printf("Group %d: ", i + 1);
        for (j = 0; j < GROUP_SIZE; j++) {
            printf("%d ", groups[i][j]);
        }
        printf("\n");
    }

    return 0;
}
