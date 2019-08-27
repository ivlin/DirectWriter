#ifndef EDITOR
#define EDITOR

#define CURSOR_UP "\033[1A"
#define CURSOR_DOWN "\033[1B"
#define CURSOR_RIGHT "\033[1C"
#define CURSOR_LEFT "\033[1D"
#define CURSOR_ROW_COL "\033[%d;%df"
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
  char* original;
  struct line* next;
} line;

struct screen {
  struct line* lines;
  struct line* changed_lines;
  struct line* current_line;
  int rows, cols;
  int cur_row, cur_col;
  
  //used when tracking whole file
  struct line* current_top;
};

struct termios;
struct winsize;

/*
cleanup - frees allocated memory
@param screen * - struct that represents the virtual window pre-initialized with its dimensions
*/
int cleanup(struct screen*);

/*
fileops - applies changes to the actual file
@param screen * - struct that represents virtual window and all lines
*/
int fileops(struct screen*);

#endif
