#ifndef IO
#define IO

int print_buffers(line*,winsize*);
int open_screen_buffer(termios*);
int open_preserved_screen(termios*);
int detect_keypress(int*, int*,line**,line**,line**,int,winsize*);
int get_cursor(int*,int*,line*,line**);

#endif
