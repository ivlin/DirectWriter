#Directory Typer (Prototype)
An incomplete attempt to create a terminal based tool for easy directory and file manipulation.

#Description
As of present, this project lets you create a list of a directory tree. Any changes made through the editor will be replicated with the actual files, so all renaming can be done in one place.

#Usage
Just run <br>
$make run <br>
to start editing your current directory!

If you would like to use a choose a different directory to work with, run<br>
~$make all<br>
-$./dirinfo [directory]<br>
-$./editor<br>
separately

#Unfinished
Due to time constraints, only renaming files is allowed. Renaming directories, deleting, and creating new files and directories is a work in progress. There is also little protection against user error.<br>
Only basic typing functionality works. Input and backspace both work. But things like delete and insertion mode do not. 

#Bugs
-Segfaults when one scrolls too quickly<br>
-Problems with small screen sizes<br>
-Scrolling downwards sometimes causes the cursor to jump into the information buffer. It is necessary to scroll back up in order to scroll properly again.
