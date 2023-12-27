#ifndef LatexrateC_H_
#define LatexrateC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

// Define a custom function pointer type
typedef float (*Real_Function)(float);

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
void write_latex_line(FILE*, const char* line);
void gen_latex(const char* file_name_plot, const char* file_name_data, const char* func_name);
void gen_file_name(const char* func_name, char* file_name_data, char* file_name_plot);
void gen_latex_code_snip(FILE* out_file,const char* function_name, const char* file_name);
FILE* create_latex_doc(const char* name);
void write_header(FILE* file);
void write_load_plot(FILE* file, const char* file_name_plot );
void calc_and_plot(FILE* file, Named_Function function, Plot_Config plot);

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

void write_latex_line(FILE* file, const char* line)
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
  
  write_latex_line(file, "\\begin{tikzpicture}");
  sprintf(tmp, "\\begin{axis}[axis lines = middle, xlabel=$x_0$, ylabel=$y(x)$, title={%s}]", func_name);
  write_latex_line(file, tmp);
  sprintf(tmp,"\\addplot[color=red, dashed] table[meta=y] {%s};", file_name_data);
  write_latex_line(file, tmp);
  write_latex_line(file, "\\end{axis}");
  write_latex_line(file, "\\end{tikzpicture}");

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

void gen_latex_code_snip(FILE* out_file,const char* functionName, const char* file_name)
{
  FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }
  bool in_function=false;
  const int max_size = 256;
  char line[max_size];
  write_latex_line(out_file, "\\begin{verbatim}");
  while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, functionName) != NULL) {
            in_function = true;
        }

        if (in_function) {
	  write_latex_line(out_file, line);
        }

        if (strstr(line, "}") != NULL && in_function) {
            break;
        }
    }

  write_latex_line(out_file, "\\end{verbatim}");
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
  write_latex_line(file, "\\documentclass{article}");
  write_latex_line(file, "\\usepackage[utf8]{inputenc}");
  write_latex_line(file, "\\usepackage{pgfplots}");
  write_latex_line(file, "\\pgfplotsset{compat=1.18}");
  write_latex_line(file, "\\usepackage{verbatim}");
  write_latex_line(file,"\\begin{document}");
}
 
void write_load_plot(FILE* file, const char* file_name_plot )
{
  write_latex_line(file,"\\begin{center}");
  write_latex_line(file,"\\begin{minipage}{\\textwidth}");
  char tmp[256];
  sprintf(tmp, "\\input{%s}", file_name_plot);
  write_latex_line(file,tmp);
  write_latex_line(file,"\\end{minipage}");
  write_latex_line(file,"\\end{center}");
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
#endif //LatexrateC_IMPLEMENTATION
