pdflatex poster.tex
pdflatex poster.tex
pdf2ps poster.pdf
tool/poster -mA4 -pA0 poster.ps > poster_A4.ps # UNCOMMENT
ps2pdf poster_A4.ps
rm poster_A4.ps
rm poster.ps
