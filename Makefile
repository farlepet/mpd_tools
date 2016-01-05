# Makefile for oisc-cpu

MAINDIR	= $(CURDIR)

DUP_FIND      = $(CURDIR)/bin/mpd_dup_find
DISP_PLAYLIST = $(CURDIR)/bin/mpd_disp_playlist

all: $(DUP_FIND) $(DISP_PLAYLIST)

$(DUP_FIND):
	@make -C $(CURDIR)/mpd_dup_find EXEC=$(DUP_FIND)

$(DISP_PLAYLIST):
	@make -C $(CURDIR)/mpd_disp_playlist EXEC=$(DISP_PLAYLIST)

clean:
	@echo -e "\033[33m  \033[1mRemoving executables\033[0m"
	@rm -f $(DUP_FIND) $(DISP_PLAYLIST)
	@echo -e "\033[33m  \033[1mCleaning individual programs\033[0m"
	@make -C $(CURDIR)/mpd_dup_find clean
	@make -C $(CURDIR)/mpd_disp_playlist clean
