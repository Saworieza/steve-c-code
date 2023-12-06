CC=gcc
CFLAGS=-I. -w
DEPS = ag.h
OBJ = autograderv1.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ag: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
