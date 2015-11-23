#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"

static long get_file_size(FILE *f)
{
    long file_size;

    if (fseek(f, 0, SEEK_END) != 0) {
        /* Failure */
        return -1;
    }

    file_size = ftell(f);

    rewind(f);

    return file_size;
}

int read_file_null_terminated(struct buffer *buf, const char *filename)
{
    FILE *f;
    long file_size;
    char *file_data;

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Dictionary file does not exist: %s\n", filename);
        goto err_fopen;
    }

    file_size = get_file_size(f);
    if (file_size < 0) {
        fprintf(stderr, "get_file_size() failed\n");
        goto err_file_size;
    }

    file_data = (char*)malloc(file_size + 1);
    if (!file_data) {
        fprintf(stderr, "Failed to allocate memory\n");
        goto err_file_size;
    }

    if (fread(file_data, 1, file_size, f) != file_size) {
        fprintf(stderr, "Failed to read the whole file\n");
        goto err_fread;
    }

    file_data[file_size] = '\0';

    buf->data = file_data;
    buf->length = file_size + 1;

    return 0;

err_fread:
    free(file_data);
err_file_size:
    fclose(f);
err_fopen:
    return -1;
}

/* Count lines */
int calc_line_count(struct buffer *buf)
{
    int line_count;
    long i;

    line_count = 0;
    if (buf->data[0] != '\0') {
        line_count++;
    }

    for (i = 1; i < buf->length; ++i) {
        /* Looking for starts of lines */
        if (buf->data[i - 1] == '\0' && buf->data[i] != '\0') {
            line_count++;
        }
    }

    return line_count;
}

int *list_lines(struct buffer *buf, int line_count)
{
    int *line_offsets;
    int next_index;
    long i;

    line_offsets = (int *)malloc(sizeof(int) * line_count);
    if (!line_offsets) {
        return NULL;
    }

    next_index = 0;
    if (buf->data[0] != '\0') {
        line_offsets[next_index++] = 0;
    }

    for (i = 1; i < buf->length; ++i) {
        /* Looking for starts of lines */
        if (buf->data[i - 1] == '\0' && buf->data[i] != '\0') {
            line_offsets[next_index++] = i;
        }
    }

    return line_offsets;
}

static int isline(char c)
{
    return c >= 32 && c <= 127;
}

/* Replace all invalid characters with '\0' */
void split_with_zero_chars(struct buffer *buf)
{
    /* Replace all invalid characters with '\0' */
    long i;
    for (i = 0; i < buf->length; ++i) {
        if (!isline(buf->data[i])) {
            buf->data[i] = '\0';
        }
    }
}
