#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"

/*
 * Taku Sakikawa
 * U1075644
 * CS440 - Fall 2018
 */

#define READ  (0)
#define WRITE (1)

static void run_tournament(int seed, int rounds, int num_progs, char **progs);
void start_player(pid_t pids[], int pl_pipeOut[][2],int pl_pipeIn[][2], int n,  char **progs );
void start_game_maker(int seed, int n, int gameMakerOut[], int gameMakerIn[] );

int is_wrong_player(int n, int player, int wrong_player[]);

void win_handler(char* buffer, int pl_pipeOut[][2], int pl_pipeIn[][2], int gameMakerOut[],int gameMakerIn[], int wrong_player[], int pids[], int n, int i);

static int done_flag = 0;
static int count = 0;
static int win_counter= 0;
static int wrong_counter = 0;

static int **game_result;
static int current_round;
static int play_nn;

pid_t pid, pid2;

void ctl_c_handler(int sigchld) {

  int f;
  sio_putl(current_round+1);
  sio_puts("\n");

  // game_result[0][0] = 0 ;
  for(f=0; f<play_nn  ; f++)
  {
    sio_putl(game_result[f][0]);
    sio_puts(" ");
    sio_putl(game_result[f][1]);
    sio_puts(" ");
    sio_putl(game_result[f][2]);
    sio_puts("\n");
  }
    _exit(0);
}


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

  // printf("argc - 3: %d\n",argc - 3 );
  run_tournament(seed, rounds, argc - 3, argv + 3);  /// argv + 3 = [scan_player, step_player]

  return 0;
}

static void run_tournament(int seed, int rounds, int n, char **progs)
{
  Signal(SIGPIPE, SIG_IGN);
  play_nn = n ;
  pid_t pids[n];
  int wrong_player[n];

  int f;

  game_result= (int **)malloc(n * sizeof(int *));
    for (f=0; f<n; f++)
         game_result[f] = (int *)malloc(3 * sizeof(int));

  // initialize the game result array
  for(f=0; f< n ; f++)
  {
    game_result[f][0] = 0 ; // win = 0
    game_result[f][1] = 0 ; // lose = 0
    game_result[f][2] = 0 ; // fault = 0
  }
  Signal(SIGINT, ctl_c_handler);

  sigset_t sigs;
  Sigemptyset(&sigs);
  Sigaddset(&sigs, SIGINT);


  for(current_round = 0; current_round < rounds; current_round++) // change 2 to rounds
  {
    Sigprocmask(SIG_BLOCK, &sigs, NULL);

    done_flag =0 ;
    count = 0;
    win_counter=0;
    wrong_counter= 0;

    int pl_pipeOut[n][2];
    int pl_pipeIn[n][2];

    int gameMakerOut[2];
    int gameMakerIn[2];

    Pipe(gameMakerOut);
    Pipe(gameMakerIn);


    if ((pid = Fork()) == 0)
      start_game_maker( seed,  n,  gameMakerOut,  gameMakerIn );
    else
    {
      char buffer[7*n+7+1];
      int nn = Rio_readn(gameMakerIn[READ], buffer, 7); // read the target position
      buffer[nn] = 0;

      start_player( pids,  pl_pipeOut, pl_pipeIn,  n, progs);
      int player_n = 0;
      int j;

      // close pipes that are not gonna be used and also initialize the wrong player array
      for(j = 0; j < n; j++)
      {
        wrong_player[j] = -1;
        close(pl_pipeOut[j][READ]);
        close(pl_pipeIn[j][WRITE]);
      }
      close(gameMakerOut[READ]);
      close(gameMakerIn[WRITE]);
      // close pipes that are not gonna be used and also initialize the wrong player array

      while(!done_flag) // iterate turn
      {
        int i ;
        for(i = 0 ; i < n ; i++) // iterate each player
        {
          // determine if the current player is a failture
          // if failture -> skip the player
          int not_wrong_player = is_wrong_player(n, i, wrong_player);

          int current_wrong= 0 ;
          if(!win_counter && !wrong_counter && not_wrong_player)
          {
            char buffer2[8];
            char buffer[8];

            int nn = Rio_readn(gameMakerIn[READ], buffer, 7); // read message from game maker
            buffer[nn] = 0;

            if(strcmp(buffer, "winner\n")==0) // if someone wins
              win_handler(buffer, pl_pipeOut,  pl_pipeIn,  gameMakerOut, gameMakerIn, wrong_player,  pids, n, i);\

            if(strcmp(buffer, "wrong!\n")==0) // if someone goes worng
            {
              current_wrong++;

              kill(pids[i], SIGKILL);
              // int status;

              game_result[i][1]++;
              game_result[i][2]++;

              wrong_player[player_n] = i;
              player_n++;

              if(player_n == n ) // everyone went wrong
              {
                // wait everyone and close all the pipe
                int i;
                close(gameMakerOut[WRITE]);
                close(gameMakerIn[READ]);
                int statusg;
                Waitpid(pid, &statusg, 0);

                for(i= 0; i < n; i++)
                {
                  close(pl_pipeOut[i][READ]);
                  close(pl_pipeIn[i][WRITE]);
                  close(pl_pipeOut[i][WRITE]);
                  close(pl_pipeIn[i][READ]);

                  kill(pids[i], SIGKILL);
                  int statusp;
                  Waitpid(pids[i], &statusp, 0);
                }

                wrong_counter++;
                done_flag++;
              }
            }


            if(strcmp(buffer, "winner\n")!=0 && !current_wrong )
            {
              Rio_writen(pl_pipeOut[i][WRITE], buffer, 7 ); // send game_maker message to player // send winner

              int nnn;
              nnn = Rio_readn(pl_pipeIn[i][READ], buffer2, 7);

              buffer2[nnn] = 0;
              Rio_writen(gameMakerOut[WRITE], buffer2, 7 ) ; // send player message to game maker
            }

            count++;
          }
        }
      } // end of 1 round
    } // end of else
    Sigprocmask(SIG_UNBLOCK, &sigs, NULL);
  }// end of all rounds

  // print out the result
  int i ;
  printf("%d\n",rounds );
  for(i = 0; i < n ; i++)
    printf("%d %d %d\n", game_result[i][0],game_result[i][1],game_result[i][2]);

}

