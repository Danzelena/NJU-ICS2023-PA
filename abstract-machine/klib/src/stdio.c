#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define LEN 10

char *itoa();
int printf(const char *fmt, ...)
{
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list ap;
  int d;
  char c;
  char *s;

  va_start(ap, fmt);
  while (*fmt)
  {
    switch (*fmt)
    {
    case '%':
      switch (*(fmt + 1))
      {
      case 's':
        s = va_arg(ap, char *);
        out = strcat(out, s);
        out += strlen(s);
        break;
      case 'd':
        char snum[LEN];

        d = va_arg(ap, int);
        itoa(d, snum);
        out = strcat(out, snum);
        out += strlen(snum);
        break;

      case 'c':
        // need a cast here becase va_arg only takes fully promoted types
        c = (char)va_arg(ap, int);
        out = strcat(out, &c);
        out += 1;
        break;
      }
      fmt++;
      break;

    default:
      *out = *fmt;
      break;
    }
    fmt++;
  }
  va_end(ap);
  return 0;
  // panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

// dec for common use
char *itoa(int val, char *str)
{
  char *ret = str;
  if (val < 0)
  {
    *str = '-';
    str++;
    val = -val;
  }
  int num = val;
  int cnt = -1;
  while (num > 0)
  {
    num /= 10;
    cnt++;
  }
  int len = cnt;
  while (cnt >= 0)
  {
    *(str + cnt) = '0' + val % 10;
    cnt--;
    val /= 10;
  }
  //   *str = '0' + val%10;
  *(str + len + 1) = '\0';
  return ret;
}
#endif
