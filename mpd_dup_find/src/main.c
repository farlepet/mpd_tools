#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <libmpd-1.0/libmpd/libmpd.h>

// See: http://api.sarine.nl/libmpd/

long quickhash(char *, char *);
int get_yesno();
void help_msg();

char *playlist = NULL;

int main(int argc, char **argv) {
	int argn = 1;
	for(; argn < argc; argn++) {
		char *arg = argv[argn];
		if(arg[0] == '-') {
			switch(arg[1]) {
				case 'h': // Help Message
					help_msg();
					return 0;

				case 'P': // Playlist
					if(argn == (argc - 1)) {
						printf("You must specify a playlist!\n");
						return 1;
					}
					argn++; playlist = argv[argn];
					break;

				default:
					printf("Unrecognized argument: %s\n", arg);
					return 1;
			}
		}
	}

	if(playlist == NULL) {
		printf("You must specify a playlist!\n");
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
		return 1;
	}

	int pl_len = 1; // The following loop will not add one for the last one, so we are assuming the playlist has at least one song
	for(songs = mpd_data_get_first(songs); !mpd_data_is_last(songs); songs = mpd_data_get_next(songs), pl_len++);

	long *hashes	  = (long *)malloc(pl_len * sizeof(long));
	mpd_Song **ssongs = (mpd_Song **)malloc(pl_len * sizeof(mpd_Song *));
	int *del_idxs     = (int *)malloc((pl_len / 2) * sizeof(int));
	int del_idx = 0; del_idxs[0] = -1;

	//MpdData *songs = mpd_playlist_get_song_from_pos_range(mpd, 0, pl_len - 1);
	int idx;
	for(songs = mpd_data_get_first(songs), idx = 0; songs != NULL; songs = mpd_data_get_next(songs), idx++)
	{
		ssongs[idx] = songs->song;
		hashes[idx] = quickhash(songs->song->title, songs->song->artist);
		
		int i;
		for(i = 0; i < idx; i++) {
			if(hashes[i] == hashes[idx]) {
				mpd_Song *dup1 = ssongs[i];
				mpd_Song *dup2 = songs->song;

				if(dup1 == NULL) continue;
				if(dup2 == NULL) continue;

				printf("\e[33mHashes match of indexes: \e[32m%d \e[33m& \e[32m%d\e[0m\n", idx, i);
				printf("\e[33mPossible duplicate:\e[0m\n");
				printf("  \e[33mSong1: \e[32m%s \e[33m- \e[32m%s\e[0m\n", dup1->artist, dup1->title);
				printf("  \e[33mSong2: \e[32m%s \e[33m- \e[32m%s\e[0m\n", dup2->artist, dup2->title);

				printf("remove Song2? (y/n):");
				if(get_yesno()) {
					del_idxs[del_idx] = idx;
					del_idx++;
					del_idxs[del_idx] = -1;
				}
				printf("\n");
				break;
			}
		}
	}

	free(ssongs);
	free(hashes);

	int i;
	for(i = del_idx - 1; i >= 0; i--) {
		if(del_idxs[i] == -1) break;
		printf("Deleting idx %d\n", del_idxs[i]);
		mpd_database_playlist_list_delete(mpd, playlist, del_idxs[i]);
	}

	printf("Disconnecting from MPD server\n");
	mpd_disconnect(mpd);
	mpd_free(mpd);

	return 0;
}

long quickhash(char *str1, char *str2) {
	long hash = 0;
	unsigned int i;
	for(i = 0; i < strlen(str1); i++) {
		hash += (int)str1[i];
	}
	for(i = 0; i < strlen(str2); i++) {
		hash *= (str2[i] - 30);
	}
	return hash;
}

int get_yesno() {
	char yn;
	scanf(" %c",&yn);
	if(tolower(yn) == 'y') return 1;
	else return 0;
}

void help_msg() {
	printf("USAGE: mpd_dup_find [options] -P playlist\n"
		   "	-h          Display this help message\n"
		   "	-P playlist Playlist in which to check for duplicates\n"
		   );
}
