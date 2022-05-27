#include "trie.h"
#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


TrieNode *trie_node_create(uint16_t index){
    TrieNode *new_node = (struct TrieNode *)calloc(1, sizeof(TrieNode));
    if (new_node){
        new_node->code = index;
    }
    return new_node;
}

void trie_node_delete(TrieNode *n){
    if (n){
        free(n);
        n = NULL;
    }
}

TrieNode *trie_create(void){
    return trie_node_create(EMPTY_CODE);
}

void trie_reset(TrieNode *root){
    for (int i = START_CODE; i < ALPHABET; i++){
        if (root->children[i] != NULL){
            trie_delete(root->children[i]);
        }        
    }
}

void trie_delete(TrieNode *n){
    trie_reset(n);
    trie_node_delete(n);
}

TrieNode *trie_step(TrieNode *n, uint8_t sym){
    if(n->children[sym] != NULL){
        return n->children[sym];
    }
    return NULL;
}
