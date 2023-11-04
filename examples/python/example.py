from LatexrateC import wraper as L
import math

# Define a parabola function for your calculations
def parabola(x):
    return x * x

# Function to create a LaTeX document
def create_latex_doc(name):
    file = open(name, "w")
    return file

# Function to write the LaTeX document header
def write_header(file):
    file.write("\\documentclass{article}\n")
    file.write("\\usepackage[utf8]{inputenc}\n")
    file.write("\\usepackage{pgfplots}\n")
    file.write("\\pgfplotsset{compat=1.18}\n")
    file.write("\\begin{document}\n")

# Function to write LaTeX code to load a plot
def write_load_plot(file, file_name_plot):
    file.write("\\begin{minipage}{\\textwidth}\n")
    file.write(f"\\input{{{file_name_plot}}}\n")
    file.write("\\end{minipage}\n")
    
def main():
    # Create an empty LaTeX file
    doc = create_latex_doc("example.tex")

    # Write the standard header for the LaTeX document
    write_header(doc)

    # Begin the "2D plots" section
    doc.write("\\section{2D plots}\n")

    # Define functions to be plotted
    functions = [
        (lambda x: math.sin(x), "Sinus"),
        (lambda x: math.cos(x), "Cosinus"),
        (parabola, "Parabola")
    ]

    for func, func_name in functions:
        # Define function parameters
        a, b = -4.0, 4.0
        n = 100

        # Calculate plot data using your C library
        xs, ys = L.calculate_plot_data(func, a, b, n)

        # Generate file names
        file_name_data, file_name_plot = L.generate_file_names(func_name)

        # Save calculated plot data to a file
        L.save_plot_data((xs, ys), file_name_data)

        # Generate a separate TeX file defining the plot from the data file
        L.generate_latex(file_name_plot, file_name_data, func_name)

        # Load the plot from the separate file
        write_load_plot(doc, file_name_plot)

    # End the LaTeX document
    doc.write("\\end{document}\n")
    doc.close()

if __name__ == "__main__":
    main()
