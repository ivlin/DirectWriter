#ifndef SCANNER
#define SCANNER

line* get_previous(int, int, line*, winsize*);
line* get_next(int, int, line*, winsize*);
int fill_buffers(int, int, line*);

#endif
