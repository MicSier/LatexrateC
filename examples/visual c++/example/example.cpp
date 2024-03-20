// example.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define LatexrateC_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "../../../LatexrateC.h"

float parabola(float x){ return x*x;}
bool is_even(Real_Function f, float x){	return (f(x) == f(-x));}


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
    Named_Function functions[func_number] = {{sinf, "Sinus"}, {cosf, "Cosinus"}, {parabola, "Parabola"}};

  //Set some plotting config
  Plot_Config plot_config = /*(Plot_Config)*/{
    /*.grid_count =*/ 100,
    /*.x_axis = (Axis_Config)*/{/*.a =*/ -4, /*.b =*/ 4},
    /*.y_axis = (Axis_Config)*/{/*.a =*/ -4, /*.b =*/ 4}
  };
  
  for(int i=0;i<func_number;i++){
    calc_and_plot(doc, functions[i], plot_config, "");
  }

  //Begin section
  write_line(doc,"\\section{Code}");
  write_line(doc,"In this section we will show code snipet showing implementation of function used to calculate parabola for this example.");  
  //Write code from file to latex document
  gen_code_snip(doc, "parabola", "../../example.cpp");

  write_line(doc,"Let's run some tests on our functions to see if it has properties some interesting properties.");
  write_line(doc, "Function is called even, if $f(x)=f(-x)$.");
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
