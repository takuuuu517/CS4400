#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
  pid_t pid ;
  char buffer[10], result[10];
  int pp[2], qq[2];
  int a , b, c;

  pipe(pp); // 親へデータを送るためのパイプ
  pipe(qq); // 計算結果を子に送るためのパイプ

  pid = fork();  // 親子のプロセスの生成

  // 子プロセスの動き
  if ( pid == 0 ) {
    close(pp[0]);
    close(qq[1]);

    // dup2(pp[1], 1);

    // 1つ目の変数の代入とパイプへの書き込み

    printf("a = ");
    fgets(buffer, 10, stdin);
    write(pp[1], buffer, strlen(buffer)+1);

    // 2つ目の変数の代入とパイプへの書き込み
    printf("b = ");
    fgets(buffer, 10, stdin);
    write(pp[1], buffer, strlen(buffer)+1);

    // 計算結果を読み込む
    read(qq[0], result, 10);

    printf("a + b = %s\n", result);
    close(qq[0]);
    close(pp[1]);
  }
  // 親プロセスの動き
  else {
    close(qq[0]);
    close(pp[1]);

    // 1つ目の変数の読み込み
    read(pp[0], buffer, 256);
    a = atoi(buffer);

    // 2つ目の変数の読み込み
    read(pp[0], buffer, 256);
    b = atoi(buffer);

    c = a + b ; // 計算
    sprintf(buffer, "%d", c);

    dup2(qq[1], 1);
    printf("%s",buffer );

    // 計算結果をパイプへ書き込む
    // write(qq[1], buffer, strlen(buffer)+1);

    close(pp[0]);
    close(qq[1]);
  }
  return 0;
}
