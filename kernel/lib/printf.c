#include <bareio.h>
#include <barelib.h>
#include <stdarg.h>
/*
 *  In this file, we will write a 'printf' function used for the rest of
 *  the semester.  In is recommended  you  try to make your  function as
 *  flexible as possible to allow you to implement  new features  in the
 *  future.
 *  Use "va_arg(ap, int)" to get the next argument in a variable length
 *  argument list.
 */

void convert_to_hex_or_dec(unsigned long int number, int base)
{
  unsigned long int temp = number % base;

  if (number == 0)
  {
    return;
  }
  else
  {
    convert_to_hex_or_dec(number / base, base);
  }

  if (temp < 10)
  {
    uart_putc((temp + 48));
  }
  else
  {
    uart_putc((temp + 87));
  }
}

void printf(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);

  while (*format != '\0')
  {
    if (*format == '%')
    {
      ++format;
      if (*format == 'd')
      {
        unsigned long int temp = va_arg(ap, unsigned long int);
        if (temp == 0)
        {
          uart_putc('0');
        }
        else
        {
          convert_to_hex_or_dec(temp, 10);
        }
      }
      else if (*format == 'x')
      {
        uart_putc('0');
        uart_putc('x');
        unsigned long int temp = va_arg(ap, unsigned long int);
        if (temp == 0)
        {
          uart_putc('0');
        }
        else
        {
          convert_to_hex_or_dec(temp, 16);
        }
      }
    }
    else
    {
      uart_putc(*format);
    }
    ++format;
  }

  va_end(ap);
}
