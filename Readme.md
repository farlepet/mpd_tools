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


example: 
`mpd_copy_pl_files -S ~/Music -T /mnt/usb/Music -P "Example Playlist"`

### mpd_create_m3u8
Create a m3u8 playlist from a MPD playlist with a specified music base directory. Useful when used in conjunction with `mpd_copy_pl_files`.

example:
`mpd_create_m3u8 -P "Example Playlist" -D /Music > /mnt/usb/Playlists/ex_playlist.m3u8`

### mpd_create_csv
Create a CSV file from an MPD playlist in the format `title,artist,album,` This format is intended to be used with gmusic-playlist (https://github.com/soulfx/gmusic-playlist) or similar.

example:
`mpd_create_m3u8 -P "Example Playlist" > ex_playlist.csv`

### mpd_to_device
Copies songs to a device and creates playlist files on the device.

example:
`mpd_to_device -S ~/Music -T /mnt/usb/Music -d /mnt/usb/Playlists -D /Music -P "Playlist 1" -P "Playlist 2" -P "Playlist 3"`