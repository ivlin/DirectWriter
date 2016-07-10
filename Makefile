all: ./editor.out

./editor.out: editor.c editor.h scanner.c scanner.h io.c io.h dirinfo.c dirinfo.h
	gcc -o editor.out editor.c io.c scanner.c dirinfo.c

clean:
	rm *.out *.o *~ #*
