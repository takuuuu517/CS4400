a.out: w.c x.c y.c z.c main.c
	gcc -c w.c
	gcc -c x.c
	gcc -c y.c
	gcc -c z.c
	ar -ruv l.a x.o w.o
	gcc main.c z.o l.a y.o
