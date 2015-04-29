RDIR := $(dir $(lastword $(MAKEFILE_LIST)))

all: $(RDIR)relatorio.pdf

$(RDIR)relatorio.pdf : $(RDIR)relatorio.tex
	latexmk $(RDIR)relatorio.tex -pdf -outdir=$(RDIR)
