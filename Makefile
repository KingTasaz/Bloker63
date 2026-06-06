CC = gcc

SRC = Baloker Online/src/main.c
INCLUDE = Baloker Online/include

TARGET = Baloker Online/build/Baloker Online.exe

CFLAGS = -I"$(INCLUDE)" -Wall -Wextra
LDFLAGS = -L"Baloker Online"
LDLIBS = -lSDL3

all:
	$(CC) "$(SRC)" $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o "$(TARGET)"
	copy /Y "Baloker Online\SDL3.dll" "Baloker Online\build\"

clean:
	del /Q "Baloker Online\build\*" 2>NUL

.PHONY: all clean
