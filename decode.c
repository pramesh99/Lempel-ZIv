#include "trie.h"
#include "word.h"
#include "io.h"
#include "code.h"
#include <math.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <inttypes.h>

int total_r_bytes = 0;
int total_w_bytes = 0;

uint8_t bit_length(uint16_t code){
    if (code == 0){
        return 1;
    } 

    return (uint8_t) (log2(code) + 1);
}

int main(int argc, char **argv){
    int opt, v;

    // OPEN FILES
    int srcfile = STDIN_FILENO;
    int dstfile = STDOUT_FILENO;

    while ((opt = getopt(argc, argv, "vi:o:")) != -1){
        switch (opt){
            case 'v':;
                v = 1;
                break;
            case 'i':;
                srcfile = open(optarg, O_RDONLY);
                break;
            case 'o':;
                dstfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                break;                
        }
    }
    
    // READ INPUT HEADER
    FileHeader header;
    read_header(srcfile, &header);
    if (header.magic != MAGIC){
        printf("Input file not encoded by encode.c\n");
        return 0;
    }

    // COPY PERMISSIONS
    struct stat srcstats;
    struct stat dststats;
    fstat(srcfile, &srcstats);
    fstat(dstfile, &dststats);
    fchmod(dstfile, srcstats.st_mode);

    // WRITE HEADER
    // write_header(dstfile, &header);

    // DECOMPRESSION    
    WordTable *table = wt_create();
    uint8_t curr_sym = 0;
    uint16_t curr_code = 1;
    uint16_t next_code = START_CODE;
    
    while(read_pair(srcfile, &curr_code, &curr_sym, bit_length(next_code))){
        table[next_code] = word_append_sym(table[curr_code], curr_sym);
        buffer_word(dstfile, table[next_code]);

        next_code++;
        
        if (next_code == MAX_CODE){
            wt_reset(table);
            next_code = START_CODE;
        }
    }
    flush_words(dstfile);

    //CLOSE FILES
    close(srcfile);
    close(dstfile);

    wt_delete(table);
    
    if (v == 1){
        //compression ratio = 100*(1-(compSize/uncompSize))        
        printf("Compressed file size: %d bytes\n", total_w_bytes);
        printf("Uncompressed file size: %d bytes\n", total_r_bytes);
        double ratio = 100 * (1 - ((double)total_w_bytes / (double)total_r_bytes));
        printf("Compression ratio: %lf%%\n", ratio);
    }

    return 0;
}
