#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "editor.h"
#include "scanner.h"

int init(line*);
int cleanup(line*,line*);
int print_buffers(line*);
int open_screen_buffer(termios*);
int open_preserved_screen(termios*);
int detect_keypress(int*, int*,line**,line**,line**,int);
int get_cursor(int*,int*,line*,line**);
int fileops(line*);

int main(){
  int cursor_row, cursor_col, read_line;
  struct termios term;
  struct winsize window;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &global_win);

  line* first_line = (line*)malloc(sizeof(line));
  line* changed_lines = (line*)malloc(sizeof(line));
  line* current_line;

  init(first_line);

  int map = open("./map",O_RDWR);
  fill_buffers(map,0,first_line);
  open_screen_buffer(&term);  
  print_buffers(first_line);

  while (detect_keypress(&cursor_row,&cursor_col,&first_line,&current_line,&changed_lines,map));

  open_preserved_screen(&term);

  cleanup(first_line,changed_lines);

  fileops(changed_lines);

  close(map);
}

int get_cursor(int* row, int* col, 
	line* first_line, line** current_line){
  printf(GET_CURSOR);
  int temp;
  temp = getchar();//\033
  temp = getchar();//]
  *row = getchar()-'0';//row
  temp = getchar()-'0';
  while (temp >= 0 && temp <= 9){
    *row = 10 * *row + temp; 
    temp = getchar()-'0';
  }
  //; divider
  *col = getchar()-'0';//col
  temp = getchar()-'0';
  while (temp >= 0 && temp <= 9){
    *col = 10 * *col + temp; 
    temp = getchar()-'0';
  }
  while (temp != 'R'-'0'){
    temp = getchar()-'0';
  }
  //update current_line
  int i;
  for (i=1;i<*row;i++){
    first_line = first_line->next;
  }
  *current_line = first_line;
}

int detect_keypress(int* cursor_row, int* cursor_col, 
	line** first_line_ptr, line** current_line, line** changed_lines, int map){
  int key;
  line* first_line = *first_line_ptr;
  key = getchar();
  if (key == 17){//ctrl+q
    return 0;
  }
  else if (key == '\033'){
    if (getchar() == '['){
      key = getchar();
      if (key == 65){
	if (*cursor_row == 1 && first_line->file_offset > 1){//up
	  line* new_first = get_previous(map,first_line->file_offset, *changed_lines);
	  new_first->next = *first_line_ptr;
	  line* last;
	  while (first_line->next != NULL){
	  	last = first_line;
	  	first_line = first_line->next;
	  }
	  last->next = NULL;
	  char* temp = strdup(&first_line->text[first_line->begin_edit+1]);
	  temp[strlen(temp)-1] = 0;
	  line* changed_line = *changed_lines;
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

	  *first_line_ptr = new_first;
	  first_line = *first_line_ptr;
	  printf(CURSOR_SAVE);
	  print_buffers(first_line);
	  printf(CURSOR_RESTORE);
	}
	printf(CURSOR_UP);
	get_cursor(cursor_row, cursor_col,first_line,current_line);
      }
      else if (key == 66){
	if (*cursor_row == global_win.ws_row-1){
	  (*current_line)->next = get_next(map,(*current_line)->file_offset,*changed_lines);
	  (*first_line_ptr) = first_line->next;

	  //adds changes
	  char* temp = strdup(&first_line->text[first_line->begin_edit+1]);
	  printf("%s\n",first_line->text);
	  temp[strlen(temp)-1] = 0;
	  line* changed_line = *changed_lines;
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

	  first_line = (*first_line_ptr);
	  
	  print_buffers(first_line);
	 
	   printf(CURSOR_UP);
	}
	else{
		printf(CURSOR_DOWN);
	}
	get_cursor(cursor_row, cursor_col,first_line,current_line);
	  }
      else if (key == 67){
	printf(CURSOR_RIGHT);
	get_cursor(cursor_row, cursor_col,first_line,current_line);
      }
      else if (key == 68){
	printf(CURSOR_LEFT);
  	get_cursor(cursor_row, cursor_col,first_line,current_line);
      }
    }
  }
  else if (*cursor_col-1 <= (*current_line)->begin_edit ||
  	*cursor_col-1 > strlen((*current_line)->text)-1){
  	//editing something that shouldn't be edited
  	return 1;
  }
  else if (key == 127){//backspace
  	printf(CURSOR_LEFT);
   	printf(CURSOR_SAVE);
    line* cur = *current_line;
    strcpy(&cur->text[*cursor_col-2],&cur->text[*cursor_col-1]);
    printf(CLEAR_LINE);
    printf("\r%*s\r%s",global_win.ws_col,cur->status,cur->text);
    printf(CURSOR_RESTORE);
    *cursor_col = *cursor_col - 1;
  }
  else{
  	printf(CURSOR_RIGHT);
  	printf(CURSOR_SAVE);
    char temp[global_win.ws_col];
    temp[0] = (char)key;
    temp[1] = 0;
    line* cur = *current_line;
    strcat(temp,&cur->text[*cursor_col-1]);
    strcpy(&cur->text[*cursor_col-1],temp);
    printf(CLEAR_LINE);
    printf("\r%*s\r%s",global_win.ws_col,cur->status,cur->text);
    printf(CURSOR_RESTORE);
  	*cursor_col = *cursor_col + 1;
  }
}

int print_buffers(line* first_line){
  while (first_line != NULL){
    printf("%*s\r%s",global_win.ws_col,first_line->status,first_line->text);
    first_line = first_line->next;
  }
  printf("%*s\r%s",global_win.ws_col,"ORIGINAL","NEWNAME(CTRL+Q to EXIT)");
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

int init(line* line_node){
  int i;
  for (i=1; i<global_win.ws_row ; i++){
    line_node->text = (char*)malloc(global_win.ws_col);
    line_node->status = (char*)malloc(global_win.ws_col);
    if(i<global_win.ws_row - 1)
    	line_node->next = (line*)malloc(sizeof(line));
    line_node = line_node->next;
  }
  return 0;
}

int cleanup(line* line_node,line* changed){
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

int fileops(line* changes){
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
