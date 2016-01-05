#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <libmpd-1.0/libmpd/libmpd.h>

// See: http://api.sarine.nl/libmpd/

long quickhash(char *, char *);
int get_yesno();

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	MpdObj *mpd = mpd_new("localhost", 6600, NULL);
	if(mpd_connect(mpd)) {
		printf("Failed to connect to MPD\n");
		return 1;
	}
	printf("Connected to MPD\n");

	int pl_len = mpd_playlist_get_playlist_length(mpd);

	int *ids		= (int *)malloc(pl_len * sizeof(int));
	long *hashes	= (long *)malloc(pl_len * sizeof(long));


	MpdData *songs = mpd_playlist_get_song_from_pos_range(mpd, 0, pl_len - 1);
	int idx;
	for(songs = mpd_data_get_first(songs), idx = 0; songs != NULL; songs = mpd_data_get_next(songs), idx++)
	{
		int i;
		ids[idx] = songs->song->id;
		for(i = 0; i < idx; i++) {
			if(ids[i] == ids[idx]) {
				printf("Found duplicate: %i: %s - %s\n", songs->song->id, songs->song->artist, songs->song->title);
			}
		}

		hashes[idx] = quickhash(songs->song->title, songs->song->artist);
		for(i = 0; i < idx; i++) {
			if(hashes[i] == hashes[idx]) {
				mpd_Song *dup1 = mpd_playlist_get_song(mpd, ids[i]);
				mpd_Song *dup2 = songs->song;

				if(dup1 == NULL) continue;
				if(dup2 == NULL) continue;

				printf("\e[33mHashes match of ids: \e[32m%d \e[33m& \e[32m%d\e[0m\n", ids[idx], ids[i]);
				printf("\e[33mPossible duplicate:\e[0m\n");
				printf("  \e[33mSong1: \e[32m%s \e[33m- \e[32m%s\e[0m\n", dup1->artist, dup1->title);
				printf("  \e[33mSong2: \e[32m%s \e[33m- \e[32m%s\e[0m\n", dup2->artist, dup2->title);

				printf("remove Song2? (y/n):");
				if(get_yesno()) {
					printf("  \e[91mRemoving song...\e[0m\n");
					int err = mpd_playlist_delete_id(mpd, ids[idx]);
					if(err == MPD_OK) {
						printf("  \e[32m    Song removed\e[0m\n");
					} else {
						printf("  \e[31m    Song could not be removes\e[0m\n");
					}
				}
				printf("\n");
			}
		}
	}

	free(ids);
	free(hashes);

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
