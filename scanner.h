#ifndef SCANNER
#define SCANNER

line* get_previous(FILE*, struct screen*);
line* get_next(FILE*, struct screen*);
int fill_buffers(FILE*, int, struct screen*);

#endif
