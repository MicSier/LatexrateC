#ifndef LatexrateC_H_
#define LatexrateC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

// Define a custom function pointer type
typedef float (*Real_Function)(float);

typedef bool (*Predicate)(Real_Function,float);

typedef struct {
  Real_Function func;
  const char* name;
} Named_Function;
  
typedef struct {
  float *xs;
  float *ys; 
  size_t size;
} Calc_Result;

typedef struct {
  float a;
  float b;
} Axis_Config;

typedef struct {
  int grid_count;
  Axis_Config x_axis;
  Axis_Config y_axis;
} Plot_Config;

Calc_Result calc(Real_Function func, float a, float b, size_t n);
bool save(Calc_Result a, const char* name);
void write_line(FILE* file, const char* line);
void gen_name(const char* file_name_plot, const char* file_name_data, const char* func_name);
void gen_file_name(const char* func_name, char* file_name_data, char* file_name_plot);
void gen_codev_snip(FILE* out_file,const char* function_name, const char* file_name);
void gen_code_snip(FILE* out_file, const char* function_name, const char* file_name);
FILE* create_doc(const char* name);
void write_header(FILE* file);
void write_load_plot(FILE* file, const char* file_name_plot );
void calc_and_plot(FILE* file, Named_Function function, Plot_Config plot);
char* str_int(const char* format, ...);
void run_test(FILE *doc,Named_Function f, float test_x, Predicate p);
#endif // LatexrateC_H_

#ifdef LatexrateC_IMPLEMENTATION

Calc_Result calc(Real_Function func, float a, float b, size_t n)
{
  Calc_Result arr;
  arr.xs = (float*)malloc(n * sizeof(*(arr.xs)));
  arr.ys = (float*)malloc(n * sizeof(*(arr.ys)));
  arr.size = n;
  
  for(size_t i=0;i<n;i++){
     float x=a+i*(b-a)/(n-1);
     float y=func(x);
     arr.xs[i]=x;
     arr.ys[i]=y;
     }

   return arr;
}

bool save_calc_result(Calc_Result a, const char* name)
{
    FILE *filePointer;
    // Open the file for writing
    filePointer = fopen(name, "w");

    // Check if the file was opened successfully
    if (filePointer == NULL) {
      perror("Error opening file");
      goto error; // Jump to the cleanup label
    }

    // Write data to the file
    fprintf(filePointer, "x y\n");
    
    for(size_t i=0;i<a.size;i++)
      fprintf(filePointer, "%f %f\n", a.xs[i], a.ys[i]);

    // Close the file    
    fclose(filePointer);
    return 0;
    
  error:
    // Perform cleanup
    if (filePointer != NULL) {
      fclose(filePointer);
    }

    return 1; // Indicate an error
}

void write_line(FILE* file, const char* line)
{
  fprintf(file, "%s\n", line);
}

void gen_latex(const char* file_name_plot, const char* file_name_data, const char* func_name)
{
  FILE *file = fopen(file_name_plot, "w");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }
  char tmp[256];
  
  write_line(file, "\\begin{tikzpicture}");
  sprintf(tmp, "\\begin{axis}[axis lines = middle, xlabel=$x_0$, ylabel=$y(x)$, title={%s}]", func_name);
  write_line(file, tmp);
  sprintf(tmp,"\\addplot[color=red, dashed] table[meta=y] {%s};", file_name_data);
  write_line(file, tmp);
  write_line(file, "\\end{axis}");
  write_line(file, "\\end{tikzpicture}");

  fclose(file);
}

void gen_file_name(const char* func_name, char* file_name_data, char* file_name_plot)
{
  const char* data_file_extension = ".txt";
  const char* plot_file_extension = ".tex";
  const char* path = "../build/"; 
  // Assuming 'func_name' is a null-terminated string
  snprintf(file_name_data, 256, "%s%s%s", path, func_name, data_file_extension);
  snprintf(file_name_plot, 256, "%s%s%s", path, func_name, plot_file_extension);
}

void gen_code_snip(FILE* out_file,const char* function_name, const char* file_name)
{
  write_line(out_file, "\\begin{verbatim}");
  gen_codev_snip(out_file, function_name, file_name);
  write_line(out_file, "\\end{verbatim}");
}

void gen_codev_snip(FILE* out_file, const char* function_name, const char* file_name)
{
 FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }
  bool in_function=false;
  const int max_size = 256;
  char line[max_size];

  while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, function_name) != NULL) {
            in_function = true;
        }

        if (in_function) {
	  write_line(out_file, line);
        }

        if (strstr(line, "}") != NULL && in_function) {
            break;
        }
    }
  fclose(file);
}

FILE* create_latex_doc(const char* name)
{
    FILE *file = fopen(name, "w");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }
  return file;
}

void write_header(FILE* file)
{
  write_line(file, "\\documentclass{article}");
  write_line(file, "\\usepackage[utf8]{inputenc}");
  write_line(file, "\\usepackage{pgfplots}");
  write_line(file, "\\pgfplotsset{compat=1.18}");
  write_line(file, "\\usepackage{verbatim}");
  write_line(file,"\\begin{document}");
}
 
void write_load_plot(FILE* file, const char* file_name_plot )
{
  write_line(file,"\\begin{center}");
  write_line(file,"\\begin{minipage}{\\textwidth}");
  char tmp[256];
  sprintf(tmp, "\\input{%s}", file_name_plot);
  write_line(file,tmp);
  write_line(file,"\\end{minipage}");
  write_line(file,"\\end{center}");
}

void calc_and_plot(FILE* file, Named_Function function, Plot_Config plot)
{
    //Calculate plot data
    Calc_Result res = calc(function.func,plot.x_axis.a,plot.x_axis.b,plot.grid_count);

    //Append file extensions to function name to create name of the files
    char file_name_data[256], file_name_plot[256];
    gen_file_name(function.name, file_name_data, file_name_plot);

    //Save calculated plot data to file
    if(!save_calc_result(res, file_name_data)){
      
      //Generate seperate tex file defining plot from data file
      gen_latex(file_name_plot, file_name_data, function.name);
    }
    free(res.xs);
    free(res.ys);
    
    //Load plot from seperate file 
    write_load_plot(file, file_name_plot);
}

char* str_int(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    size_t size = vsnprintf(NULL, 0, format, args);
    char* result = (char*) malloc (size+1);
    vsprintf(result,format, args);

    va_end(args);
    return result;
}


void run_test(FILE *doc,Named_Function f, float test_x, Predicate p)
{
  float ya=f.func(test_x),yb=f.func(-test_x);
  bool test_result = p(f.func,test_x);
  char* test_text = str_int("\\[%s(%.2f)= %.4f\\] and \\[%s(%.2f)=%.4f\\], and therefore \\[%s(%.2f)%s%s(%.2f)\\] so the function %s the test.",f.name,test_x,ya,f.name,-test_x,yb,f.name,test_x,(test_result)?"=":"\\neq ",f.name,-test_x,(test_result)?"passes":"failes");
    
  write_line(doc,test_text); 
}
#endif //LatexrateC_IMPLEMENTATION
