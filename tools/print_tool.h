#ifndef PRINT_TOOL_H
#define PRINT_TOOL_H

#define WIDTH 90   // 方框的宽度
#define HEIGHT 4   // 方框的高度
// 定义参数结构体
typedef struct {
    int int1, int2;
    float float1, float2;
    int int3, int4, int5;
    float float4, float5;
    float float6, float7;
} Parameters;

void printHorizontalLine(int width, char borderChar, char fillChar);
void printRow(int width, const char *leftText, const char *rightText);
void printFormattedLine(Parameters params, int width);
void pso_save_to_csv(const char *filename, double rho[], double lambda[], int dv, int dc);

#endif