#include "word.h"
#include "code.h"
#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Word *word_create(uint8_t *syms, uint32_t len){
    Word *new_word = (struct Word *)calloc(1, sizeof(Word));
    if (new_word){
        new_word->syms = (uint8_t *)calloc(len, sizeof(uint8_t));
        new_word->len = len;
        for (int i = 0; i < (int) len; i++){
            new_word->syms[i] = syms[i];
        }
    }
    return new_word;
}

Word *word_append_sym(Word *w, uint8_t sym){
    
    if (w != NULL){        
        uint8_t *new_syms = (uint8_t *)calloc((w->len + 1 ), sizeof(uint8_t));
        
        for (int i = 0; i < (int) (w->len); i++){
            new_syms[i] = w->syms[i];
        }
        
        new_syms[w->len] = sym;

        Word *new_word = word_create(new_syms, w->len + 1);
        
        free(new_syms);
        new_syms = NULL;
        
        return new_word;
    } else {
        
        return word_create(&sym, w->len + 1);
    }
}

void word_delete(Word *w){
    if (w){
        free(w->syms);
        w->syms = NULL;
        free(w);
        w = NULL;
    }
}

WordTable *wt_create(void){
    WordTable *new_wt = (WordTable *)calloc(MAX_CODE, sizeof(Word));
    if (new_wt){
        new_wt[EMPTY_CODE] = word_create(0, 0);
    }
    return new_wt;
}

void wt_reset(WordTable *wt){
    for (int i = START_CODE; i < ALPHABET; i++){
        if (wt[i] != NULL){
            word_delete(wt[i]);
        }
    }
}

void wt_delete(WordTable *wt){
    wt_reset(wt);
    word_delete(wt[0]);
    free(wt);
    wt = NULL;
}
