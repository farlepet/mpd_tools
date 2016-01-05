# Makefile for mpd_copy_pl_files

MAINDIR	= $(CURDIR)
SRC	= $(MAINDIR)/src

CSRC	= $(wildcard $(SRC)/*.c)
OBJ	= $(patsubst %.c,%.o,$(CSRC))
EXEC	= $(MAINDIR)/copy_pl_files


CFLAGS  = -Wall -Wextra -Werror -g
LDFLAGS = -lmpd

all: $(OBJ)
	@echo -e "\033[33m  \033[1mLD\033[21m    \033[34m$(EXEC)\033[0m"
	@$(CC) $(OBJ) $(LDFLAGS) -o $(EXEC)

clean:
	@echo -e "\033[33m  \033[1mCleaning mpd_copy_pl_files\033[0m"
	@rm -f $(OBJ) $(EXEC)

%.o: %.c
	@echo -e "\033[32m  \033[1mCC\033[21m    \033[34m$<\033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<
