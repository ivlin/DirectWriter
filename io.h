#ifndef IO
#define IO

int print_bufffers(struct screen*);
int open_screen_buffer(termios*);
int open_preserved_screen(termios*);
int detect_keypress(FILE*,struct screen*);
//int get_cursor(struct screen*,line**);
int get_cursor(struct screen*);
#endif
