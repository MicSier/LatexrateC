// example.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define LatexrateC_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "../../../LatexrateC.h"

float parabola(float x) { return x * x; }
bool is_even(Real_Function f, float x) { return (f(x) == f(-x)); }


int main()
{
	//Create empty file
	FILE* doc = create_latex_doc("example.tex");

	//Write standard header for article class document
	write_header(doc);

	//Begin section
	write_line(doc, "\\section{2D plots}");
	write_line(doc, "In this section we will first draw a simple plot.");

	//Pick functions to be ploted
	Named_Function function = { parabola, "Parabola" };

	//Set some plotting config
	Plot_Config plot_config = default_plot_config();
	calc_and_plot(doc, &function, plot_config, "");

	write_line(doc, "Next we will draw a graph containing two plots with diffrent colors and line styles.");

	Named_Function functions[2] = { {sinf, "sin(x)"}, {cosf, "cos(x)"} };
	plot_config.plot_count = 2;
	Line_Color line_color[] = { COLOR_RED, COLOR_BLUE };
	Line_Style line_style[] = { STYLE_DASHED, STYLE_SOLID };
	plot_config.line_color = line_color;
	plot_config.line_style = line_style;
	plot_config.description = "Plot of sin and cos functions.";
	plot_config.title = "sin(x) and cos(x)";
	calc_and_plot(doc, functions, plot_config, "");

	

	//Begin section
	write_line(doc, "\\section{Code}");
	write_line(doc, "In this section we will show code snipet showing implementation of function used to calculate parabola for this example.");
	//Write code from file to latex document
	gen_code_snip(doc, "parabola", "../../example.cpp");

	write_line(doc, "Let's run some tests on our functions to see if it has properties some interesting properties.");
	write_line(doc, "Function is called even, if $f(x)=f(-x)$.");
	float x = 1.63;
	write_line(doc, str_int("We can check if they are even for some specific argument lets say x=%1.2f.", x));

	for (int i = 0; i < 2; i++) {
		run_test(doc, functions[i], x, is_even);
	}

	//Begin section
	write_line(doc, "\\section{Equations}");
	write_line(doc, "\\[ f(x) = x^2\\]");

	//End document
	write_line(doc, "\\end{document}");
	return 0;
}
