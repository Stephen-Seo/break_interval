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
