#ifndef IO
#define IO

int print_bufffers(struct screen*);
int open_screen_buffer(termios*);
int open_preserved_screen(termios*);
int detect_keypress(int*, int*,line**,line**,line**,int,struct screen*);
int get_cursor(int*,int*,line*,line**);

#endif
