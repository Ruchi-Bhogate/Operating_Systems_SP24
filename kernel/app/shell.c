#include <bareio.h>
#include <barelib.h>
#include <shell.h>

#define PROMPT "bareOS$ " /*  Prompt printed by the shell to the user  */
#define MAX_LENGTH 1024   /* Lines are AT MOST 1024 characters long (after expansion) */
int result = 0;

/*
 * 'shell' loops forever, prompting the user for input, then calling a function based
 * on the text read in from the user.
 */

int command_check(char *buffer_input, char *command)
{
  for (int i = 0; i < MAX_LENGTH && command[i] != '\0'; i++)
  {
    if (command[i] == buffer_input[i])
    {
      continue;
    }
    else
    {
      return 0;
    }
  }
  return 1;
}

byte shell(char *arg)
{
  while (1)
  {
    printf("%s", PROMPT);

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
      else if (temp == '$')
      {
        temp = uart_getc();
        if (temp == '?')
        {
          buffer_input[i++] = '0' + result; // As Return values are unsigned char ranging from 0-255
        }
        else
        {
          buffer_input[i++] = '$';
          buffer_input[i++] = temp;
        }
      }
      else
      {
        buffer_input[i++] = temp;
      }
    }
    //printf("%s\n",buffer_input);
    if (command_check(buffer_input, "hello"))
    {
      // printf("inside hello");
      result = builtin_hello(buffer_input);
    }
    else if (command_check(buffer_input, "echo"))
    {
      // printf("inside echo");
      result = builtin_echo(buffer_input);
    }
    else if (buffer_input[0] == '\0')
    {
      continue;
    }
    else
    {
      printf("Unknown command\n");
    }
    // printf("Result is %d", result);
  }
  return 0;
}
