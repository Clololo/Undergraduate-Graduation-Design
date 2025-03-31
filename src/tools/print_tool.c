#include <stdio.h>
#include <string.h>
#include "print_tool.h"
#include "math_tool.h"

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


void pso_save_to_csv(const char *filename, double rho[], double lambda[], int dc, int dv) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    // 写入表头
    fprintf(file, "Degree,Rho,Lambda\n");
    for (int i = 0; i <= (dv > dc ? dv : dc); i++) {
        fprintf(file, "%d,", i);
        if (i <= dc) {
            fprintf(file, "%.6f,", rho[i]);
        } else {
            fprintf(file, ",");
        }
        if (i <= dv) {
            fprintf(file, "%.6f", lambda[i]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Results saved to %s\n", filename);
}

int pegsrc_save_to_csv(const char *filename, double rho[], double lambda[], int length, int dc, int dv, int flag){
    FILE *file = fopen(filename, "w");
    int res = 0;
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return -1;
    }
    if(flag == 1){
        fprintf(file, "Rho\n");
        for(int i = 0; i < dc;i++){
            int rep = approx(rho[i]*(double)length);
            for(int j = 0;j < rep;j++){
                fprintf(file, "%d", i);
                res += i;
                fprintf(file, "\n");
            }
        }
    }
    else if(flag == 2){
        fprintf(file, "Lambda\n");
        for(int i = 0; i < dv;i++){
            int rep = approx(lambda[i]*(double)length);
            for(int j = 0;j < rep; j++){
                fprintf(file, "%d", i);
                res += i;
                fprintf(file, "\n");
            }
        }
    }
    return res;
}


int read_csv_column(const char* filename, int* data, int size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error: Unable to open file");
        return -1;  // 返回错误码 -1，表示打开文件失败
    }

    char line[2048];
    int count = 0;

    // 跳过表头
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 0;  // 如果没有数据，返回 0
    }

    int i = 0;
    // 读取数据并存储到数组
    while (fgets(line, sizeof(line), file) && i < size) {
        int value;
        if (sscanf(line, "%d", &value) == 1) {
            data[i] = value;
            i++;
            count++;  // 记录有效数据的行数
        }
    }

    fclose(file);
    return count;  // 返回实际读取的数据行数
}

// 将数字写入 CSV 文件（文件格式：第一行是表头，第二行是数字）
void write_number_to_csv(const char *filename, int number) {
    FILE *file = fopen(filename, "w");  // 以写模式打开文件
    if (!file) {
        perror("无法打开文件");
        return;
    }
    fprintf(file, "Value\n");  // 写入表头
    fprintf(file, "%d\n", number);  // 写入数值
    fclose(file);
}

// 从 CSV 文件读取数字（假设数字在第一列第二行）
int read_number_from_csv(const char *filename, int *number) {
    FILE *file = fopen(filename, "r");  // 以读模式打开文件
    if (!file) {
        perror("无法打开文件");
        return -1;
    }
    
    char header[100];  // 用于存储表头
    if (fgets(header, sizeof(header), file) == NULL) {  // 读取表头
        perror("读取表头失败");
        fclose(file);
        return -1;
    }
    
    if (fscanf(file, "%d", number) != 1) {  // 读取数值
        perror("读取数值失败");
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return 0;  // 读取成功
}

#define HEADER "SNR(dB),BER_OPT,BER,FER_OPT,FER,ITER_OPT,ITER\n"

int append_performance_data(const char* filename, 
    double snr_db, 
    double ber_opt,
    double ber,
    double fer_opt,
    double fer,
    double iter_opt,
    double iter) {
    
    // 先检查文件是否已有标题
    FILE* file = fopen(filename, "r");  // 以只读模式检查
    int has_header = 0;

    if (file) {  // 文件存在
        char first_line[256];
        if (fgets(first_line, sizeof(first_line), file)) { 
            // 读取第一行并检查是否与 HEADER 相同
            if (strstr(first_line, "SNR(dB)") != NULL) {
                has_header = 1;
            }
        }
        fclose(file);
    } 

    // 以追加模式打开文件
    file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // 如果没有标题，先写入标题
    if (!has_header) {
        fprintf(file, "%s", HEADER);
    }

    // 追加写入数据
    int result = fprintf(file, "%.2f,%.2e,%.2e,%.2e,%.2e,%.2e,%.2e\n", 
        snr_db, ber_opt, ber, fer_opt, fer, iter_opt, iter);

    if (result < 0) {
        perror("Error writing to file");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}