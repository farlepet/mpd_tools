# mpd_tools
`mpd_tools` is a set of programs for interacting with MPD and your music library.

Requirements:
 * `libmpd`

## Included tools

### mpd_disp_playlist
List playlists and/or the songs within them.

### mpd_dup_find
Find duplicates withing playlists and remove them if wanted.

### mpd_copy_pl_files
Copy all the songs in a playlist to a directory, if they do not already exist. This is very useful if you are copying playlists to a media player.

### mpd_create_m3u8
Create a m3u8 playlist from a MPD playlist with a specified music base directory. Useful when used in conjunction with `mpd_copy_pl_files`.
