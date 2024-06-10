// vim: et sw=2 ts=2 sts=2

// Standard library includes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

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

int play_jingle_from_file(char *player,
                          char **player_args,
                          unsigned int args_count,
                          char *jingle_filename) {
  int jingle_fd = open(jingle_filename, O_RDONLY);
  if (jingle_fd == -1) {
    puts("ERROR: Failed to play jingle: Failed to open file!");
    return 2;
  }
  close(jingle_fd);

  switch(fork()) {
  case -1:
    puts("ERROR: Failed to play jingle: Failed to fork!");
    return 3;
  case 0:
    // Pipe output to /dev/null.
    int null_fd = open("/dev/null", O_WRONLY);
    if (null_fd == -1) {
      puts("ERROR: Failed to redirect play-jingle-output to /dev/null!");
      exit(4);
    }
    dup2(null_fd, 1);
    dup2(null_fd, 2);

    // Exec.
    if (args_count == 0 && strcmp(player, "/usr/bin/mpv") == 0) {
      if (execl(player, player, "--vid=no", jingle_filename, (char*)NULL) == -1) {
        printf("ERROR: Failed to play with player \"%s\" and jingle \"%s\"!\n", player, jingle_filename);
        close(null_fd);
        exit(5);
      }
    } else {
      // Every pointer has the same size, but for sanity use sizeof(char*).
      char **argvs = malloc(sizeof(char*) * (args_count + 3));
      memset(argvs, 0, sizeof(char*) * (args_count + 3));
      argvs[0] = player;
      printf("<%s> ", player);
      for (unsigned int i = 0; i < args_count; ++i) {
        argvs[1 + i] = player_args[i];
        printf("%s ", player_args[i]);
      }
      argvs[1 + args_count] = jingle_filename;
      printf("%s NULL\n", jingle_filename);
      if (execv(player, argvs) == -1) {
        printf("ERROR: Failed to play with player \"%s\"!\n", player);
        free(argvs);
        close(null_fd);
        exit(4);
      }
    }

    // This shouldn't run due to exec, but handle it anyway.
    close(null_fd);
    exit(0);
    break;
  default:
    // Intentionally left blank.
    break;
  }

  return 0;
}

/// Only supports playing with /usr/bin/mpv .
int play_jingle_from_memory(const char *jingle_data,
                            int jingle_size) {
  int pipe_filedes[2];
  if (pipe(pipe_filedes) != 0) {
    puts("ERROR: Failed to play jingle: Failed to create pipe!");
    return 1;
  }

  switch(fork()) {
  case -1:
    puts("ERROR: Failed to play jingle: Failed to fork!");
    return 2;
  case 0:
    // Handle pipe into stdin.
    close(pipe_filedes[1]);
    dup2(pipe_filedes[0], 0);

    // Pipe output to /dev/null.
    int null_fd = open("/dev/null", O_WRONLY);
    if (null_fd == -1) {
      puts("ERROR: Failed to redirect play-jingle-output to /dev/null!");
      close(pipe_filedes[0]);
      exit(3);
    }
    dup2(null_fd, 1);
    dup2(null_fd, 2);

    // Exec.
    if (execl("/usr/bin/mpv", "/usr/bin/mpv", "--vid=no", "-", (char*)NULL) == -1) {
      printf("ERROR: Failed to play!\n");
      close(pipe_filedes[0]);
      close(null_fd);
      exit(4);
    }

    // This shouldn't run due to exec, but handle it anyway.
    close(pipe_filedes[0]);
    close(null_fd);
    exit(0);
    break;
  default:
    close(pipe_filedes[0]);
    write(pipe_filedes[1], jingle_data, jingle_size);
    close(pipe_filedes[1]);
    break;
  }

  return 0;
}

void print_help(void) {
  puts("./program [minutes] [file_to_play_on_interval] [player_program] [player_args...]");
  puts("  minutes defaults to 5, file defaults to internal file, program defaults to \"/usr/bin/mpv\".");
}

int main(int argc, char **argv) {
   if (argc > 1 && (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0
       || strcmp(argv[1], "--help") == 0)) {
     print_help();
     return 0;
   }

  unsigned int minutes = 5;
  char *file_name = NULL;
  char *player_name = NULL;
  char **player_args = NULL;
  unsigned int args_count = 0;

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
  } else if (argc > 4) {
    minutes = atoi(argv[1]);
    file_name = argv[2];
    player_name = argv[3];
    player_args = argv + 4;
    args_count = argc - 4;
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

  if (file_name) {
    printf("Using file \"%s\"...\n", file_name);
  }

  // Setup for loop
  is_running = 1;

  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = interval_notification_handle_signal;
  if(sigaction(SIGINT, &action, NULL) != 0) {
    puts("ERROR: Failed to set handling of SIGINT!");
    return 2;
  }

  int ret;

  puts("Begin main loop...");
  while(is_running) {
    sleep(minutes * 60);
/*    sleep(10);*/
    if (!is_running) {
      break;
    }
    if (file_name) {
      ret = play_jingle_from_file(player_name ?
            player_name : DEFAULT_FILE_PLAYER_PROGRAM,
          player_args,
          args_count,
          file_name);
    } else {
      ret = play_jingle_from_memory(interval_notification_mp3,
                                    interval_notification_mp3_len);
    }
    if (ret != 0) {
      printf("ERROR: Failed to play jingle! (returned \"%i\")\n", ret);
      break;
    }
    printf(".");
    fflush(stdout);
  }

  return 0;
}
