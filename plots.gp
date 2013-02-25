
set format x "%.0f"
set format y "%.3f"
set key center right
set xtics rotate by 45 offset -2,-2
#set yrange  [0:*]
set terminal pdf
set output 'fixed_len.pdf'
set ylabel 'Seconds'
set xlabel 'Pattern length'
plot \
    "data/fixed_len.dat" using 1:2 title 'Aho-Corasick' with lines lw 2, \
    "data/fixed_len.dat" using 1:4 title 'Rabin-Karp' with lines lw 2, \
    "data/fixed_len.dat" using 1:3 title 'Aho-Corasick without preprocessing' with lines lw 2;

set terminal pdf
set output 'var_len.pdf'
set xlabel 'Total pattern length'
plot \
    "data/var_len.dat" using 1:2 title 'Aho-Corasick' with lines lw 2, \
    "data/var_len.dat" using 1:4 title 'Rabin-Karp' with lines  lw 2, \
    "data/var_len.dat" using 1:3 title 'Aho-Corasick without preprocessing' with lines lw 2; \

set output 'text_len.pdf'
set xlabel 'Text length'
plot \
    "data/text_len.dat" using 1:2 title 'Aho-Corasick' with lines lw 2, \
    "data/text_len.dat" using 1:4 title 'Rabin-Karp' with lines  lw 2;
#"data/text_len.dat" using 1:3 title 'Aho-Corasick without preprocessing' with lines lw 2; \

set output 'text_len_fixed.pdf'
set xlabel 'Text length'
plot \
    "data/text_len_fixed.dat" using 1:2 title 'Aho-Corasick' with lines lw 2, \
    "data/text_len_fixed.dat" using 1:4 title 'Rabin-Karp' with lines  lw 2;
#"data/text_len_fixed.dat" using 1:3 title 'Aho-Corasick without preprocessing' with lines lw 2; \

