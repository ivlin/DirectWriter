#ifndef SCANNER
#define SCANNER

int fill_buffers(FILE*, int, struct screen*);
int init_and_fill_buffers(FILE* fstream, struct screen* term_screen);

#endif
