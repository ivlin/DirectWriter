#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "editor.h"
#include "scanner.h"
#include "io.h"

line* get_previous(int map, int next_line, line* changes, struct screen* term_screen){
  FILE* fstream = fdopen(dup(map),"r");
  line* first = (line*)malloc(sizeof(line));
  size_t buff_size = sizeof(first->text); 
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
  		fclose(fstream);
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
  fclose(fstream);
  return first;
}

line* get_next(int map, int previous_line, line* changes, struct screen* term_screen){
  FILE* fstream = fdopen(dup(map),"r");
  line* next = (line*)malloc(sizeof(line));
  size_t buff_size = sizeof(next->text);
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
  		fclose(fstream);
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
  fclose(fstream);
  return next;
}

int fill_buffers(int file, int start_read, line* first_line){
  FILE* fstream = fdopen(dup(file),"r");
  size_t buff_size = sizeof(first_line->text); 
  first_line->file_offset = 1;
  getline(&first_line->text, &buff_size, fstream);
  first_line->begin_edit = strlen(first_line->text)-1;
  while (first_line->text[first_line->begin_edit] != '/'){
  	first_line->begin_edit = first_line->begin_edit - 1;
  }
  strcpy(first_line->status,&first_line->text[first_line->begin_edit+1]);
  *strchr(first_line->status,'\n') = 0;
  while(first_line->next != NULL){
    first_line = first_line->next;
    first_line->file_offset = (int)ftell(fstream);
    getline(&first_line->text, &buff_size, fstream);
    if (first_line->file_offset == -1){
      printf("%s\n",strerror(errno));
    }
    first_line->begin_edit = strlen(first_line->text)-1;
    while (first_line->text[first_line->begin_edit] != '/'){
    	first_line->begin_edit = first_line->begin_edit - 1;
    }
    strcpy(first_line->status,&first_line->text[first_line->begin_edit+1]);
    *strchr(first_line->status,'\n') = 0;
  }
  fclose(fstream);
}
