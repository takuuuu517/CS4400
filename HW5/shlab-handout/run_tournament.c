#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"

#define READ  (0)
#define WRITE (1)

static void run_tournament(int seed, int rounds, int num_progs, char **progs);
void start_player(pid_t pids[], int pl_pipeOut[][2],int pl_pipeIn[][2], int n,  char **progs );
void remove_element(int *array, int index, int array_length);
void remove_row(int array[][2], int row, int row_n, int col_n );
static int done_flag = 0;
static int count = 0;
// static int wrong_player = -1;

static int win_counter= 0;
static int wrong_counter = 0;


// ctrl c
static int **game_result;
// static int rround;
static int current_round;
static int play_nn;

  // static int game_result[][3];
  // static int nn == 0 ;
//

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

void done(int sigchld) {
  sio_puts("done\n");

  int status;
  pid_t p = Wait(&status);
  sio_puts("pid:  ");
  char pp[10];
  sprintf(pp, "%d", p);
  sio_puts(pp);
  sio_puts("\n");


  return;
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
  play_nn = n ;
  // Signal(SIGCHLD, done);
  pid_t pids[n];
  int wrong_player[n];

  // どっかのタイミングでgame maker プログラムをやる。
  // int current_round;
  char buf[7];



  // int game_result[n][3]; // player1(n):[win_count][lose_count][fail_count]
  int f;

  // static int **game_result;
  game_result= (int **)malloc(n * sizeof(int *));
    for (f=0; f<n; f++)
         game_result[f] = (int *)malloc(3 * sizeof(int));

  // initialize the game result array
  // int f;
  for(f=0; f< n ; f++)
  {
    game_result[f][0] = 0 ; // win = 0
    game_result[f][1] = 0 ; // lose = 0
    game_result[f][2] = 0 ; // fault = 0
    wrong_player[f] = -1;
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
    // printf("current_round:  %d\n", current_round );


    int pl_pipeOut[n][2];
    int pl_pipeIn[n][2];

    // 子から親への通信用パイプ
    int gameMakerOut[2];
    int gameMakerIn[2];
    //
    //
    Pipe(gameMakerOut);
    Pipe(gameMakerIn);




    if ((pid = Fork()) == 0)
    { // child -> call game maker

      // game maker

        char current_seed[10];
        sprintf(current_seed, "%d", seed+current_round);

        char num_of_player[10];
        sprintf(num_of_player, "%d", n);
        char *game_maker_argv[] = {"game_maker", current_seed, num_of_player, "40", "45", NULL };


        // 親→子への出力を標準入力として割り当て
        dup2(gameMakerOut[READ], 0);
        // 子→親への入力を標準出力に割り当て
        dup2(gameMakerIn[WRITE], 1);
        // 割り当てたファイルディスクリプタは閉じる


        close(gameMakerOut[READ]);
        close(gameMakerIn[WRITE]);
        close(gameMakerOut[WRITE]);
        close(gameMakerIn[READ]);

        Setpgid(0, 0);
        Execve("game_maker", game_maker_argv, environ);
    }
    else
    {
      // printf("%d\n",pid );

      char buffer[7*n+7+1];
      char buffer2[7*n+7+1];
      char buffer3[7*n+7+1];
      int nn = Rio_readn(gameMakerIn[READ], buffer, 7);
      buffer[nn] = 0;
      // printf("\ncurrent_round: %d\n",current_round );
      // printf("target: %s",buffer );



      //  nn = Rio_readn(gameMakerIn[READ], buffer2, 7);
      // buffer2[nn] = 0;
      // printf("game maker message: %s",buffer2 );
      //
      //  nn= Rio_readn(gameMakerIn[READ], buffer3, 7);
      // buffer3[nn] = 0;
      // printf("game maker message: %s",buffer3 );

      // close(gameMakerOut[READ]);
      // close(gameMakerOut[WRITE]);



      start_player( pids,  pl_pipeOut, pl_pipeIn,  n, progs);
      int player_n = 0;
      int j;
      for(j = 0; j < n; j++)
      {
        wrong_player[j] = -1;

        close(pl_pipeOut[j][READ]);
        close(pl_pipeIn[j][WRITE]);
      }

      close(gameMakerOut[READ]);
      close(gameMakerIn[WRITE]);

      while(!done_flag) // iterate turn
      {
        // if(wrong_player != -1)
        // {
        //   printf("%d\n",wrong_player );
        //   remove_element(pids, wrong_player, player_n);
        //   close(pl_pipeOut[wrong_player][READ]);
        //   close(pl_pipeIn[wrong_player][WRITE]);
        //   close(pl_pipeOut[wrong_player][WRITE]);
        //   close(pl_pipeIn[wrong_player][READ]);
        //
        //   remove_row(pl_pipeIn, wrong_player, player_n , 2);
        //   remove_row(pl_pipeOut, wrong_player, player_n , 2);
        //   player_n--; // decrement player number
        //   if(player_n == 0 ) // everyone went wrong
        //   {
        //     wrong_counter++;
        //     done_flag++;
        //   }
        //   kill(pids[wrong_player], SIGKILL);
        //   int status;
        //   pid_t players_pid = Waitpid(pids[wrong_player], &status, 0);
        //   wrong_player = -1;
        // }
        int i ;
        for(i = 0 ; i < n ; i++) // iterate each player
        {
          int j, f =1 ;

          // determine if the current player is a failture
          // if failture -> skip the player
          for(j = 0; j < n; j++)
          {
            if(wrong_player[j] == i)
              f = 0;
          }

          int current_wrong= 0 ;
          if(!win_counter && !wrong_counter && f)
          {
            char buffer2[8];
            char buffer[8];
            // printf("\nturn:   %d\n", count );
            // printf("player_n:  %d\n", n);
            // printf("testing\n" );


            int nn = Rio_readn(gameMakerIn[READ], buffer, 7); // read message from game maker
            buffer[nn] = 0;
            // if(count == 0 )
            // {
              // printf("game maker message: %s",buffer );
              //
              // printf("game_result: %d\n",game_result[i][2] );

            // }
            // printf("player:    %d\n", i);
            // Rio_writen(pl_pipeOut[i][WRITE], buffer, 7 ); // send game_maker message to player // send winner
//

            if(strcmp(buffer, "winner\n")==0) // if someone wins
            {
              Rio_writen(pl_pipeOut[i][WRITE], buffer, 7 ); // send game_maker message to player // send winner

              // printf("winwinwinwinwinwinwinwinwinwinwinwinwinwinwinwin\n" );
              int status;
              pid_t game_p = Waitpid(pid, &status, 0);
              // printf("\n\ncurrent_round:  %d\n",current_round );
              done_flag++;
              win_counter++;


              // printf("game pid:  %d\n", pid);
              // printf("game done pid:  %d\n", game_p);

              close(gameMakerOut[WRITE]);
              close(gameMakerIn[READ]);
              // close(gameMakerOut[READ]);
              // close(gameMakerIn[WRITE]);

              // force to terminate other player
              int p;
              for(p =0; p < n; p++)
              {
                // printf("pid[]: %d\n",pids[p] );
                pid_t player_pid;
                if(p != i)
                {
                  int t = 1;
                  for(j = 0; j < n; j++)
                  {
                    // printf("wrong_player[j]:   %d\n",wrong_player[j] );
                    if(wrong_player[j] == p)
                      t = 0;
                  }
                  if(t)
                    game_result[p][1]++;
                  // game_result[p][1]++; // increment lose count
                  kill(pids[p], SIGKILL);
                  int status;
                  player_pid = Waitpid(pids[p], &status, 0);
                  // printf("player done pid1:  %d\n", player_pid);
                  // printf("%d\n",WEXITSTATUS(status) );
                }
                else
                { // winner
                  game_result[p][0]++; // increment win result

                  int status = 0;
                   player_pid = Waitpid(pids[p], &status, 0);
                   // printf("player done pid2:  %d\n", player_pid);



                   if (WIFEXITED(status)){
                     if(WEXITSTATUS(status) > 0)
                        game_result[p][2]++; // increment failture result
                      }
                   else if ( WIFSIGNALED(status))
                   {
                     game_result[p][2]++; // increment failture result
                     // printf("%sasdf\n" );
                      // psignal(WTERMSIG(status), "Exit signal");

                      // printf("%d\n",WTERMSIG(status) );
                   }
                }

                close(pl_pipeOut[p][READ]);
                close(pl_pipeIn[p][WRITE]);
                close(pl_pipeOut[p][WRITE]);
                close(pl_pipeIn[p][READ]);
              }
            } // end of if winner

            if(strcmp(buffer, "wrong!\n")==0) // if someone goes worng
            {
              current_wrong++;
              // printf("wrong wrong wrong wrong  wrong wrong wrong wrong wrong wrong wrong\n" );

              kill(pids[i], SIGKILL);
              int status;
              // pid_t players_pid = Waitpid(pids[i], &status, 0);

              game_result[i][1]++;
              game_result[i][2]++;


              wrong_player[player_n] = i;
              player_n++;
              // wrong_player = i;
              // remove_element(pids, i, player_n);
              // close(pl_pipeOut[i][READ]);
              // close(pl_pipeIn[i][WRITE]);
              // close(pl_pipeOut[i][WRITE]);
              // close(pl_pipeIn[i][READ]);
              //
              // remove_row(pl_pipeIn, i, player_n , 2);
              // remove_row(pl_pipeOut, i, player_n , 2);

              if(player_n == n ) // everyone went wrong
              {
                int i;
                close(gameMakerOut[WRITE]);
                close(gameMakerIn[READ]);
                int statusg;
                pid_t game_p = Waitpid(pid, &statusg, 0);



                for(i= 0; i < n; i++)
                {
                  close(pl_pipeOut[i][READ]);
                  close(pl_pipeIn[i][WRITE]);
                  close(pl_pipeOut[i][WRITE]);
                  close(pl_pipeIn[i][READ]);

                  kill(pids[i], SIGKILL);
                  int statusp;
                  pid_t player_pid = Waitpid(pids[i], &statusp, 0);
                }


                // wait everyone and close all the pipe



                // printf("%s\n","alksdfhaksdhfk;ahsdf" );
                wrong_counter++;
                done_flag++;
              }
            }


            if(strcmp(buffer, "winner\n")!=0 && !current_wrong )
            {
              Rio_writen(pl_pipeOut[i][WRITE], buffer, 7 ); // send game_maker message to player // send winner

              int nnn;
              // if(current_round == 7)
              // {
                // printf("stack1\n" );

                nnn = Rio_readn(pl_pipeIn[i][READ], buffer2, 7);
                // printf("stack2\n" );

              // }
              // else
              // {
                // does not work if a player sends 0 byte message because it waits forever
                 // nnn = Read(pl_pipeIn[i][READ], buffer2, 7);  // read message from player // read winner
              // }


              // printf("nnn:  %d\n",nnn );
              // printf("stack1\n" );

              buffer2[nnn] = 0;
              // sio_puts(buffer2);
              // printf("player message: %s",buffer2 );
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

void remove_element(int *array, int index, int array_length)
{
   int i;
   for(i = index; i < array_length - 1; i++)
    array[i] = array[i + 1];
}

void remove_row(int array[][2], int row, int row_n, int col_n )
{
  int i,j;
  for(i = row; i< row_n; i++)
    for(j = 0 ; j< col_n; j++)
      array[i][j] = array[i+1][j];
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
