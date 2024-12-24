#include <stdio.h>
#include <string.h>
#include "print_tool.h"


void printHorizontalLine(int width, char borderChar, char fillChar) {
    for (int i = 0; i < width; i++) {
        if (i == 0 || i == width - 1) {
            printf("%c", borderChar);
        } else {
            printf("%c", fillChar);
        }
    }
    printf("\n");
}

void printRow(int width, const char *leftText, const char *rightText) {
    printf("| %s", leftText);
    for (int i = 0; i < (width - 2 - (int)strlen(leftText) - (int)strlen(rightText))-1; i++) {
        printf(" ");
    }
    printf("%s|\n", rightText);
}


// 计算字段宽度
int calculateFieldWidth1(int intField) {
    return snprintf(NULL, 0, "%2.d", intField) + 2; // +2 for the space around
}

int calculateFieldWidth(float floatField, char format) {
    int len = 0;
    if (format == 'e') {
        len = snprintf(NULL, 0, "%8.2e", floatField);
    } else {
        len = snprintf(NULL, 0, "%.2f", floatField);
    }
    return len + 2; // +2 for the space around
}

// 打印格式化的数字行
void printFormattedLine(Parameters params, int width) {
    // 计算各字段宽度
    //printf("%d %d %f %f %d\n",params.int1,params.int2,params.float1,params.float2,params.int3);
    int int1Width = calculateFieldWidth1(params.int1);
    int int2Width = calculateFieldWidth1(params.int2);
    int float1Width = calculateFieldWidth(params.float1, 'f');
    int float2Width = calculateFieldWidth(params.float2, 'f');
    int int3Width = calculateFieldWidth1(params.int3);
    int int4Width = calculateFieldWidth1(params.int4);
    int int5Width = calculateFieldWidth1(params.int5);
    int float4Width = calculateFieldWidth(params.float4, 'e');
    int float5Width = calculateFieldWidth(params.float5, 'e');
    int float6Width = calculateFieldWidth(params.float6, 'e');
    int float7Width = calculateFieldWidth(params.float7, 'e');

    // 打印行的起始边界
    printf("| %*d/%*d   %.2f/%.2f  %*d  %*d  %*d   %*.2e  %*.2e  %*.2e  %*.2e",
        int1Width - 2, params.int1, 
        int2Width - 2, params.int2,
        params.float1, 
        params.float2, 
        int3Width - 2, params.int3, 
        int4Width - 2, params.int4,
        int5Width - 2, params.int5,
        float4Width - 2, params.float4, float5Width - 2, params.float5,
        float6Width - 2, params.float6, float7Width - 2, params.float7
    );

    // 计算填充空白的数量
    int currentLength = 2 + int1Width + int2Width + float1Width + float2Width +
                        int3Width + int4Width + int5Width +
                        float4Width + float5Width + float6Width + float7Width;
    int paddingLength = width - currentLength -1;

    // 打印填充空白
    for (int i = 0; i < paddingLength; i++) {
        printf(" ");
    }

    // 打印行的结束边界
    printf("|\n");
}
