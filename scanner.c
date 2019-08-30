#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"
#include "scanner.h"
#include "io.h"

/*
  Initializes each line in term_screen using the input from the indicated file stream.
  Return the length of the longest path+file name.
*/

int init_and_fill_buffers(FILE* fstream, struct screen* term_screen){
  line *cur_line, *prev_line;
  char* unparsed=NULL;
  size_t buff_size = 0;
  int longest_str_size=0;
  while (getline(&unparsed,&buff_size,fstream)!=-1){
    cur_line = (line*)malloc(sizeof(line));
    //line processing
    *(unparsed+strlen(unparsed)-1)=0;
    cur_line->begin_edit=strrchr(unparsed,'/')-(unparsed)+1;
    //allocation
    cur_line->path = unparsed;
    cur_line->original = (char*)malloc(term_screen->cols);
    cur_line->revised = (char*)malloc(term_screen->cols);
    //initialization
    strncpy(cur_line->original,&cur_line->path[cur_line->begin_edit],term_screen->cols);
    strncpy(cur_line->revised,cur_line->original,term_screen->cols);
    cur_line->path[cur_line->begin_edit] = 0;

    if (term_screen->lines == NULL){
      term_screen->lines = cur_line;
      term_screen->current_top = term_screen->lines;
    }
    else{
      prev_line->next = cur_line;
    }

    prev_line=cur_line;
    unparsed=NULL;

    int min_cols = strlen(cur_line->path)+strlen(cur_line->original)+strlen(cur_line->revised);
    longest_str_size = min_cols > longest_str_size ? min_cols : longest_str_size;
  }
  return longest_str_size;
}