void start_game_maker(int seed, int n, int gameMakerOut[], int gameMakerIn[] )
{
  char current_seed[10];
  sprintf(current_seed, "%d", seed+current_round);

  char num_of_player[10];
  sprintf(num_of_player, "%d", n);
  char *game_maker_argv[] = {"game_maker", current_seed, num_of_player, "40", "45", NULL };

  dup2(gameMakerOut[READ], 0);
  dup2(gameMakerIn[WRITE], 1);

  close(gameMakerOut[READ]);
  close(gameMakerIn[WRITE]);
  close(gameMakerOut[WRITE]);
  close(gameMakerIn[READ]);

  Setpgid(0, 0);
  Execve("game_maker", game_maker_argv, environ);
}

void start_player(pid_t pids[], int pl_pipeOut[][2],int pl_pipeIn[][2], int n,  char **progs )
{
  int i;
  for(i = 0; i < n; i++)
  {
    Pipe(pl_pipeOut[i]);
    Pipe(pl_pipeIn[i]);
    if((pids[i] = Fork()) == 0)
    {
      dup2(pl_pipeOut[i][READ], 0);
      dup2(pl_pipeIn[i][WRITE], 1);
      close(pl_pipeOut[i][WRITE]);
      close(pl_pipeIn[i][READ]);
      close(pl_pipeOut[i][READ]);
      close(pl_pipeIn[i][WRITE]);

      char *player_argv[] = {progs[i], NULL };
      Setpgid(0, 0);
      Execve(progs[i], player_argv, environ);
      // call player program
    }
  }
  return;
}


int is_wrong_player(int n, int player, int wrong_player[])
{
  int j;
  int result = 1;
  for(j = 0; j < n; j++)
  {
    if(wrong_player[j] == player)
      result = 0;
  }

  return result;
}

void win_handler(char* buffer, int pl_pipeOut[][2], int pl_pipeIn[][2], int gameMakerOut[],int gameMakerIn[], int wrong_player[], int pids[], int n, int i)
{
    Rio_writen(pl_pipeOut[i][WRITE], buffer, 7 ); // send game_maker message to player // send winner

    int status;
    Waitpid(pid, &status, 0);
    done_flag++;
    win_counter++;

    // force to terminate other player
    int p;
    for(p =0; p < n; p++) // iterate through all the players
    {
      if(p != i) // if the player is not the winner
      {
        int not_wrong_player = is_wrong_player(n, p, wrong_player);
        if(not_wrong_player)
          game_result[p][1]++; // increment the lose point

        // terminate the player and finish the process
        kill(pids[p], SIGKILL);
        int status;
        Waitpid(pids[p], &status, 0);
        // terminate the player and finish the process
      }
      else
      { // winner
        game_result[p][0]++; // increment win point

        int status;
        Waitpid(pids[p], &status, 0);

        // determine if the winning player exit with bad statas. (e.g. abort, exit(5))
        if (WIFEXITED(status))
        {
          if(WEXITSTATUS(status) > 0)
            game_result[p][2]++; // increment failture result
        }
        else if ( WIFSIGNALED(status))
          game_result[p][2]++; // increment failture result
      }

      close(pl_pipeOut[p][READ]);
      close(pl_pipeIn[p][WRITE]);
      close(pl_pipeOut[p][WRITE]);
      close(pl_pipeIn[p][READ]);

      close(gameMakerOut[WRITE]);
      close(gameMakerIn[READ]);
    }
}
