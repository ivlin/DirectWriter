all: ./dirinfo.out ./editor.out

./dirinfo.out: dirinfo.c
	gcc -o dirinfo.out dirinfo.c

./editor.out: editor.c editor.h
	gcc -o editor.out editor.c

clean:
	rm *.out *.o *~
