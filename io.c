#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "editor.h"
#include "scanner.h"
#include "io.h"

/*
  Goes through the screen and prints out all visible lines.
*/
int print_buffers(struct screen* term_screen){
  printf(CURSOR_ROW_COL,1,1);
  struct line* first_line = term_screen->current_top;
  int i;
  for (i=0;i<term_screen->rows-1;i++){
    if (first_line!=NULL){
      printf("%*s\r%s%s\n",term_screen->cols,first_line->original,first_line->path,first_line->revised);
      first_line = first_line->next;
    }
    else{
      printf("\n");
    }
  }
  printf("%*s\r%s",term_screen->cols,"ORIGINAL","NEWNAME(CTRL+Q to EXIT)");
}

/*
  Updates the position of the cursor relative to its current position
  The y-axis increases going down.
*/
int step_cursor(struct screen* term_screen, int vert_change, int hor_change){
  term_screen->cur_row+=vert_change;
  term_screen->cur_col+=hor_change;
  while (hor_change>0){
    printf(CURSOR_RIGHT);
    hor_change--;
  }
  while (hor_change<0){
    printf(CURSOR_LEFT);
    hor_change++;
  }
}

/*
  Updates the position of the cursor in the terminal window.
*/
int get_cursor(struct screen* term_screen){
  struct line* first_line = term_screen->current_top;
  printf(GET_CURSOR);
  int temp;
  temp = getchar();//\033
  temp = getchar();//]
  term_screen->cur_row = getchar()-'0';
  temp = getchar()-'0';
  while (temp >= 0 && temp <= 9){
    term_screen->cur_row = 10 * term_screen->cur_row + temp;
    temp = getchar()-'0';
  }
  //; divider
  term_screen->cur_col = getchar()-'0';//col
  temp = getchar()-'0';
  while (temp >= 0 && temp <= 9){
    term_screen->cur_col = 10 * term_screen->cur_col + temp;
    temp = getchar()-'0';
  }
  while (temp != 'R'-'0'){
    temp = getchar()-'0';
  }
  //update current_line
  int i;
  for (i=1;i<term_screen->cur_row;i++){
    first_line = first_line->next;
  }
  term_screen->current_line = first_line;
}

/*
  Keypress listener
*/
int detect_keypress(struct screen* term_screen){
  int key;
  line* first_line = term_screen->lines;
  key = getchar();
  if (key == 17){//ctrl+Q
    return 0;
  }
  else if (key == '\033'){
    if (getchar() == '['){
      key = getchar();
      //UP
      if (key == 65){
        if (term_screen->cur_line_index > 0){
          if (term_screen->lines!=term_screen->current_top && term_screen->cur_row == 1){//up
            line* cur;
            for (cur=term_screen->lines;cur->next!=term_screen->current_top;cur=cur->next){
            }
            term_screen->current_top=cur;
            printf(CURSOR_SAVE);
            print_buffers(term_screen);
            printf(CURSOR_RESTORE);
            term_screen->cur_line_index--;
          }
          else{
            printf(CURSOR_UP);
            term_screen->cur_line_index--;
          }
        }
        get_cursor(term_screen);
      }
      //DOWN
      else if (key == 66){
        if (term_screen->cur_line_index < term_screen->num_lines-1){
          if (term_screen->cur_row >= term_screen->rows-1){
            term_screen->current_top = term_screen->current_top->next;
            print_buffers(term_screen);
            printf(CURSOR_UP);
            term_screen->cur_line_index++;
          }
          else{
            printf(CURSOR_DOWN);
            term_screen->cur_line_index++;
          }
        }
        get_cursor(term_screen);
      }
      //RIGHT
      else if (key == 67){
        step_cursor(term_screen,0,STEP_RIGHT);
      }
      //LEFT
      else if (key == 68){
        step_cursor(term_screen,0,STEP_LEFT);
      }
    }
  }
  else if (term_screen->cur_col <= term_screen->current_line->begin_edit ||
     term_screen->cur_col-1 > strlen(term_screen->current_line->revised)+strlen(term_screen->current_line->path)){
    return 1;
  }
  else if (key == 127){//backspace
    step_cursor(term_screen,0,STEP_LEFT);
    printf(CURSOR_SAVE);
    line* cur = term_screen->current_line;
    strcpy(&cur->revised[term_screen->cur_col-cur->begin_edit-1],&cur->revised[term_screen->cur_col-cur->begin_edit]);
    printf(CLEAR_LINE);
    printf("\r%*s\r%s%s",term_screen->cols,cur->original,cur->path,cur->revised);
    printf(CURSOR_RESTORE);
  }
  else{
    printf(CURSOR_SAVE);
    char temp[term_screen->cols];
    temp[0] = (char)key;
    temp[1] = 0;
    line* cur = term_screen->current_line;
    strcat(temp,&cur->revised[(term_screen->cur_col)-(cur->begin_edit+1)]);
    strcpy(&cur->revised[term_screen->cur_col-(cur->begin_edit+1)],temp);
    printf(CLEAR_LINE);
    printf("\r%*s\r%s%s",term_screen->cols,cur->original,cur->path,cur->revised);
    printf(CURSOR_RESTORE);
    step_cursor(term_screen,0,STEP_RIGHT);;
  }
}

/*
  Saves and clears the terminal for writing
*/
int open_screen_buffer(struct termios* term){
  printf(ALT_BUFFER);
  tcgetattr(STDIN_FILENO,term);
  term->c_lflag = ~(ICANON|ECHO);
  tcsetattr(STDIN_FILENO,TCSANOW,term);
}

/*
  Loads the saved terminal
*/
int open_preserved_screen(struct termios* term){
  printf(PRESERVED_SCREEN);
  term->c_lflag = (ICANON|ECHO);
  tcsetattr(STDIN_FILENO,TCSANOW,term);
}
