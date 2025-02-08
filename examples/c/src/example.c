#ifdef RUN
#define LatexrateC_IMPLEMENTATION
#include "../../../LatexrateC.h"

// Macro to generate function definition code with expression as a string
//#define CREATE_FUNCTIONEQUATION (text, expression, result_type, function_name, parameter_list) \
    result_type function_name parameter_list { return (expression);}\
    write_line(doc, str_int(text, #expression)); 

#define CREATE_FUNCTIONEQUATION(text, expression, result_type, function_name, ...) \
    result_type function_name(__VA_ARGS__) { return (expression); } \
    write_line(doc, str_int(text, #expression));

int main()
{
  
  //Create empty file
  FILE* doc = create_latex_doc("example.tex");
  
  //Write standard header for article class document
  write_header(doc);
  
  //Begin section
  write_line(doc,"\\section{2D plots}");
  write_line(doc,"In this section we will draw plot.");
  
  //Pick function to be ploted
  float parabola(float x){ return x*x;}
  Named_Function function = {{parabola, "Parabola"}};

  //Set some plotting config
  Plot_Config plot_config = default_plot_config();
  calc_and_plot(doc, &function, plot_config, "");

	write_line(doc, "Next we will draw a graph containing two plots with diffrent colors and line styles.");

	Named_Function functions[2] = { {sinf, "sin(x)"}, {cosf, "cos(x)"} };
	plot_config.plot_count = 2;
	enum Line_Color line_color[] = { COLOR_RED, COLOR_BLUE };
	enum Line_Style line_style[] = { STYLE_DASHED, STYLE_SOLID };
	plot_config.line_color = line_color;
	plot_config.line_style = line_style;
	plot_config.description = "Plot of sin and cos functions.";
	plot_config.title = "sin(x) and cos(x)";
	calc_and_plot(doc, functions, plot_config, "");

	write_line(doc, "Let's now draw a simple 3D plot.");
  CREATE_FUNCTIONEQUATION ("Function we will draw is defined as %s.", x * x - y * y, float, saddle, float x, float y); 
	Named_Function_3D function_3D = { saddle,"saddle(x,y)" };
	Plot_Config_3D plot_config_3D = default_plot_config_3D();
	plot_config_3D.title = "saddle";
	calc_and_plot_3D(doc, &function_3D, plot_config_3D, "");


  //Begin section
  write_line(doc,"\\section{Code}");
  write_line(doc,"In this section we will show code snipet showing implementation of function used to calculate parabola for this example.");  
  //Write code from file to latex document
  gen_code_snip(doc, "parabola", "../src/example.c");

  write_line(doc,"Let's run some tests on our functions to see if it has properties some interesting properties.");
  
  CREATE_FUNCTIONEQUATION ("Function is called symmetrical, if %s.", f(x) == f(-x), bool, is_symmetrical, Real_Function f, float x); 

  float x= 1.63;
  write_line(doc,str_int("We can check if they are symmetrical for some specific argument lets say x=%1.2f.",x));

  for(int i=0;i<2;i++){
    run_test(doc,functions[i],x,is_symmetrical);
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
    nob_cmd_append(&cmd, "-Wall", "-Wextra");
    nob_cmd_append(&cmd, "-o", "example.exe");
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
    nob_cmd_append(&cmd, "example.exe");
    if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);

defer:
    nob_cmd_free(cmd);
    return result;
}

bool print_compiler_version()
{
    bool result = true;
    Nob_Cmd cmd = {0};

    cmd.count = 0;
    nob_cmd_append(&cmd, "gcc", "--version");
    if (!nob_cmd_run_sync(cmd)) nob_return_defer(false);

defer:
    nob_cmd_free(cmd);
    return result;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    const char *program = nob_shift_args(&argc, &argv);

    //if (!print_compiler_version()) return 1;
    if (!build_example()) return 1;
    if (!run_example()) return 1;
    if (!run_pdflatex()) return 1;

    return 0;
}
#endif //RUN
