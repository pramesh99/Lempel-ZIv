# Lempel-ZIv
Implementation of Lempel-Ziv compression algorithm in C

encode.c and decode.c can be compiled with the "make all" or "make" commands. To compile only encode.c/decode.c and its supporting files, run "make encode" or "make decode". The program can then be run with the resultant ./encode or ./decode executable. "make clean" will clear all compiler generated files and "make infer" will run infer. There are 3 command line arguments that can be chosen from:

    -i <file> specifies input file
        stdin is default
    -o <file> specifies output file
        stdout is default
    -v displays compression/decompression statistics

Encode and decode also support I/O redirection. The program will output a compressed or decompressed file at the location
specified and display stats if specified.

Memory Hygiene:

    encode/decode pass make infer without any errors. 

Errors:
    - Encode's output is slightly off from the expected output. For the fox test, the last two bytes have single bit 
        errors but, when decoded, the output is as expected showing that decode works. 
    - When testing binary compression and decompression
