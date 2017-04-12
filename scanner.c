#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"
#include "scanner.h"
#include "io.h"

/*
  Initializes each line in term_screen using the input from the indicated file stream.
*/
int init_and_fill_buffers(FILE* fstream, struct screen* term_screen){
  line* line_node = term_screen->lines;
  size_t buff_size = sizeof(line_node->text);
  while (getline(&line_node->text,&buff_size,fstream)!=-1){
    line_node->begin_edit = strlen(line_node->text)-1;
    while (line_node->text[line_node->begin_edit] != '/'){
    	line_node->begin_edit = line_node->begin_edit - 1;
    }
    line_node->status = (char*)malloc(term_screen->cols);
    strcpy(line_node->status,&line_node->text[line_node->begin_edit+1]);
    *strchr(line_node->status,'\n') = 0;    
    line_node->next = (line*)malloc(sizeof(line));
    line_node = line_node->next;
    line_node->text = NULL;
  }
}
