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

enum Line_Color
{
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE
};

enum Line_Style
{
    STYLE_SOLID,
    STYLE_DASHED
};


typedef struct {
  int grid_count;
  Axis_Config x_axis;
  Axis_Config y_axis;
  unsigned short plot_count;
  Line_Color* line_color;
  Line_Style* line_style;
} Plot_Config;

const char* to_color_name(Line_Color line_color);
const char* to_style_name(Line_Style line_style);
Plot_Config default_plot_config();
bool calc_and_save(Named_Function* functions, Plot_Config plot);
void write_line(FILE* file, const char* line);
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

const char* to_color_name(Line_Color line_color)
{
    switch (line_color)
    {
    case COLOR_RED: return "red";
    case COLOR_GREEN: return "green";
    case COLOR_BLUE: return "blue";
    default: return "black";
    }
}

const char* to_style_name(Line_Style line_style)
{
    switch (line_style)
    {
    case STYLE_SOLID: return "solid";
    case STYLE_DASHED: return "dashed";
    default: return "solid";
    }
}

Plot_Config default_plot_config()
{
    Line_Color line_color[] = { COLOR_RED };
    Line_Style line_style[] = { STYLE_SOLID };

/*   line_color = (Line_Color*)malloc(sizeof(line_color));
    line*/
    Plot_Config plot_config = {
        .grid_count = 100,
        .x_axis = {.a = -5, .b = 5},
        .y_axis = {.a = -5, .b = 5},
        .plot_count = 1,
        .line_color = line_color,
        .line_style = line_style
  };
    return plot_config;
}

bool calc_and_save(Named_Function* functions, Plot_Config plot)
{
    FILE* filePointer;

    for (int j = 0; j < plot.plot_count; j++)
    { 
        filePointer = fopen(str_int("%s.%s",functions[j].name, "txt"), "w");

        // Check if the file was opened successfully
        if (filePointer == NULL) {
            perror("Error opening file");
            goto error; // Jump to the cleanup label
        }

        // Write data to the file
        fprintf(filePointer, "x y\n");
        for (size_t i = 0; i < plot.grid_count; i++) {
            float x = plot.x_axis.a + i * (plot.x_axis.b - plot.x_axis.a) / (plot.grid_count - 1);
            float y = functions[j].func(x);
            fprintf(filePointer, "%f %f\n", x, y);
        }
  
        fclose(filePointer);
    }
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

char* gen_latex(Named_Function* functions, Plot_Config plot_config)
{

  FILE *file = fopen(str_int("%s.%s",functions[0].name, "tex"), "w");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }
  char tmp[256];
  
  write_line(file, "\\begin{tikzpicture}");
  sprintf(tmp, "\\begin{axis}[axis lines = middle, xlabel=$x_0$, ylabel=$y(x)$, title={%s}]", functions[0].name);
  write_line(file, tmp);
  for (int i = 0; i < plot_config.plot_count; i++)
  {
    sprintf(tmp,"\\addplot[color=%s, %s] table[meta=y] {%s};", to_color_name(plot_config.line_color[i]), to_style_name(plot_config.line_style[i]), str_int("%s.%s",functions[i].name, "txt"));
    write_line(file, tmp);
  }

  write_line(file, "\\end{axis}");
  write_line(file, "\\end{tikzpicture}");

  fclose(file);

  return (char*)(functions[0].name);
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

void calc_and_plot(FILE* file, Named_Function* functions, Plot_Config plot, const char* path)
{
    //Save calculated plot data to file
    if(!calc_and_save(functions,plot)){
      
      //Generate seperate tex file defining plot from data file
      char* file_plot = gen_latex(functions, plot);
      //Load plot from seperate file 
      write_load_plot(file, file_plot);

    }    
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
