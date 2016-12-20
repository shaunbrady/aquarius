CFLAGS = -nostdlib -nodefaultlibs --no-builtin -Wall -I./include
#CFLAGS = -nostdlib -nodefaultlibs --no-builtin -I./include
CXXFLAGS = $(CFLAGS)
LDFLAGS = -nostdlib -nodefaultlibs -Ttext 100000
LDFLAGS_EXT = --oformat binary --static -nostdlib -Ttext 0
LIBS =
CC = gcc
LD = ld


all: kernel

entry:
	$(CC) $(CFLAGS) -o ./src/entry.o -c ./src/entry.S

isr_wrapper:
	$(CC) $(CFLAGS) -o ./src/isr_wrapper.o -c ./src/isr_wrapper.S

memory:
	$(CC) $(CFLAGS) -o ./src/memory.o -c ./src/memory.c

cpu:
	$(CC) $(CFLAGS) -o ./src/cpu.o -c ./src/cpu.c

interrupts:
	$(CC) $(CFLAGS) -o ./src/interrupts.o -c ./src/interrupts.c

pci:
	$(CC) $(CFLAGS) -o ./src/pci.o -c ./src/pci.c

screen:
	$(CC) $(CFLAGS) -o ./src/screen.o -c ./src/screen.c

keyboard:
	$(CC) $(CFLAGS) -o ./src/keyboard.o -c ./src/keyboard.c

cli:
	$(CC) $(CFLAGS) -o ./src/cli.o -c ./src/cli.c

fifo:
	$(CC) $(CFLAGS) -o ./src/fifo.o -c ./src/fifo.c

string:
	$(CC) $(CFLAGS) -o ./src/string.o -c ./src/string.c

main:
	$(CC) $(CFLAGS) -o ./src/main.o -c ./src/main.c

ext_proc_compile:
	$(CC) $(CFLAGS) -o ./src/ext_proc.o -c ./src/ext_proc.c

ext_entry:
	$(CC) $(CFLAGS) -o ./src/ext_entry.o -c ./src/ext_entry.S

ext_proc: ext_proc_compile ext_entry
	$(LD) $(LDFLAGS_EXT) -o ./ext_proc ./src/ext_entry.o ./src/ext_proc.o

compile: entry isr_wrapper memory cpu keyboard interrupts pci main screen cli fifo string

kernel: compile
#	$(LD) $(LDFLAGS) -o ./kernel ./src/entry.o ./src/cpu.o ./src/main.o ./src/memory.o ./src/interrupts.o ./src/isr_wrapper.o ./src/pci.o ./src/screen.o
	$(LD) $(LDFLAGS) -o ./kernel ./src/entry.o ./src/cpu.o ./src/memory.o ./src/interrupts.o ./src/isr_wrapper.o ./src/pci.o ./src/screen.o ./src/keyboard.o ./src/cli.o ./src/fifo.o ./src/string.o ./src/main.o

clean:
	-rm -f ./src/*.o
#	-rm -f ./ext_proc
	-rm -f ./kernel

install:
