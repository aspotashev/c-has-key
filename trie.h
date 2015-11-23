#ifndef TRIE_H
#define TRIE_H

#include "trie.h"

struct trie_node {
    int prefix_begin;
    int prefix_end;

    char is_final;

    struct trie_node *children_head;
    struct trie_node *next_sibling;
};

int trie_insert(
    struct trie_node *root_node, const char *data,
    int new_string_begin, int new_string_end);

int trie_has_key(struct trie_node *node, const char *data, const char *key);

void trie_print(struct trie_node *root_node, const char *data, int indent);

struct trie_node *trie_create_leaf(int string_begin, int string_end, int is_final);

void trie_node_free(struct trie_node *node);

#endif // TRIE_H
