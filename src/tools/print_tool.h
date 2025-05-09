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
void pso_save_to_csv(const char *filename, double rho[], double lambda[], int dc, int dv);
int pegsrc_save_to_csv(const char *filename, double rho[], double lambda[], int length, int dc, int dv, int flag);
int read_csv_column(const char* filename, int* data, int size);
void write_number_to_csv(const char *filename, int number);
int read_number_from_csv(const char *filename, int *number);
int append_performance_data(const char* filename, 
    double snr_db, 
    double ber_opt,
    double ber,
    double fer_opt,
    double fer,
    double iter_opt,
    double iter) ;
void save_pop_to_csv(double snr, const double *pop, int length, int pso_en);
void load_pop_from_csv(double snr, double *pop, int max_length, int pso_en);

#endif