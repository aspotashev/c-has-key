#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "trie.h"

static int min(int a, int b)
{
    return a < b ? a : b;
}

/*
 * Returns NULL on failure.
 */
struct trie_node *trie_create_leaf(int string_begin, int string_end, int is_final)
{
    struct trie_node *node = (struct trie_node *)malloc(sizeof(struct trie_node));
    if (node) {
        node->prefix_begin = string_begin;
        node->prefix_end = string_end;
        node->children_head = NULL;
        node->next_sibling = NULL;
        node->is_final = is_final;
    }

    return node;
}

/*
 * Returns the new node created which starts in the middle.
 * Returns NULL on failure.
 */
static struct trie_node *chop_node(struct trie_node *node, int offset)
{
    struct trie_node *middle;

    middle = trie_create_leaf(node->prefix_begin + offset, node->prefix_end, node->is_final);
    if (middle) {
        middle->children_head = node->children_head;

        node->prefix_end = middle->prefix_begin;
        node->children_head = middle;
        node->is_final = 0;
    }

    return middle;
}

/*
 * Requirement: data[new_string_begin] == data[matching_node->prefix_begin]
 */
static int branch_insert(
    struct trie_node *matching_node, const char *data,
    int new_string_begin, int new_string_end)
{
    struct trie_node *middle;
    int matching_length;
    int common_length;

    common_length = min(
        new_string_end - new_string_begin,
        matching_node->prefix_end - matching_node->prefix_begin);

    /*
     * We skip the check data[new_string_begin] == data[matching_node->prefix_begin]
     * because this condition already holds when calling this function.
     */
    matching_length = 1;
    while (matching_length < common_length &&
        data[new_string_begin + matching_length] ==
        data[matching_node->prefix_begin + matching_length]) {
        matching_length++;
    }

    if (matching_length < matching_node->prefix_end - matching_node->prefix_begin) {
        /* Chop @matching_node at @matching_length */
        middle = chop_node(matching_node, matching_length);
        if (!middle) {
            return -1;
        }
    }

    /* Add suffix as child to @matching_node */
    return trie_insert(matching_node, data, new_string_begin + matching_length, new_string_end);
}

/*
 * Returns 0 on success, -1 on failure
 */
int trie_insert(
    struct trie_node *root_node, const char *data,
    int new_string_begin, int new_string_end)
{
    struct trie_node *child;
    struct trie_node *prev_child;
    struct trie_node *new_leaf;
    char start_char;

    if (new_string_end - new_string_begin <= 0) {
        root_node->is_final = 1;
        return 0;
    }

    /* Walk children and look for branch starting with new_string[0] */
    start_char = data[new_string_begin];

    for (child = root_node->children_head; child; child = child->next_sibling) {
        if (data[child->prefix_begin] == start_char) {
            return branch_insert(child, data, new_string_begin, new_string_end);
        }

        prev_child = child;
    }

    new_leaf = trie_create_leaf(new_string_begin, new_string_end, 1);
    if (!new_leaf) {
        return -1;
    }

    /* Matching child not found, thus we add a new child */
    if (root_node->children_head) {
        /* Add another child after the last one in the list */
        prev_child->next_sibling = new_leaf;
    } else {
        /* Add the very first child */
        root_node->children_head = new_leaf;
    }

    return 0;
}

/*
 * Find a child such that it is a prefix of @s
 */
static struct trie_node *find_matching_child(
    struct trie_node *node, const char *data, const char *s, int s_len)
{
    struct trie_node *child;
    int prefix_len;

    for (child = node->children_head; child; child = child->next_sibling) {
        prefix_len = child->prefix_end - child->prefix_begin;

        if (prefix_len <= s_len && strncmp(data + child->prefix_begin, s, prefix_len) == 0) {
            return child;
        }
    }

    return NULL;
}

int trie_has_key(struct trie_node *node, const char *data, const char *key)
{
    int key_len;
    int node_len;

    key_len = strlen(key);

    while (key_len > 0) {
        node = find_matching_child(node, data, key, key_len);
        if (!node) {
            return 0;
        }

        node_len = node->prefix_end - node->prefix_begin;
        key += node_len;
        key_len -= node_len;
    }

    return node->is_final;
}

void trie_print(struct trie_node *root_node, const char *data, int indent)
{
    int i;
    struct trie_node *child;

    for (i = 0; i < indent; ++i) {
        printf(" ");
    }
    for (i = root_node->prefix_begin; i < root_node->prefix_end; ++i) {
        printf("%c", data[i]);
    }
    if (root_node->is_final) {
        printf(" [*]");
    }
    printf("\n");

    for (child = root_node->children_head; child; child = child->next_sibling) {
        trie_print(child, data, indent + root_node->prefix_end - root_node->prefix_begin);
    }
}

void trie_node_free(struct trie_node *node)
{
    struct trie_node *child;
    struct trie_node *next;

    for (child = node->children_head; child; child = next) {
        next = child->next_sibling;
        trie_node_free(child);
    }

    free(node);
}
