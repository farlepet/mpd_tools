#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <libmpd-1.0/libmpd/libmpd.h>

// See: http://api.sarine.nl/libmpd/

#define VERSION "0.1"

char *song_fmt = "%A - %T";

enum {
	NOTHING,
	LIST_PLAYLISTS,
	LIST_PLAYLIST_SONGS
} task = NOTHING;

char *playlist = NULL;

void help_msg();
void print_song(mpd_Song *, char *);

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
		} else if(!strcmp(arg, "-l")) {
			task = LIST_PLAYLISTS;
		} else if(!strcmp(arg, "-f")) {
			if(argn == (argc - 1)) {
				printf("You must specify a format string!\n");
				return 1;
			}
			argn++;
			song_fmt = argv[argn];
		} else if(!strcmp(arg, "-L")) {
			task = LIST_PLAYLIST_SONGS;
			if(argn == (argc - 1)) {
				printf("You must specify a playlist!\n");
				return 1;
			}
			argn++;
			playlist = argv[argn];
		} else {
			printf("Unrecognized argument: %s\n\n", arg);
			help_msg();
			return 1;
		}
	}

	if(task == NOTHING) {
		printf("Nothing to do, exiting.\n");
		return 0;
	}
	
	MpdObj *mpd = mpd_new("localhost", 6600, NULL);
	if(mpd_connect(mpd)) {
		printf("Failed to connect to MPD\n");
		return 1;
	}
	
	switch(task) {
		case LIST_PLAYLISTS: {
			MpdData *playlists = mpd_database_playlist_list(mpd);
			for(playlists = mpd_data_get_first(playlists); playlists != NULL; playlists = mpd_data_get_next(playlists)) {
				printf("%s\n", playlists->playlist->path);
			}
			} break;

		case LIST_PLAYLIST_SONGS: {
			MpdData *songs = mpd_database_get_playlist_content(mpd, playlist);
			for(songs = mpd_data_get_first(songs); songs != NULL; songs = mpd_data_get_next(songs)) {
				print_song(songs->song, NULL);
			}
			} break;
		
		default:
			printf("Nothing to do, exiting\n");
			break;
		
	}
		//MpdData *songs = mpd_database_get_playlist_content(mpd, playlists->playlist->path);
		//for(songs = mpd_data_get_first(songs); songs != NULL; songs = mpd_data_get_next(songs)) {
		//	printf("  %s - %s\n", songs->song->artist, songs->song->title);
		//}
	//}

	mpd_disconnect(mpd);
	mpd_free(mpd);
}

void help_msg() {
	printf("USAGE: disp_playlist [Options]\n");
	printf("    -h             Display this help message\n"
	       "    -v             Display the version\n"
	       "    -l             List playlists\n"
		   "    -L [Playlist]  List all songs in the specified playlist\n"
		   "    -f [Format]    Format to use when printing song names (see below)\n"
		   "\n"
		   "  Format string specifiers:\n"
		   "    %%T Title        %%f file\n"
		   "    %%a Album        %%A Artist\n"
		   "    %%l Album Artist %%c Composer\n"
		   "    %%g Genre        %%n Name\n"
		   "    %%t Track        %%d Date\n"
		   "    %%D Disc         %%p Performer\n"
		   "    %%C Comment      %%L Length\n"
		   "    %%P Position     %%i ID\n"
		   );
}

void print_song(mpd_Song *song, char *prepend) {
	if(prepend != NULL) printf(prepend);
	
	// TODO: insert printing based on `song_fmt`!

	/*
	char *	file
	char *	artist
	char *	title
	char *	album
	char *	track
	char *	name
	char *	date
	char *	genre
	char *	composer
	char *	performer
	char *	disc
	char *	comment
	char *	albumartist
	int		time
	int		pos
	int		id
	*/

	unsigned int i = 0;
	while(song_fmt[i] != 0) {
		switch(song_fmt[i]) {
			case '%':
				i++;
				switch(song_fmt[i]) {
					case 'T': // Title
						printf("%s", song->title);
						break;

					case 'f': // File
						printf("%s", song->file);
						break;

					case 'a': // Album
						printf("%s", song->album);
						break;

					case 'A': // Artist
						printf("%s", song->artist);
						break;

					case 'l': // A(l)bum Artist
						printf("%s", song->albumartist);
						break;

					case 'c': // Composer
						printf("%s", song->composer);
						break;

					case 'g': // Genre
						printf("%s", song->genre);
						break;

					case 'n': // Name
						printf("%s", song->name);
						break;

					case 't': // Track
						printf("%s", song->track);
						break;

					case 'd': // Date
						printf("%s", song->date);
						break;

					case 'D': // Disc
						printf("%s", song->disc);
						break;

					case 'p': // Performer
						printf("%s", song->performer);
						break;

					case 'C': // Comment
						printf("%s", song->comment);
						break;

					case 'L': // Length
						printf("%d", song->time);
						break;

					case 'P': // Position
						printf("%d", song->pos);
						break;

					case 'i': // ID
						printf("%d", song->id);
						break;

					default:
						printf("%%%c", song_fmt[i]);
						break;
				}
				break;

			case '\\':
				break;

			default:
				printf("%c", song_fmt[i]);
				break;
		}
		i++;
	}
	printf("\n");
}
