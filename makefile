
---

### Makefile
```makefile
CC = gcc
CFLAGS = -Wall -g
TARGET = eshop
SRC = main.c
HEADERS = functions.h

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
