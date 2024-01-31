#ifdef RUN
#define LatexrateC_IMPLEMENTATION
#include "../../../LatexrateC.h"

int main()
{
  
  //Create empty file
  FILE* doc = create_latex_doc("example.tex");
  
  //Write standard header for article class document
  write_header(doc);
  
  //Begin section
  write_line(doc,"\\section{2D plots}");
  write_line(doc,"In this section we will draw plots of 3 functions.");
  
  //Pick functions to be ploted
  #define func_number 3
  float parabola(float x){ return x*x;}
  Named_Function functions[func_number] = {{sinf, "Sinus"}, {cosf, "Cosinus"}, {parabola, "Parabola"}};

  //Set some plotting config
  Plot_Config plot_config = (Plot_Config){
    .grid_count = 100,
    .x_axis = (Axis_Config){ .a =-4, .b =4 },
    .y_axis = (Axis_Config){ .a =-4, .b =4 }
  };
  
  for(int i=0;i<func_number;i++){
    calc_and_plot(doc, functions[i], plot_config);
  }

  //Begin section
  write_line(doc,"\\section{Code}");
  write_line(doc,"In this section we will show code snipet showing implementation of function used to calculate parabola for this example.");  
  //Write code from file to latex document
  gen_code_snip(doc, "parabola", "../src/example.c");

  write_line(doc,"Let's run some tests on our functions to see if it has properties some interesting properties.");
  write_line(doc, "Function is called even, if $f(x)=f(-x)$.");
  bool is_even(Real_Function f, float x){	return (f(x) == f(-x));}
  float x= 1.63;
  write_line(doc,str_int("We can check if they are even for some specific argument lets say x=%1.2f.",x));

  for(int i=0;i<func_number;i++){
    run_test(doc,functions[i],x,is_even);
  }

  //Begin section
  write_line(doc,"\\section{Equations}");
  write_line(doc,"\\[ f(x) = x^2\\]");

  //End document
  write_line(doc,"\\end{document}");
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
