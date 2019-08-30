/**
editor.c - main file for editing operations
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "editor.h"
#include "scanner.h"
#include "io.h"
#include "dirinfo.h"

char blah[1000];

int main(){
  struct termios term;
  struct winsize window;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

  struct screen* term_screen = (struct screen*)malloc(sizeof(struct screen));
  term_screen->rows=window.ws_row;
  term_screen->cols=window.ws_col;
  term_screen->lines = NULL;
  term_screen->changed_lines = NULL;
  term_screen->current_top = NULL;
  //term_screen->lines = (line*)malloc(sizeof(line));
  //term_screen->changed_lines = (line*)malloc(sizeof(line));
  //term_screen->current_top = term_screen->lines;

  FILE* map = generate_map_file(".");
  rewind(map);

  init_and_fill_buffers(map,term_screen);

  open_screen_buffer(&term);

  print_buffers(term_screen);

  while (detect_keypress(term_screen));
  //printf("wut\n");

  open_preserved_screen(&term);

  cleanup(term_screen);
  fileops(term_screen);
  fclose(map);
}

/*
  Deallocates all allocated memory
*/
int cleanup(struct screen* term_screen){
  line* line_node = term_screen->lines;
  line* changed = NULL;
  while (line_node){
    printf("hi\n");
    if (strcmp(line_node->original,line_node->revised)){
      //printf("%s v %s\n",line_node->original,line_node->revised);
      if (term_screen->changed_lines==NULL){
        term_screen->changed_lines=line_node;
        changed=line_node;
      }
      else{
        changed->next=line_node;
        changed=changed->next;
      }
      line_node = line_node->next;
      changed->next = NULL;
    }
    else{
      free(line_node);
      free(line_node->revised);
      free(line_node->original);
      line_node = line_node->next;
    }
  }
  return 0;
}

/*
    Applies changes to the actual files
*/
int fileops(struct screen* term_screen){
  line* changes = term_screen->changed_lines;
  char old_file_w_path[512];
  char new_file_w_path[512];
  while (changes != NULL){
    if (strcmp(changes->original,changes->revised)){
      strcpy(old_file_w_path, changes->path);
      strcat(old_file_w_path, changes->original);
      strcpy(new_file_w_path, changes->path);
      strcat(new_file_w_path, changes->revised);
      printf("Renaming %s to %s\n",old_file_w_path,new_file_w_path);
      rename(old_file_w_path,new_file_w_path);
    }
    free(changes->revised);
    free(changes->original);
    free(changes);
    changes = changes->next;
  }
}

