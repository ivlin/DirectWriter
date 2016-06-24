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
#include "io.h"

int init(line*);
int cleanup(line*,line*);
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
