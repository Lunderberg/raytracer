all: main derivations.pdf reducing_constants short_variables

%: %.cc
	g++ $< -o $@ -std=c++11

derivations.pdf: derivations.tex
	pdflatex $<
