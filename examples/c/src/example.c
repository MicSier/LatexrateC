#ifdef RUN
#define LatexrateC_IMPLEMENTATION
#include "../../../LatexrateC.h"

float parabola(float x)
{
  return x*x;
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
  write_latex_line(file,"\\begin{minipage}{\\textwidth}");
  char tmp[256];
  sprintf(tmp, "\\input{%s}", file_name_plot);
  write_latex_line(file,tmp);
  write_latex_line(file,"\\end{minipage}");
}

void calc_and_plot(FILE* file, Named_Function function)
{
    //TO DO: expose this and other options outside function 
    float a=-4.0, b=4.0;
    size_t n=100;

    //Calculate plot data
    Calc_Result res = calc(function.func,a,b,n);

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
 
int main()
{
  //Create empty file
  FILE* doc = create_latex_doc("example.tex");
  
  //Write standard header for article class document
  write_header(doc);

  //Begin section
  write_latex_line(doc,"\\section{Code}");
  
  //Write code from file to latex document
  gen_latex_code_snip(doc, "../src/example.c");
  
  //Begin section
  write_latex_line(doc,"\\section{2D plots}");

  //Pick functions to be ploted
  #define func_number 3
  Named_Function functions[func_number] = {{sinf, "Sinus"}, {cosf, "Cosinus"}, {parabola, "Parabola"}};

  for(int i=0;i<func_number;i++){
    calc_and_plot(doc, functions[i]);
  }
  
  //End document
  write_latex_line(doc,"\\end{document}");
  return 0;
}


#else

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define NOB_IMPLEMENTATION
#include "../../../nob.h"

bool build_example()
{
    bool result = true;
    Nob_Cmd cmd = {0};
    
    cmd.count = 0;
    nob_cmd_append(&cmd, "gcc");
    nob_cmd_append(&cmd, "-Wall", "-Wextra", "-g");
    nob_cmd_append(&cmd, "-o", "./example");
    nob_cmd_append(&cmd, "../src/example.c");
    nob_cmd_append(&cmd, "-DRUN");
    if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);

defer:
    nob_cmd_free(cmd);
    return result;
}

bool run_pdflatex()
{
    bool result = true;
    Nob_Cmd cmd = {0};

    cmd.count = 0;
    nob_cmd_append(&cmd, "pdflatex");
    nob_cmd_append(&cmd, "-output-directory", "../build");
    nob_cmd_append(&cmd, "../build/example.tex");
    
    if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);

defer:
    nob_cmd_free(cmd);
    return result;
}

bool run_example()
{
    bool result = true;
    Nob_Cmd cmd = {0};

    cmd.count = 0;
    nob_cmd_append(&cmd, "./example.exe");
    if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);

defer:
    nob_cmd_free(cmd);
    return result;
}


int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    const char *program = nob_shift_args(&argc, &argv);

    if (!build_example()) return 1;
    if (!run_example()) return 1;
    if (!run_pdflatex()) return 1;

    return 0;
}
#endif //RUN
