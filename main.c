#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "buffer.h"
#include "trie.h"

static char *read_line()
{
    char *buf;
    char *new_buf;
    int ch;
    int len;
    int max_size;

    len = 0;
    max_size = 1;

    buf = (char *)malloc(max_size);
    if (!buf) {
        return NULL;
    }

    /* Skip invalid characters */
    do {
        ch = fgetc(stdin);
    } while (ch != EOF && (ch < 32 || ch > 127));

    /* Read word */
    while (ch != EOF && ch >= 32 && ch <= 127) {
        if (len + 2 > max_size) {
            /* Extend the buffer */
            max_size *= 2;
            new_buf = (char *)malloc(max_size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }

            strncpy(new_buf, buf, len);
            free(buf);
            buf = new_buf;
        }

        buf[len++] = (char)ch;

        ch = fgetc(stdin);
    }

    buf[len] = '\0';

    return buf;
}

struct trie_node *trie_from_dict_file(struct buffer *buf, const char *filename)
{
    int line_count;
    int *line_offsets;
    struct trie_node *root_node;
    int i;

    if (read_file_null_terminated(buf, filename) < 0) {
        goto err_read_file;
    }

    /* Replace all invalid characters with '\0' */
    split_with_zero_chars(buf);

    /* Count lines */
    line_count = calc_line_count(buf);

    /* Determine offsets of lines in file */
    line_offsets = list_lines(buf, line_count);
    if (!line_offsets) {
        goto err_list_lines;
    }

    root_node = trie_create_leaf(0, 0, 0);
    if (!root_node) {
        goto err_create_root_node;
    }

    for (i = 0; i < line_count; ++i) {
        int offset = line_offsets[i];
        size_t length = strlen(buf->data + offset);

        if (trie_insert(root_node, buf->data, offset, offset + length) < 0) {
            goto err_trie_insert;
        }
    }

    free(line_offsets);

    return root_node;

err_trie_insert:
    trie_node_free(root_node);
err_create_root_node:
    free(line_offsets);
err_list_lines:
    free(buf->data);
err_read_file:
    return NULL;
}

int main(int argc, char *argv[])
{
    char *input;
    int req_exit;
    struct trie_node *root_node;
    struct buffer buf;
    int ret;

    ret = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: ./has_key [file]\n");
        ret = 1;
        goto err_initial;
    }

    root_node = trie_from_dict_file(&buf, argv[1]);
    if (!root_node) {
        fprintf(stderr, "Failed to load dictionary from file.\n");
        ret = 2;
        goto err_initial;
    }

    req_exit = 0;
    while (!req_exit) {
        input = read_line();
        if (!input) {
            fprintf(stderr, "Failed to read query from stdin.\n");
            ret = 3;
            goto err_read_line;
        }

        if (!strcmp(input, "exit")) {
            req_exit = 1;
        } else {
            printf(trie_has_key(root_node, buf.data, input) ? "YES\n" : "NO\n");
        }

        free(input);
    }

err_read_line:
    free(buf.data);
    trie_node_free(root_node);
err_initial:
    return ret;
}
