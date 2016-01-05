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
char *target_dir = NULL;
char *source_dir = NULL;

void help_msg();
int file_exists(char *);

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
		} else if(!strcmp(arg, "-S")) {
			if(argn == (argc - 1)) {
				printf("You must specify a source directory!\n");
				return 1;
			}
			argn++;
			source_dir = argv[argn];
		} else if(!strcmp(arg, "-T")) {
			if(argn == (argc - 1)) {
				printf("You must specify a target directory!\n");
				return 1;
			}
			argn++;
			target_dir = argv[argn];
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

	if(target_dir == NULL) {
		printf("You must specify a target directory (-T target-dir)\n");
		return 1;
	}

	if(source_dir == NULL) {
		printf("You must specify a source directory (-S source-dir)\n");
		return 1;
	}



	MpdObj *mpd = mpd_new("localhost", 6600, NULL);
	if(mpd_connect(mpd)) {
		printf("Failed to connect to MPD\n");
		return 1;
	}
	printf("Connected to MPD\n");

	MpdData *songs = mpd_database_get_playlist_content(mpd, playlist);
	if(songs == NULL) {
		printf("Could not retrieve songs from playlist \"%s\"\n", playlist);
	}

	size_t target_dir_strlen = strlen(target_dir);
	size_t source_dir_strlen = strlen(source_dir);
	for(songs = mpd_data_get_first(songs); songs != NULL; songs = mpd_data_get_next(songs)) {
		mpd_Song *song = songs->song;
		char *target_file = (char *)malloc(target_dir_strlen + strlen(song->file) + 2);
		strcpy(target_file, target_dir);
		target_file[target_dir_strlen] = '/';
		strcpy(target_file + (target_dir_strlen + 1), song->file);
		if(!file_exists(target_file)) {
			printf("Copying song to %s\n", target_file);

			char *source_file = (char *)malloc(source_dir_strlen + strlen(song->file) + 2);
			strcpy(source_file, source_dir);
			source_file[source_dir_strlen] = '/';
			strcpy(source_file + (source_dir_strlen + 1), song->file);
			if(!file_exists(source_file)) {
				printf("  -> ERROR: Source file does not exist: \"%s\"\n", source_file);
			}
			else {
				printf("  -> From \"%s\"\n", source_file);

				char *dir = dirname(target_file);
				char *cmd = malloc(strlen("mkdir -p \"\" ") + strlen(dir) + 1);
				sprintf(cmd, "mkdir -p \"%s\"", dir);
				system(cmd);
				free(cmd);

				cmd = (char *)malloc(strlen(source_file) + strlen(target_file) + strlen("/bin/cp \"\" \"\"") + 1);
				sprintf(cmd, "/bin/cp \"%s\" \"%s\"", source_file, target_file);
			  system(cmd);
				free(cmd);
			}
			free(source_file);
		}
		free(target_file);
	}

	mpd_disconnect(mpd);
	mpd_free(mpd);
	printf("Disconnected from MPD\n");
}

void help_msg() {
	printf("USAGE: disp_playlist [options] -P playlist -S source_dir -T target_dir\n");
	printf("    -h             Display this help message\n"
	       "    -v             Display the version\n"
		   "    -P playlist    Playlist to use\n"
			 "    -T target_dir  Directory in which to place the music\n"
			 "    -S source_dir  Directory containing the music in the MPD playlist\n"
		   );
}

int file_exists(char *filename) {
	if (access(filename, F_OK) == 0)
	{
		return 1;
	}
	return 0;
}
