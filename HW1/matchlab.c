#include <stdio.h>
#include<string.h>
#include <stdlib.h>

void mode_select(char *arguments[], int* t_flag, int* mode, int argc, int* str_index );
void a_mode(char *string, int t_flag);
void b_mode(char *string, int t_flag);
void c_mode(char *string, int t_flag);

int b_is_valid_string(char *str,char* newstr);

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


int b_is_valid_string(char *str, char* newstr) // return 1 if valid, otherwise returns 0
{
  int is_valid = 1;

  int h_total = 0;
  int colon_toal= 0;
  int num_total = 0;
  int z_total =0 ; // has to be 4 or 5
  int equal_total =0;

  int num_of_upper = 0;// %2 == 1
  int odd_num = -1;

  int h_done = 0;
  int colon_done = 0;
  int num_done = 0;
  int z_done = 0;
  int equal_done = 0;

  int insert_num = 0;

  int i;
  for(i=0; i < strlen(str); i++)
  {

    // if(is_valid == 0)
    //   return is_valid;

    // printf("%c\n",str[i] );

    if(h_done == 0) // h and : check
    {
      if(str[i] == 'h')
        h_total++;
      else if(h_total > -1 && h_total < 2 && str[i] == ':')
      {
        h_done++;
        colon_done++;
        colon_toal++;
      }
      else
        return 0;
    }

    else if(num_done == 0) // num check_
    {
      if(str[i] > 47 && str[i] < 58)
      {
        num_total++;
        if(num_total == 2)
          odd_num = (int) (str[i] - '0');
      }

      else if(num_total > 0 && num_total < 4 && str[i] == 'z')
      {
        num_done++;
        z_total++;
      }
      else
        return 0;
    }

    else if(z_done == 0 ) // z and = check
    {
      if(str[i] == 'z')
        z_total++;
      else if (z_total > 3 && z_total < 6 && str[i] == '=')
      {
        z_done++;
        equal_done++;
        equal_total++;
      }
      else
        return 0;
    }

    if(equal_done == 1||equal_done == 2)
    {
      if(equal_done == 1)
        equal_done++;

      // check after = , has to be number or upper case letter
      else if(str[i] > 64 && str[i] < 91) // check upper
      {

        num_of_upper++;
      }
      else if(!((str[i] > 47 && str[i] < 58) || str[i] == odd_num)) // check number
      {
        // printf("asdf%d\n",odd_num );
        return 0;
      }
    }


    //create new string
    insert_num = i %8;
    newstr[i*2] = str[i];
    newstr[i*2+1] = insert_num+'0';


  } // end of for loop

  //check upper case number
  if(num_of_upper %2 != 1)
  {
    return 0;

  }



  return is_valid;

}

void b_mode(char *string, int t_flag)
{
  char * newstr = (char*)malloc(strlen(string)*3);
  if(t_flag == 0)
  {
    if(b_is_valid_string(string, newstr) ==1)
      printf("yes\n");
    else
      printf("no\n" );
  }
  else
  {

    if(b_is_valid_string(string, newstr) == 1)
      printf("%s\n",newstr);
  }

  free(newstr);

}

void c_mode(char *string, int t_flag)
{

}
