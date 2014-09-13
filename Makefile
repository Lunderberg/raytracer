OBJ=main_11 derivations.pdf reducing_constants_11 short_variables_11 no_vector

all: $(OBJ)

%: %.cc
	g++ -g $< -o $@

%_11: %.cc
	g++ -g $< -o $@ -std=c++11

derivations.pdf: derivations.tex
	pdflatex $<

clean:
	rm -f $(OBJ) derivations.aux derivations.log output.ppm
