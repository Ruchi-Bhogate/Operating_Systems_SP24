#include <bareio.h>
#include <barelib.h>

#define MAX_LENGTH 1024

/*
 * 'builtin_echo' reads in a line of text from the UART
 * and prints it.  It will continue to do this until the
 * line read from the UART is empty (indicated with a \n
 * followed immediately by another \n).
 */
byte builtin_echo(char *arg)
{
  // printf("inisde echo function");
  if (arg[4] == ' ')
  {
    printf("%s\n", &(arg[5]));
    return 0;
  }
  else
  {
    int result = 0;
    while (1)
    {
      int count = 0;
      char temp;
      char buffer_input[MAX_LENGTH];

      for (int i = 0; i < MAX_LENGTH; i++)
      {
        buffer_input[i] = '\0';
      }

      int i = 0;

      while (1)
      {
        temp = uart_getc();
        if (temp == '\n')
        {
          break;
        }
        else
        {
          buffer_input[i++] = temp;
          count++;
        }
      }
      if (count){
        printf("%s\n", buffer_input);
        result += count;
      }
      else{
        break;
      }
    }
    return result;
  }
}
