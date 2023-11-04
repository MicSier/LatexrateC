#define LatexrateC_IMPLEMENTATION
#include "..\..\LatexrateC.h"

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
 
int main()
{
  //Create empty file
  FILE* doc = create_latex_doc("example.tex");
  
  //Write standard header for article class document
  write_header(doc);

  //Begin section
  write_latex_line(doc,"\\section{2D plots}");

  //Pick functions to be ploted
  #define func_number 3
  Named_Function functions[func_number] = {{ sinf, "Sinus"}, {cosf, "Cosinus"}, {parabola, "Parabola"}};

  for(int i=0;i<func_number;i++){
    //Define 
    float a=-4.0, b=4.0;
    size_t n=100;

    //Calculate plot data
    Calc_Result res = calc(functions[i].func,a,b,n);

    //Append file extensions to function name to create name of the files
    char file_name_data[256], file_name_plot[256];
    gen_file_name(functions[i].name, file_name_data, file_name_plot);

    //Save calculated plot data to file
    if(!save_calc_result(res, file_name_data)){
      
      //Generate seperate tex file defining plot from data file
      gen_latex(file_name_plot, file_name_data, functions[i].name);
    }
    
    //Load plot from seperate file 
    write_load_plot(doc, file_name_plot);
    free(res.xs);
    free(res.ys);
  }
  
  //End document
  write_latex_line(doc,"\\end{document}");
  return 0;
}
