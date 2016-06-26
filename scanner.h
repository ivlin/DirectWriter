#ifndef SCANNER
#define SCANNER

line* get_previous(int, int, line*, struct screen*);
line* get_next(int, int, line*, struct screen*);
int fill_buffers(int, int, line*);

#endif
