# Only the next 3 lines need to be edited - leave the rest!
TARGET = client 
DEPS = rdwrn.h
OBJ = rdwrn.o client.o

CC=cc
CFLAGS=-I. -Wall -g -O0

all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJ)




