set terminal pdf
set output 'fixed_len.pdf'
set ylabel 'Seconds'
set xlabel 'Pattern length'
plot \
    "data/fixed_len.dat" using 1:2 title 'Aho-Corasick' with lines, \
    "data/fixed_len.dat" using 1:3 title 'Rabin-Karp' with lines;

set terminal pdf
set output 'var_len.pdf'
set xlabel 'Total pattern length'
plot \
    "data/var_len.dat" using 1:2 title 'Aho-Corasick' with lines, \
    "data/var_len.dat" using 1:3 title 'Rabin-Karp' with lines;

