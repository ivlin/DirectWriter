# Directory Writer
A terminal based tool for easy, fast file manipulation. Written in C.

# Description
This project writes out the structure of a directory as text in your terminal. You can then edit the text in-terminal and any changes made through the editor will be applied to the actual file names, so all renaming can be done in one place.

# Motivation
While simple scripts and regexes can easily rename large batches files with a consistent naming scheme, things are not as simple when the original or modified filenames don't follow any specific pattern. Whether on GUI or command line, the process becomes a tedious process of keystrokes. Directory Writer reduces the time spent on this meaningless labour.

# Usage
Just run
```$ make run```
to start editing your current directory!

If you would like to use a choose a different directory to work with, run<br>
```
~$ make all
-$ ./dirinfo [directory]
-$ ./editor
```
separately

# Future Development
Since renaming files is allowed, the application may as well integrate moving and removing files as well.

# Bugs
-Segfaults when one scrolls too quickly<br>
-Problems with small screen sizes<br>
-Scrolling downwards sometimes causes the cursor to jump into the information buffer. It is necessary to scroll back up in order to scroll properly again.
