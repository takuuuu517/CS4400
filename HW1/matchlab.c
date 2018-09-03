#include <stdio.h>
#include<string.h>

void mode_select(char *arguments[], int* t_flag, int* mode, int argc, int* str_index );
void a_mode(char *string, int t_flag);
void b_mode(char *string, int t_flag);
void c_mode(char *string, int t_flag);


int main(int argc, char *argv[])  // argc 何個arg があるか　argv 内容string
{
  int t_flag = 0; // 0 = flase;
  int mode = 5; // 0 = a, 1 = b, 2 = c, 3 = exeption, default = 5;
  int str_index = 1 ; // need to change later;

  // printf("%s\n", argv[0]);
  // printf("%s\n", argv[1]);
  // printf("%s\n", argv[2]);

  // printf("argc: %d\n", argc);
  mode_select(argv, &t_flag, &mode, argc , &str_index);
  // printf("mode: %c\n",mode+97);
  // printf("t flag: %d\n", t_flag);
  //
  // printf("starting index: %d\n", str_index);
  // printf("first string: %s\n", argv[str_index]);
  // printf("last string: %s\n\n\n\n", argv[argc-1]);

  while(str_index < argc)
  {
    switch (mode) {
      case 0:
      a_mode(argv[str_index], t_flag);
      str_index++;
      break;

      case 1:
      b_mode(argv[str_index], t_flag);
      str_index++;
      break;

      case 2:
      c_mode(argv[str_index], t_flag);
      str_index++;
      break;

      default:
      printf("%s\n", "Mode is not valid");
    }
  }

}

void mode_select(char *arguments[], int* t_flag, int* mode, int argc, int* str_index)
{
  int i =1;

  // example of labmda          string x = 1 == 1 ? "yes it does" : "no it doesn't";
  int max = argc > 2 ? 3 : 2;

  // printf("max: %d\n",max );


  for (i = 1; i < max; i++)
  {

    // printf("%d\n", strcmp(arguments[i], "-t"));
    if(strcmp(arguments[i], "-t") == 0 )
    {
      *str_index += 1;
      *t_flag = 1;
    }
    else if(strcmp(arguments[i], "-a") == 0 )
    {
      *str_index += 1;
      *mode = 0;
    }
    else if(strcmp(arguments[i], "-b") == 0 )
    {
      *str_index += 1;
      *mode = 1;
    }
    else if(strcmp(arguments[i], "-c") == 0 )
    {
      *str_index += 1;
      *mode = 2;
    }
  }
  if(*mode == 5)
  {
    *mode = 0 ;
  }

}

void a_mode(char *string, int t_flag)
{
  int m_valid = 1; // default is true;
  int __valid = 1; // default is true;
  int x_valid = 1; // default is true;
  int equal_valid = 1; // default is true;
  int number_valid =1; // default is true;

  int string_valid =1;


  int m_total = 0 ;
  // int __total = 0;
  int x_total = 0;
  // int equal_total = 0;
  int num_total = 0;

  // printf("%s\n",string );
  // printf("%u\n", (unsigned)strlen(string));

  int i;
  for(i=0; i<(unsigned)strlen(string);i++)
  {
    // printf("%c\n", string[i]);

    if(string_valid == 0)
      break; // terminate

    if(m_valid == 1) // m check
    {
      if(string[i] == '_')
      {
        m_valid = 2;
        __valid = 2;
      }

      else if(string[i] != 'm')
        string_valid = 0;

      else
        m_total++;
    }

    else if(__valid == 2 && m_valid == 2) // _ check
    {
      // if(string[i] == 'x')
      //   __valid = 2;
      // else
      if(string[i] != 'x')
        string_valid =0;
    }

    else if(x_valid == 1 && __valid == 2)
    {
      if(string[i] == '=')
      {
        x_valid = 2;
      }
      else if(string[i] != 'x')
        string_valid =0;

      else
        x_total++;
    }

    else if(equal_valid == 1 && x_valid == 2) //一回しかこない
    {
      if(string[i] == '=')
        equal_valid = 2;
      else
        string_valid =0;

      // if(string[i] > 47 && string[i] < 58)
      //   equal_valid = 2;
      // else if(string[i] != '=')
      //   string_valid =0;
    }

    if(number_valid == 1 && equal_valid == 2)
    {
      if(!(string[i] > 47 && string[i] < 58))
        string_valid =0;
      else if(string[i] > 47 && string[i] < 57)
      {
        string[i] += 1;
        num_total++;
      }

      else if(string[i] == 59)
        num_total++;
    }
    // printf("%c\n",string[5]);
  } // end of for loop


  if(t_flag == 0)
  {
    if(string_valid == 1 && m_total>0 && x_total > 0 && num_total < 0 && num_total < 4)
      printf("yes\n");
    else
      printf("no\n" );
  }
  else
  {
    if(string_valid == 1)
      printf("%s\n",string);
  }

}

void b_mode(char *string, int t_flag)
{
  int i;

  for(i=0; i<(unsigned)strlen(string);i++)
  {




  }
}

void c_mode(char *string, int t_flag)
{

}
