pdflatex $1.tex
pdflatex $1.tex
pdf2ps $1.pdf
tool/poster -mA4 -pA1 $1.ps > $1_A4.ps # UNCOMMENT
ps2pdf $1_A4.ps
rm $1_A4.ps
rm $1.ps
