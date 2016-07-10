#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int read_top_level(char *path, FILE* map_file, int level){
  DIR *dir;

  dir = opendir(path);
  if (dir == NULL){
    printf("Opendir: %s\n",strerror(errno));
  }
  
  struct dirent *dir_file;
  dir_file = readdir(dir);
  
  char name[256];
  char *temp;

  while (dir_file != NULL){
    strcpy(name, path);
    strcat(name, "/");
    strcat(name, dir_file->d_name);
   
    temp = strrchr(name,'/');

    if (strcmp(temp,"/..")!=0 && strcmp(temp,"/.")!=0){
      int i;
      if (dir_file->d_type == DT_DIR){
	fprintf(map_file,"%s\n",name);
	read_top_level(name, map_file, level+1);
      }else{
	fprintf(map_file,"%s\n",name);
      }
    }
    dir_file = readdir(dir);
  }
  closedir(dir);
  return 0;
}

FILE* generate_map_file(char* path){
  FILE* map_file = tmpfile();
  read_top_level(path,map_file,0);
  return map_file;
}

