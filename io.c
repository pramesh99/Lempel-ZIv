#include "io.h"
#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>

#define BYTE    8     // 8 bits per byte.
#define BLOCK   4096  // 4KB per block.
// BUFFER AND INDEX FOR BUFFER_PAIR
static uint8_t buf_bp[BLOCK];
static int bit_index_bp = 0;
// BUFFER AND INDEX FOR READ_PAIR
static uint8_t buf_rp[BLOCK];
static int bit_index_rp = BLOCK * BYTE;
// BUFFER AND INDEX FOR BUFFER_WORDS
static uint8_t bbw[BLOCK];
static int sym_index = 0;
// BUFFER AND INDEX FOR READ_SYM
static uint8_t buf_rs[BLOCK];
static int rs_index = 0;
// FOR STATS
extern int total_r_bytes;
extern int total_w_bytes;

int read_bytes(int infile, uint8_t *buf, int to_read){
    int counter = 0;
    int num_bytes = 0;
    int byte_left = to_read;
    while (byte_left > BLOCK){
        num_bytes += read(infile, buf + counter * BLOCK, BLOCK);
        byte_left -= BLOCK;
        counter++;        
    }
    num_bytes += read(infile, buf + counter * BLOCK, byte_left);

    total_r_bytes += num_bytes;
    return num_bytes;
}

int write_bytes(int outfile, uint8_t *buf, int to_write){
    int counter = 0;
    int num_bytes = 0;
    int byte_left = to_write;
    while (byte_left > BLOCK){
        num_bytes += write(outfile, buf + counter * BLOCK, BLOCK);
        byte_left -= BLOCK;
        counter++;        
    }
    num_bytes += write(outfile, buf + counter * BLOCK, byte_left);

    total_w_bytes += num_bytes;
    return num_bytes;
}

void read_header(int infile, FileHeader *header){
    read_bytes(infile, (uint8_t *) header, sizeof(FileHeader));
}

void write_header(int outfile, FileHeader *header){
    write_bytes(outfile, (uint8_t *) header, sizeof(FileHeader));
}

bool read_sym(int infile, uint8_t *byte){
    static int end = 0;
    if (rs_index == 0){
        end = read_bytes(infile, buf_rs, BLOCK);
    }

    *byte = buf_rs[rs_index];
    rs_index++;

    if (rs_index == BLOCK){
        rs_index = 0;
    }

    if (end == BLOCK){
        return true;
    } else {
        if (rs_index == end){
            return false;
        } else {
            return true;
        }
    }
}

void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bit_len){
    for (int i = 0; i < bit_len; i++){
        //printf("%d\n", code);
        if ((code >> i) & 1){
            buf_bp[bit_index_bp / BYTE] |= (1 << (bit_index_bp % BYTE));
        } else {
            buf_bp[bit_index_bp / BYTE] &= ~(1 << (bit_index_bp % BYTE));
        }

        bit_index_bp++;
        //code >>= 1;

        if (bit_index_bp == BLOCK * BYTE){
            write_bytes(outfile, buf_bp, BLOCK);
            // for (int i = 0; i < BLOCK; i++){
            //     buf_bp[i] = 0;
            // }
            bit_index_bp = 0;
        }
    }

    for (int i = 0; i < BYTE; i++){
        //printf("%d\n", sym);
        if ((sym >> i) & 1){
            buf_bp[bit_index_bp / BYTE] |= (1 << (bit_index_bp % BYTE));
        } else {
            buf_bp[bit_index_bp / BYTE] &= ~(1 << (bit_index_bp % BYTE));
        }

        bit_index_bp++;
        //sym >>= 1;

        if (bit_index_bp == BLOCK * BYTE){
            write_bytes(outfile, buf_bp, BLOCK);
            // for (int i = 0; i < BLOCK; i++){
            //     buf_bp[i] = 0;
            // }
            bit_index_bp = 0;
        }
    }
    // printf("%d\n", buf_bp[bit_index_bp / 8 - 1]);
}

void flush_pairs(int outfile){
    // write_bytes(outfile, buf_bp, bit_index_bp / 8);
    // bit_index_bp = 0;

    // or this?
    
    int bytes = 0;
    if (bit_index_bp != 0){
        //printf("%d\n", bit_index_bp);
        if (bit_index_bp % 8 == 0){
            bytes = bit_index_bp / 8;
        } else {
            bytes = bit_index_bp / 8 + 1;
        }
        write_bytes(outfile, buf_bp, bytes); // not sure about indentation on this 
    }
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bit_len){
    for (int i = 0; i < bit_len; i++){
        if (bit_index_rp == BLOCK * BYTE){
            read_bytes(infile, buf_rp, BLOCK);
            bit_index_rp = 0;
        }
        
        int cur_bit = (buf_rp[bit_index_rp / 8] >> bit_index_rp % 8) & 1;

        if (cur_bit){
            *code |= (1 << i);
        } else {
            *code &= ~(1 << i);
        }
        bit_index_rp++;
    }

    for (int i = 0; i < BYTE; i++){
        if (bit_index_rp == BLOCK * BYTE){
            read_bytes(infile, buf_rp, BLOCK);
            bit_index_rp = 0;
        }

        int cur_bit = (buf_rp[bit_index_rp / 8] >> bit_index_rp % 8) & 1;

        if (cur_bit){
            *sym |= (1 << i);
        } else {
            *sym &= ~(1 << i);
        }
        bit_index_rp++;
    }
    
    if (bit_index_rp != BLOCK * BYTE && *code != 0){
        return true;
    } else {
        return false;
    }
}

void buffer_word(int outfile, Word *w){
    for (int i = 0; i < (int) w->len; i++){
        bbw[sym_index] = w->syms[i];
        sym_index++;
        if (sym_index == BLOCK){
            write_bytes(outfile, bbw, BLOCK);
            sym_index = 0;
        }
    }
}

void flush_words(int outfile){
    if (sym_index != 0){
        write_bytes(outfile, bbw, sym_index);
    }
}
