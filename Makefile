all: main derivations.pdf

%: %.cc
	g++ $< -o $@ -std=c++11

derivations.pdf: derivations.tex
	pdflatex $<
