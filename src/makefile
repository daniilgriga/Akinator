SOURCES=main.c log.c stack.c
EXE=test

BUILD=../build/

CC=gccc
CFLAGS=-Wall -g -lm -fsanitize=address

OBJS=$(SOURCES:.c=.o)

#OBJS=$(patsubst %.c,$(BUILD)%.o,$(SOURCES))

.PHONY: all clean

all: $(EXE)


clean:
	cd $(BUILD); rm -rf *.o $(EXE)

$(EXE): $(OBJS)
	@cd $(BUILD); $(CC) $(CFLAGS) $(OBJS) -o $@

.c.o:
	@$(CC) -I include -c $(CFLAGS) $< -o $(BUILD)$@

run:
	$(BUILD)$(EXE)
