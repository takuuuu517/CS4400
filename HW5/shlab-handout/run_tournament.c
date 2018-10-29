#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"

static void run_tournament(int seed, int rounds, int num_progs, char **progs);

int main(int argc, char **argv)
{
  int rounds, seed;

  if (argc < 4) {
    fprintf(stderr, "%s: expects <random-seed> <round-count> <player-program> <player-program> ...\n", argv[0]);
    return 1;
  }
      
  seed = atoi(argv[1]);
  if (seed < 1) {
    fprintf(stderr, "%s: bad random seed; not a positive number: %s\n", argv[0], argv[1]);
    return 1;
  }

  rounds = atoi(argv[2]);
  if (rounds < 1) {
    fprintf(stderr, "%s: bad round count; not a positive number: %s\n", argv[0], argv[2]);
    return 1;
  }
  
  run_tournament(seed, rounds, argc - 3, argv + 3);

  return 0;
}

static void run_tournament(int seed, int rounds, int n, char **progs)
{
  fprintf(stderr, "run tournament is not implemented\n");
}
