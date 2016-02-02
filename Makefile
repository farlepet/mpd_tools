# Makefile for oisc-cpu

MAINDIR	= $(CURDIR)

DUP_FIND      = $(CURDIR)/bin/mpd_dup_find
DISP_PLAYLIST = $(CURDIR)/bin/mpd_disp_playlist
COPY_PL_FILES = $(CURDIR)/bin/mpd_copy_pl_files
CREATE_M3U8   = $(CURDIR)/bin/mpd_create_m3u8
CREATE_CSV    = $(CURDIR)/bin/mpd_create_csv

all: $(DUP_FIND) $(DISP_PLAYLIST) $(COPY_PL_FILES) $(CREATE_M3U8) $(CREATE_CSV)

$(DUP_FIND):
	@make -C $(CURDIR)/mpd_dup_find EXEC=$(DUP_FIND)

$(DISP_PLAYLIST):
	@make -C $(CURDIR)/mpd_disp_playlist EXEC=$(DISP_PLAYLIST)

$(COPY_PL_FILES):
	@make -C $(CURDIR)/mpd_copy_pl_files EXEC=$(COPY_PL_FILES)

$(CREATE_M3U8):
	@make -C $(CURDIR)/mpd_create_m3u8 EXEC=$(CREATE_M3U8)

$(CREATE_CSV):
	@make -C $(CURDIR)/mpd_create_csv EXEC=$(CREATE_CSV)

clean:
	@echo -e "\033[33m  \033[1mRemoving executables\033[0m"
	@rm -f $(DUP_FIND) $(DISP_PLAYLIST) $(COPY_PL_FILES) $(CREATE_M3U8) $(CREATE_CSV)
	@echo -e "\033[33m  \033[1mCleaning individual programs\033[0m"
	@make -C $(CURDIR)/mpd_dup_find clean
	@make -C $(CURDIR)/mpd_disp_playlist clean
	@make -C $(CURDIR)/mpd_copy_pl_files clean
	@make -C $(CURDIR)/mpd_create_m3u8 clean
	@make -C $(CURDIR)/mpd_create_csv clean
