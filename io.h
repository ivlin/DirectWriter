#ifndef IO
#define IO

#define STEP_LEFT -1
#define STEP_RIGHT 1
#define STEP_UP -1
#define STEP_DOWN 1

int print_buffers(struct screen*);
int open_screen_buffer(struct termios*);
int open_preserved_screen(struct termios*);
int detect_keypress(struct screen*);
int get_cursor(struct screen*);
#endif
