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


// �����ֶο��
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

// ��ӡ��ʽ����������
void printFormattedLine(Parameters params, int width) {
    // ������ֶο��
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

    // ��ӡ�е���ʼ�߽�
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

    // �������հ׵�����
    int currentLength = 2 + int1Width + int2Width + float1Width + float2Width +
                        int3Width + int4Width + int5Width +
                        float4Width + float5Width + float6Width + float7Width;
    int paddingLength = width - currentLength -1;

    // ��ӡ���հ�
    for (int i = 0; i < paddingLength; i++) {
        printf(" ");
    }

    // ��ӡ�еĽ����߽�
    printf("|\n");
}


void pso_save_to_csv(const char *filename, double rho[], double lambda[], int dc, int dv) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    // д���ͷ
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
        return -1;  // ���ش����� -1����ʾ���ļ�ʧ��
    }

    char line[2048];
    int count = 0;

    // ������ͷ
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 0;  // ���û�����ݣ����� 0
    }

    int i = 0;
    // ��ȡ���ݲ��洢������
    while (fgets(line, sizeof(line), file) && i < size) {
        int value;
        if (sscanf(line, "%d", &value) == 1) {
            data[i] = value;
            i++;
            count++;  // ��¼��Ч���ݵ�����
        }
    }

    fclose(file);
    return count;  // ����ʵ�ʶ�ȡ����������
}

// ������д�� CSV �ļ����ļ���ʽ����һ���Ǳ�ͷ���ڶ��������֣�
void write_number_to_csv(const char *filename, int number) {
    FILE *file = fopen(filename, "w");  // ��дģʽ���ļ�
    if (!file) {
        perror("�޷����ļ�");
        return;
    }
    fprintf(file, "Value\n");  // д���ͷ
    fprintf(file, "%d\n", number);  // д����ֵ
    fclose(file);
}

// �� CSV �ļ���ȡ���֣����������ڵ�һ�еڶ��У�
int read_number_from_csv(const char *filename, int *number) {
    FILE *file = fopen(filename, "r");  // �Զ�ģʽ���ļ�
    if (!file) {
        perror("�޷����ļ�");
        return -1;
    }
    
    char header[100];  // ���ڴ洢��ͷ
    if (fgets(header, sizeof(header), file) == NULL) {  // ��ȡ��ͷ
        perror("��ȡ��ͷʧ��");
        fclose(file);
        return -1;
    }
    
    if (fscanf(file, "%d", number) != 1) {  // ��ȡ��ֵ
        perror("��ȡ��ֵʧ��");
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return 0;  // ��ȡ�ɹ�
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
    
    // �ȼ���ļ��Ƿ����б���
    FILE* file = fopen(filename, "r");  // ��ֻ��ģʽ���
    int has_header = 0;

    if (file) {  // �ļ�����
        char first_line[256];
        if (fgets(first_line, sizeof(first_line), file)) { 
            // ��ȡ��һ�в�����Ƿ��� HEADER ��ͬ
            if (strstr(first_line, "SNR(dB)") != NULL) {
                has_header = 1;
            }
        }
        fclose(file);
    } 

    // ��׷��ģʽ���ļ�
    file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // ���û�б��⣬��д�����
    if (!has_header) {
        fprintf(file, "%s", HEADER);
    }

    // ׷��д������
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