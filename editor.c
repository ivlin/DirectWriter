#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "editor.h"
#include "scanner.h"
#include "io.h"

int main(){
  struct termios term;
  struct winsize window;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
  
  struct screen* term_screen = (struct screen*)malloc(sizeof(struct screen));
  term_screen->rows=window.ws_row;
  term_screen->cols=window.ws_col; 
  term_screen->lines = (line*)malloc(sizeof(line));
  term_screen->changed_lines = (line*)malloc(sizeof(line));
 
  init(term_screen);

  FILE* map = fopen("./map","r");

  fill_buffers(map,0,term_screen->lines);

  open_screen_buffer(&term);  

  print_buffers(term_screen);
 
  while (detect_keypress(map,term_screen));

  open_preserved_screen(&term);
  
  cleanup(term_screen);
  fileops(term_screen);
  
  fclose(map);
}

int init(struct screen* term_screen){
  struct line* line_node = term_screen->lines;
  int i;
  for (i=1; i<term_screen->rows; i++){
    line_node->text = (char*)malloc(term_screen->cols);
    line_node->status = (char*)malloc(term_screen->cols);
    if(i < term_screen->rows - 1)
    	line_node->next = (line*)malloc(sizeof(line));
    line_node = line_node->next;
  }
  return 0;
}

int cleanup(struct screen* term_screen){
  line* line_node = term_screen->lines;
  line* changed = term_screen->changed_lines;
  while (changed->next != NULL){
    changed = changed->next;
  }
  char* temp;
  while (line_node->next){
    temp = strdup(&line_node->text[line_node->begin_edit+1]);
    temp[strlen(temp)-1] = 0;
    if (strcmp(line_node->status,temp)){
      line_node->next = NULL;
      changed->next = line_node;
      changed = changed->next;
    }
    else{
      free(line_node);
      free(line_node->text);
      free(line_node->status);
      line_node = line_node->next;
    }
    free(temp);
  }
  return 0;
}

int fileops(struct screen* term_screen){
  line* changes = term_screen->changed_lines;
  char* temp;
  char path[512];
  free(changes);
  changes = changes->next;
  while (changes != NULL){
    temp = strdup(&changes->text[changes->begin_edit+1]);
    temp[strlen(temp)-1] = 0;
    if (strcmp(changes->status,temp)){
      strcpy(path,changes->text);
      changes->text[strlen(changes->text)-1]=0;
      strcpy(&path[changes->begin_edit+1],changes->status);
      printf("Renaming %s to %s\n",path,changes->text);
      rename(path,changes->text);
    }
    free(changes->text);
    free(changes->status);
    free(changes);
    free(temp);
    changes = changes->next;
  }
}
