#include <bareio.h>
#include <barelib.h>

/*
 * 'builtin_hello' prints "Hello, <text>!\n" where <text> is the contents
 * following "builtin_hello " in the argument and returns 0.
 * If no text exists, print and error and return 1 instead.
 */
byte builtin_hello(char *arg)
{
  // printf("inside hello function");
  if (arg[5] == ' ')
  {
    printf("Hello, %s!\n", &(arg[6]));
    return 0;
  }
  else
  {
    printf("Error - bad argument\n");
    return 1;
  }
}
