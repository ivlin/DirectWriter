/*
read_top_level - Reads through the current directory and recursively goes through all subdirectories printing their contents to the map file
  @param char* path - path to the directory to be edited
  @param FILE* map_file - file to output the directory map
  @param int level - how many levels down to read
*/
int read_top_level(char *path, FILE* map_file, int level);
/* 
generate_map_file - Creates a file to print the directory structure at the specified path
  @param char* - path to store the directory file
  @return FILE* - file in which the directory structure is to be output
*/
FILE* generate_map_file(char* path);
