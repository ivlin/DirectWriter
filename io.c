#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "editor.h"
#include "scanner.h"
#include "io.h"

int print_buffers(struct screen* term_screen){
  struct line* first_line = term_screen->lines;
  while (first_line != NULL){
    printf("%*s\r%s",term_screen->cols,first_line->status,first_line->text);
    first_line = first_line->next;
  }
  printf("%*s\r%s",term_screen->cols,"ORIGINAL","NEWNAME(CTRL+Q to EXIT)");
}

int get_cursor(struct screen* term_screen){
  struct line* first_line = term_screen->lines;
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

int detect_keypress(FILE* map, struct screen* term_screen){
  int key;
  line* first_line = term_screen->lines;
  key = getchar();
  if (key == 17){//ctrl+q
    return 0;
  }
  else if (key == '\033'){
    if (getchar() == '['){
      key = getchar();
      if (key == 65){
	if (term_screen->cur_row == 1 && first_line->file_offset > 1){//up
	  line* new_first = get_previous(map,term_screen);

	  new_first->next = term_screen->lines;
	  line* last;
	  while (first_line->next != NULL){
	  	last = first_line;
	  	first_line = first_line->next;
	  }
	  last->next = NULL;
	  char* temp = strdup(&first_line->text[first_line->begin_edit+1]);
	  temp[strlen(temp)-1] = 0;
	  line* changed_line = term_screen->changed_lines;
	  if (strcmp(first_line->status,temp)){
	  	while (changed_line->next != NULL){
	  		changed_line = changed_line->next;
	  	}
	  	changed_line->next = first_line;
	  }
	  else{
	  	free(first_line->text);
	  	free(first_line->status);
	  	free(first_line);
	  }
	  free(temp);

	  term_screen->lines = new_first;
	  first_line = term_screen->lines;
      
	  printf(CURSOR_SAVE);
	  print_buffers(term_screen);
	  printf(CURSOR_RESTORE);
	}
	printf(CURSOR_UP);
	get_cursor(term_screen);
      }
      else if (key == 66){//DOWN
	if (term_screen->cur_row == term_screen->rows-1){
	  term_screen->current_line->next = get_next(map,term_screen);
	  term_screen->lines = first_line->next;
	  
	  //adds changes
	  char* temp = strdup(&first_line->text[first_line->begin_edit+1]);
	  printf("%s\n",first_line->text);
	  temp[strlen(temp)-1] = 0;
	  
	  line* changed_line = term_screen->changed_lines;
	  if (strcmp(first_line->status,temp)){
	    while (changed_line->next != NULL){
	      changed_line = changed_line->next;
	    }
	    changed_line->next = first_line;
	    first_line->next=NULL;
	  }
	  else{
	    free(first_line->text);
	    free(first_line->status);
	    free(first_line);
	  }

	  first_line = term_screen->lines;
	  
	  print_buffers(term_screen);
	  printf(CURSOR_UP);
	}
	else{
	  printf(CURSOR_DOWN);
	}	
	get_cursor(term_screen);
      }
      else if (key == 67){
	printf(CURSOR_RIGHT);
	get_cursor(term_screen);
      }
      else if (key == 68){
	printf(CURSOR_LEFT);
	get_cursor(term_screen);
      }
    }
  }
  else if (term_screen->cur_col-1 <= term_screen->current_line->begin_edit ||
	   term_screen->cur_col-1 > strlen(term_screen->current_line->text)-1){
    return 1;
  }
  else if (key == 127){//backspace
  	printf(CURSOR_LEFT);
   	printf(CURSOR_SAVE);
	line* cur = term_screen->current_line;
	strcpy(&cur->text[term_screen->cur_col-2],&cur->text[term_screen->cur_col-1]);
	printf(CLEAR_LINE);
	printf("\r%*s\r%s",term_screen->cols,cur->status,cur->text);
	printf(CURSOR_RESTORE);
	term_screen->cur_col = term_screen->cur_col - 1;
  }
  else{
  	printf(CURSOR_RIGHT);
  	printf(CURSOR_SAVE);
    char temp[term_screen->cols];
    temp[0] = (char)key;
    temp[1] = 0;
    line* cur = term_screen->current_line;
    //line* cur = *current_line;
    strcat(temp,&cur->text[term_screen->cur_col-1]);
    strcpy(&cur->text[term_screen->cur_col-1],temp);
    printf(CLEAR_LINE);
    printf("\r%*s\r%s",term_screen->cols,cur->status,cur->text);
    printf(CURSOR_RESTORE);
  	term_screen->cur_col = term_screen->cur_col + 1;
  }
}

int open_screen_buffer(termios* term){
  printf(ALT_BUFFER);
  tcgetattr(STDIN_FILENO,term);
  term->c_lflag = ~(ICANON|ECHO);
  tcsetattr(STDIN_FILENO,TCSANOW,term);
}

int open_preserved_screen(termios* term){
  printf(PRESERVED_SCREEN);
  term->c_lflag = (ICANON|ECHO);
  tcsetattr(STDIN_FILENO,TCSANOW,term);
}
