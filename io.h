#ifndef IO
#define IO

int print_bufffers(struct screen*);
int open_screen_buffer(struct termios*);
int open_preserved_screen(struct termios*);
int detect_keypress(FILE*,struct screen*);
int get_cursor(struct screen*);
#endif
