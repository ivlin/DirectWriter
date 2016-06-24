#ifndef SCANNER
#define SCANNER

line* get_previous(int, int, line*);
line* get_next(int, int, line*);
int fill_buffers(int, int, line*);

#endif
