// vim: et sw=2 ts=2 sts=2

// Standard library includes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// Local includes.
#include "interval_notification.h"

#define DEFAULT_FILE_PLAYER_PROGRAM "/usr/bin/mpv"

int is_running;

void interval_notification_handle_signal(int sig) {
  if (sig == SIGINT) {
    is_running = 0;
    puts("Handled SIGINT...");
  }
}

void print_help(void) {
  puts("./program [minutes] [file_to_play_on_interval] [player_program]");
  puts("  minutes defaults to 5, file defaults to internal file, program defaults to \"/usr/bin/mpv\".");
}

int main(int argc, char **argv) {
   if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0
       || strcmp(argv[1], "--help") == 0) {
     print_help();
     return 0;
   }

  unsigned int minutes = 5;
  char *file_name = NULL;
  char *player_name = NULL;

  if (argc == 1) {
    // Intentionally left blank.
  } else if (argc == 2) {
    if (!(argv[1][0] >= '0' && argv[1][0] <= '9')) {
      puts("ERROR: Expected positive integer for arg!");
      print_help();
      return 4;
    }
    minutes = atoi(argv[1]);
  } else if (argc == 3) {
    minutes = atoi(argv[1]);
    file_name = argv[2];
  } else if (argc == 4) {
    minutes = atoi(argv[1]);
    file_name = argv[2];
    player_name = argv[3];
  }

  if (minutes == 0) {
    puts("ERROR: Minutes cannot be set to zero!");
    print_help();
    return 3;
  }

  printf("Set to %u minutes...\n", minutes);
  if (player_name) {
    printf("Using player \"%s\"...\n", player_name);
  }

  char *play_audio_command;
  char *temp_filename = NULL;

  if (file_name) {
    int len = strlen(file_name) + 1 + 1;
    if (player_name) {
      len += strlen(player_name) + 10;
    } else {
      len += 14;
    }
    play_audio_command = malloc(len);
    snprintf(play_audio_command, len, "%s %s",
        player_name ? player_name : DEFAULT_FILE_PLAYER_PROGRAM, file_name);
  } else {
    int len = 5 + 1 + strlen(getenv("USER")) + 7 + 1 + 25 + 10;
    temp_filename = malloc(len);
    snprintf(temp_filename, len, "/tmp/%s%07lu_interval_audio_file.mp3",
             getenv("USER"),
             random() % 10000000);
    FILE *fd = fopen(temp_filename, "wb");
    if (fwrite(interval_notification_mp3, 1, interval_notification_mp3_len, fd)
        != interval_notification_mp3_len) {
      free(temp_filename);
      fclose(fd);
      puts("ERROR: Failed to save temporary interval audio file to /tmp/!");
      return 1;
    }
    fclose(fd);

    len = strlen(temp_filename) + 1 + 10;
    if (player_name) {
      len += strlen(player_name) + 10;
    } else {
      len += 14;
    }
    play_audio_command = malloc(len);
    snprintf(play_audio_command, len, "%s %s",
        player_name ? player_name : DEFAULT_FILE_PLAYER_PROGRAM, temp_filename);
  }

  // Setup for loop
  is_running = 1;

  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = interval_notification_handle_signal;
  if(sigaction(SIGINT, &action, NULL) != 0) {
    free(play_audio_command);
    puts("ERROR: Failed to set handling of SIGINT!");
    if (temp_filename) {
      unlink(temp_filename);
      free(temp_filename);
    }
    return 2;
  }

  int ret;

  puts("Begin main loop...");
  while(is_running) {
    sleep(minutes * 60);
/*    sleep(4);*/
    if (!is_running) {
      break;
    }
    ret = system(play_audio_command);
    if (ret != 0) {
      printf("ERROR: Executing \"%s\" failed! (returned \"%i\")\n", play_audio_command, ret);
      break;
    }
    printf(".");
  }

  free(play_audio_command);
  if (temp_filename) {
    unlink(temp_filename);
    free(temp_filename);
  }
  return 0;
}
