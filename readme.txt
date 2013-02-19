String processing project - Aleksi Hartikainen & Jussi Kokkala
--------------------------------------------------------------

Aleksi Hartikainen:
-Aho-Corasick algorithm:
    aho_corasick.cpp
    aho_corasick.hpp
    bitset.hpp

Jussi Kokkala:
 -Rabin-Karp algorithm:
    rabin_karp.cpp
    rabin_karp.hpp
    rabin_karp_hash.hpp

Both:
 - the rest


 ---------------------------------------------------------------
Usage:

Compile:
-   make

Running:
 Sanity test:
    bin/test
 Performance test:
    bin/performance [text_file] [pattern_file] [algorithm #]
  -examples:
    bin/performance input/robinson_book.txt input/musketeers_patterns.txt
    bin/performance input/musketeers_book.txt input/robinson_patterns.txt
