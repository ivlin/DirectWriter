all: ./dirinfo.out ./editor.out

./dirinfo.out: dirinfo.c
	gcc -o dirinfo.out dirinfo.c

./editor.out: editor.c editor.h scanner.c scanner.h io.c io.h
	gcc -o editor.out editor.c io.c scanner.c

clean:
	rm *.out *.o *~
