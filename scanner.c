#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editor.h"
#include "scanner.h"
#include "io.h"


line* get_previous(FILE* fstream, struct screen* term_screen){
  line* first = (line*)malloc(sizeof(line));
  line* changes = term_screen->changed_lines;
  size_t buff_size = sizeof(first->text); 
  int next_line = term_screen->current_line->file_offset;
  first->text = (char*)malloc(sizeof(term_screen->cols));
  first->status = (char*)malloc(sizeof(term_screen->cols));
  fseek(fstream,next_line,SEEK_SET);
  char temp;
  int off;
  while (temp != '\n' && off > 0){
    fseek(fstream,-2,SEEK_CUR);
    off = ftell(fstream);
    temp = (char)fgetc(fstream);
  }
  first->file_offset = ftell(fstream);

  line* last = changes;
  changes = changes->next;
  while (changes!= NULL){
    if (changes->file_offset == first->file_offset){
      last->next = changes->next;
      free(first->text);
      free(first->status);
      free(first);
      return changes;
    }
    last = changes;
    changes = changes->next;
  }

  getline(&first->text, &buff_size, fstream);
  first->begin_edit = strlen(first->text)-1;
  while (first->text[first->begin_edit] != '/'){
    first->begin_edit = first->begin_edit - 1;
  }
  strcpy(first->status,&first->text[first->begin_edit+1]);
  *strchr(first->status,'\n') = 0;
  return first;
}

line* get_next(FILE* fstream, struct screen* term_screen){
  line* next = (line*)malloc(sizeof(line));
  line* changes = term_screen->changed_lines;
  size_t buff_size = sizeof(next->text);
  int previous_line = term_screen->current_line->file_offset;;
  next->text = (char*)malloc(sizeof(term_screen->cols));
  next->status = (char*)malloc(sizeof(term_screen->cols));
  fseek(fstream,previous_line,SEEK_SET);
  getline(&next->text, &buff_size, fstream);
  next->file_offset = ftell(fstream);

  line* last = changes;
  changes = changes->next;
  while (changes!= NULL){
  	if (changes->file_offset == next->file_offset){
  		last->next = changes->next;
  		free(next->text);
  		free(next->status);
  		free(next);
  		return changes;
  	}
  	last = changes;
  	changes = changes->next;
  }

  getline(&next->text, &buff_size, fstream);
  next->next = NULL;
  next->begin_edit = strlen(next->text)-1;
  while (next->text[next->begin_edit] != '/'){
  	next->begin_edit = next->begin_edit - 1;
  }
  strcpy(next->status,&next->text[next->begin_edit+1]);
  *strchr(next->status,'\n') = 0;
  return next;
}


int fill_buffers(FILE* fstream, int start_read, struct screen* term_screen){
  line* first_line = term_screen->lines;
  size_t buff_size = sizeof(first_line->text);   
  while (first_line != NULL){
    first_line->file_offset = (int)ftell(fstream);
    getline(&first_line->text, &buff_size, fstream);
    first_line->begin_edit = strlen(first_line->text)-1;
    while (first_line->text[first_line->begin_edit] != '/'){
    	first_line->begin_edit = first_line->begin_edit - 1;
    }
    strcpy(first_line->status,&first_line->text[first_line->begin_edit+1]);
    *strchr(first_line->status,'\n') = 0;
    first_line = first_line->next;
  }
}



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
  }
}
