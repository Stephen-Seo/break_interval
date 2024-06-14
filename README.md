# break\_interval

This simple program plays a jingle at a specified interval in minutes. It is
meant to be used as a way to notify the user to take a break when the interval
jingle plays.

    # For usage:
    > ./break_interval --help
    ./program [minutes] [file_to_play_on_interval] [player_program] [player_args...]
      minutes defaults to 5, file defaults to internal file, program defaults to "/usr/bin/mpv".
    Note that this program uses execl/execv, so if a program is specified, it must be the full path to the program.

Running the program with no args will default to playing a jingle every 5
minutes.

If `[player_program]` is specified (with full path) like with
`/usr/bin/ffplay`, then the command executed will look like:

    /usr/bin/ffplay <file_to_play>

If `[player_args...]` is specified like with `--flag` and `--another-flag`, then
the command executed will look like:

    /usr/bin/ffplay --flag --another-flag <file_to_play>

It may be recommended to use a command like:

    ./break_interval 5 /my/music/file.mp3 /usr/bin/ffplay -nodisp

Which will execute (every 5 minutes):

    /usr/bin/ffplay -nodisp /my/music/file.mp3

To play the internal file instead of a specified one, use the parameter
"INTERNAL_FILE" for the filename and break\_interval will play the internal
file.

    ./break_interval 5 INTERNAL_FILE /usr/bin/ffplay -nodisp
