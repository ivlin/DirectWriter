#ifndef EDITOR
#define EDITOR

#define CURSOR_UP "\033[1A"
#define CURSOR_DOWN "\033[1B"
#define CURSOR_RIGHT "\033[1C"
#define CURSOR_LEFT "\033[1D"
#define CURSOR_SAVE "\033[s"
#define CURSOR_RESTORE "\033[u"
#define ALT_BUFFER "\033[?1049h\033[H"
#define PRESERVED_SCREEN "\033[?1049l"
#define CLEAR_LINE "\033[2K"
#define GET_CURSOR "\033[6n"

typedef struct line { 
  int file_offset;
  int begin_edit; 
  char* text; 
  char* status;
  struct line* next;
} line;

struct screen {
  struct line* lines;
  int rows, cols;
};

typedef struct termios termios;
typedef struct winsize winsize;

int init(line*,winsize* global_win);
int cleanup(line*,line*);
int fileops(line*);

#endif
