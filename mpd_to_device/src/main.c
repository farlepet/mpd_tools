#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include <libmpd-1.0/libmpd/libmpd.h>

// See: http://api.sarine.nl/libmpd/

#define VERSION "0.1"
#define MAX_PLAYLISTS 16

char *playlists[MAX_PLAYLISTS] = { NULL };
char *target_dir   = NULL;
char *source_dir   = NULL;
char *playlist_dir = NULL;
char *music_dir    = ""; // Prepended string for playlists

int n_playlists = 0;

void help_msg();
int file_exists(char *);

int main(int argc, char **argv) {
	int argn = 1;
	for(; argn < argc; argn++) {
		char *arg = argv[argn];
		if(arg[0] == '-') {
			switch(arg[1]) {
				case 'h':
					help_msg();
					return 0;
				
				case 'v':
					printf("VERSION: "VERSION"\n");
					return 0;
				
				case 'P':
					if(argn == (argc - 1)) {
						printf("You must specify a playlist as an argument to -P!\n");
						return 1;
					}
					argn++;
					playlists[n_playlists] = argv[argn];
					n_playlists++;
					break;
				
				case 'S':
					if(argn == (argc - 1)) {
						printf("You must specify a source directory as an argument to -S!\n");
						return 1;
					}
					argn++;
					source_dir = argv[argn];
					break;
				
				case 'T':
					if(argn == (argc - 1)) {
						printf("You must specify a target directory as an argument to -T!\n");
						return 1;
					}
					argn++;
					target_dir = argv[argn];
					break;
				
				case 'D':
					if(argn == (argc - 1)) {
						printf("You must specify the devices relative music directory as an argument to -D!\n");
						return 1;
					}
					argn++;
					music_dir = argv[argn];
					break;
				
				case 'd':
					if(argn == (argc - 1)) {
						printf("You must specify the target playlist directory as an argument to -d!\n");
						return 1;
					}
					argn++;
					playlist_dir = argv[argn];
					break;
				
				default:
					printf("Unrecognized argument: %s\n\n", arg);
					help_msg();
					return 1;
			}
		} else {
			printf("Unrecognized argument: %s\n\n", arg);
			help_msg();
			return 1;
		}
	}

	
	
	
	if(n_playlists < 1) {
		printf("You must specify at least one playlist (-P playlist)\n");
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
	
	if(playlist_dir == NULL) {
		playlist_dir = target_dir;
	}



	MpdObj *mpd = mpd_new("localhost", 6600, NULL);
	if(mpd_connect(mpd)) {
		printf("Failed to connect to MPD\n");
		return 1;
	}
	printf("Connected to MPD\n\n");

	

	size_t target_dir_strlen = strlen(target_dir);
	size_t source_dir_strlen = strlen(source_dir);
	
	int idx = 0;
	// Copy songs first:
	for(; idx < n_playlists; idx++) {
		printf("Copying songs for %s\n", playlists[idx]);
		MpdData *songs = mpd_database_get_playlist_content(mpd, playlists[idx]);
		if(songs == NULL) {
			printf("  Could not retrieve songs from playlist!\n");
		}
		
		for(songs = mpd_data_get_first(songs); songs != NULL; songs = mpd_data_get_next(songs)) {
			mpd_Song *song = songs->song;
			char *target_file = (char *)malloc(target_dir_strlen + strlen(song->file) + 2);
			strcpy(target_file, target_dir);
			target_file[target_dir_strlen] = '/';
			strcpy(target_file + (target_dir_strlen + 1), song->file);
			if(!file_exists(target_file)) {
				printf("  Copying song to %s\n", target_file);

				char *source_file = (char *)malloc(source_dir_strlen + strlen(song->file) + 2);
				strcpy(source_file, source_dir);
				source_file[source_dir_strlen] = '/';
				strcpy(source_file + (source_dir_strlen + 1), song->file);
				if(!file_exists(source_file)) {
					printf("    -> ERROR: Source file does not exist: \"%s\"\n", source_file);
				}
				else {
					printf("    -> From \"%s\"\n", source_file);

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
	}
	printf("\n");
	// Then create playlist files:
	for(idx = 0; idx < n_playlists; idx++) {
		printf("Creating m3u8 for %s\n", playlists[idx]);
		MpdData *songs = mpd_database_get_playlist_content(mpd, playlists[idx]);
		if(songs == NULL) {
			printf("  Could not retrieve songs from playlist!\n");
		}
		
		char *pl_fname = (char *)malloc(strlen(playlist_dir) + strlen(playlists[idx]) + 6);
		sprintf(pl_fname, "%s/%s.m3u8", playlist_dir, playlists[idx]);
		FILE *pl_file = fopen(pl_fname, "w");
		if(pl_file != NULL) {
			for(songs = mpd_data_get_first(songs); songs != NULL; songs = mpd_data_get_next(songs)) {
				mpd_Song *song = songs->song;
				fprintf(pl_file, "%s/%s\n", music_dir, song->file);
			}
		
			fclose(pl_file);
		} else {
			printf("  Could not open file for writing: %s\n", pl_fname);
		}
		free(pl_fname);
	}

	mpd_disconnect(mpd);
	mpd_free(mpd);
	printf("\nDisconnected from MPD\n");
}

void help_msg() {
	printf("USAGE: to_device [options] -S source_dir -T target_dir -P playlist1 [-P playlist2 ... ]\n"
			"    -h              Display this help message\n"
			"    -v              Display the version\n"
			"    -P playlist     Playlist to use\n"
			"    -T target_dir   Directory in which to place the music\n"
			"    -S source_dir   Directory containing the music in the MPD playlist\n"
			"    -d playlist_dir Directory in which to create playlists (defaults to `target_dir`)\n"
			"    -D34 music_dir    Devices relative music directory\n"
			);
}

int file_exists(char *filename) {
	if (access(filename, F_OK) == 0)
	{
		return 1;
	}
	return 0;
}
