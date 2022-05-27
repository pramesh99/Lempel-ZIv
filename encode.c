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
    //printf("%d\n", (int)(log2((double) code) + 1));
    return (uint8_t) (log2((double) code) + 1);
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
   

    // // READ INPUT HEADER
    // FileHeader *header;
    // read_header(srcfile, header);

    // COPY PERMISSIONS
    struct stat srcstats;
    struct stat dststats;
    fstat(srcfile, &srcstats);
    fstat(dstfile, &dststats);
    fchmod(dstfile, srcstats.st_mode);

    // WRITE HEADER
    FileHeader header;
    header.magic = MAGIC;
    header.protection = srcstats.st_mode;
    write_header(dstfile, &header);

    // COMPRESSION
    TrieNode *root = trie_create();
    TrieNode *curr_node = root;
    TrieNode *prev_node = NULL;
    uint8_t curr_sym = 0;
    uint8_t prev_sym = 0;
    uint16_t next_code = START_CODE;

    while (read_sym(srcfile, &curr_sym)) {
        TrieNode *next_node = trie_step(curr_node, curr_sym);
        if (next_node != NULL){
            prev_node = curr_node;
            curr_node = next_node;
        } else {
            buffer_pair(dstfile, curr_node->code, curr_sym, bit_length(next_code));
            curr_node->children[curr_sym] = trie_node_create(next_code);
            curr_node = root;
            next_code++;
        }

        if (next_code == MAX_CODE){
            trie_reset(root);
            curr_node = root;
            next_code = START_CODE;
        }

        prev_sym = curr_sym;
    }

    if (curr_node->code != root->code){
        buffer_pair(dstfile, prev_node->code, prev_sym, bit_length(next_code));// same^
        next_code = (next_code + 1) % MAX_CODE;
    }

    buffer_pair(dstfile, STOP_CODE, 0, bit_length(next_code));    
    flush_pairs(dstfile);
    // END COMPRESSION

    //CLOSE FILES
    close(srcfile);
    close(dstfile);
    trie_delete(root);


    if (v == 1){
        //compression ratio = 100*(1-(compSize/uncompSize))        
        printf("Compressed file size: %d bytes\n", total_w_bytes);
        printf("Uncompressed file size: %d bytes\n", total_r_bytes);
        double ratio = 100 * (1 - ((double)total_w_bytes / (double)total_r_bytes));
        printf("Compression ratio: %lf%%\n", ratio);
    }
    return 0;
}
