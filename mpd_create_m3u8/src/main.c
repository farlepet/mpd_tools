#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include <libmpd-1.0/libmpd/libmpd.h>

// See: http://api.sarine.nl/libmpd/

#define VERSION "0.1"

char *playlist   = NULL;
char *dir = NULL;

void help_msg();

int main(int argc, char **argv) {
	int argn = 1;
	for(; argn < argc; argn++) {
		char *arg = argv[argn];
		if(!strcmp(arg, "-h")) {
			help_msg();
			return 0;
		} else if(!strcmp(arg, "-v")) {
			printf("VERSION: "VERSION"\n");
			return 0;
		} else if(!strcmp(arg, "-P")) {
			if(argn == (argc - 1)) {
				printf("You must specify a playlist!\n");
				return 1;
			}
			argn++;
			playlist = argv[argn];
		} else if(!strcmp(arg, "-D")) {
			if(argn == (argc - 1)) {
				printf("You must specify a directory!\n");
				return 1;
			}
			argn++;
			dir = argv[argn];
		} else {
			printf("Unrecognized argument: %s\n\n", arg);
			help_msg();
			return 1;
		}
	}

	if(playlist == NULL) {
		printf("You must specify a playlist (-P playlist)\n");
		return 1;
	}

	if(dir == NULL) {
		printf("You must specify a target directory (-D dir)\n");
		return 1;
	}



	MpdObj *mpd = mpd_new("localhost", 6600, NULL);
	if(mpd_connect(mpd)) {
		printf("Failed to connect to MPD\n");
		return 1;
	}

	MpdData *songs = mpd_database_get_playlist_content(mpd, playlist);
	if(songs == NULL) {
		printf("Could not retrieve songs from playlist \"%s\"\n", playlist);
	}

	for(songs = mpd_data_get_first(songs); songs != NULL; songs = mpd_data_get_next(songs)) {
		mpd_Song *song = songs->song;
		printf("%s/%s\n", dir, song->file);
	}

	mpd_disconnect(mpd);
	mpd_free(mpd);
}

void help_msg() {
	printf("USAGE: mpd_create_m3u8 [options] -P playlist -D dir\n");
	printf("    -h             Display this help message\n"
	       "    -v             Display the version\n"
		   "    -P playlist    Playlist to use\n"
			 "    -D target_dir  Music directory\n"
			 "\n"
			 "OUTPUT:\n"
			 "  [dir]/song_file\n"
		   );
}
