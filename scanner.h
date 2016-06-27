#ifndef SCANNER
#define SCANNER

line* get_previous(int, struct screen*);
line* get_next(int, struct screen*);
int fill_buffers(int, int, line*);

#endif
