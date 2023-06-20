CC = gcc
CFLAG = -Wall -O0
ALL = shor

all: $(ALL)

shor: shor.c
	$(CC) $(CFLAG) -o shor shor.c

clean:
	rm -rf $(ALL)
