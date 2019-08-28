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
  size_t buff_size = sizeof(line_node->path);
  while (getline(&line_node->path,&buff_size,fstream)!=-1){
    //line processing
    line_node->path[strlen(line_node->path)-1]=0;
    line_node->begin_edit=strrchr(line_node->path,'/')-(line_node->path);
    //allocation
    line_node->original = (char*)malloc(term_screen->cols);
    line_node->revised = (char*)malloc(term_screen->cols);
    //initialization
    strcpy(line_node->original,&line_node->path[line_node->begin_edit+1]);
    strcpy(line_node->revised,line_node->original);
    line_node->path[line_node->begin_edit+1] = 0;


    line_node->next = (line*)malloc(sizeof(line));
    line_node = line_node->next;
    line_node->path = NULL;
  }
}
