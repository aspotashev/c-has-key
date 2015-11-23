#ifndef BUFFER_H
#define BUFFER_H

struct buffer {
    char *data;
    long length;
};

int read_file_null_terminated(struct buffer *buf, const char *filename);
int calc_line_count(struct buffer *buf);
int *list_lines(struct buffer *buf, int line_count);
void split_with_zero_chars(struct buffer *buf);

#endif // BUFFER_H
