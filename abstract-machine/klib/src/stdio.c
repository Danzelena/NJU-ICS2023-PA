#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// #if !defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__)

static char HEX_CHARACTERS[] = "0123456789abcdef";
#define BIT_WIDE_HEX 8
int vsprintf(char *out, const char *fmt, va_list args);

int printf(const char *fmt, ...)
{
	char buf[2048];
	va_list arg;
	va_start(arg, fmt);

	int flag = vsprintf(buf, fmt, arg);

	putstr(buf);

	va_end(arg);
	return flag;
}
int vsprintf(char *out, const char *fmt, va_list args)
{
	return vsnprintf(out, -1, fmt, args);
}

int sprintf(char *out, const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	int flag = vsprintf(out, fmt, arg);
	va_end(arg);
	return flag;
}
int snprintf(char *out, size_t n, const char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	int flag = vsnprintf(out, n, fmt, arg);
	va_end(arg);
	return flag;
}
#define append(x) {out[j++]=x; if (j >= n) {break;}}

int vsnprintf(char *out, size_t n, const char *fmt, va_list args)
{
	char buffer[128];
	char *txt, cha;
	int num, len;
	unsigned int unum;
	uint32_t pointer;

	int state = 0, i, j;
	for (i = 0, j = 0; fmt[i] != '\0'; i++)
	{
		switch (state)
		{
		case 0:
			if (fmt[i] != '%')
			{
				out[j] = fmt[i];
				j++;
				if (j >= n)
				{
					break;
				}
			}
			else
			{
				state = 1;
				break;
			}
		case 1:
			switch (fmt[i])
			{
			case 's':
				txt = va_arg(args, char *);
				for (int k = 0; txt[k] != '\0'; k++)
				{
					out[j] = txt[k];
					j++;
					if (j >= n)
					{
						break;
					}
				}
				break;

			case 'd':
				num = va_arg(args, int);
				if (num == 0)
				{
					out[j] = '0';
					j++;
				}
				else if (num < 0)
				{
					out[i] = '-';
					j++;
					num = -num;
				}
				for (len = 0; num; num /= 10, len++)
				{
					buffer[len] = HEX_CHARACTERS[num % 10];
				}
				for (int k = len - 1; k >= 0; k--)
				{
					out[i] = buffer[k];
					j++;
				}
				break;

			case 'c':
				cha = (char)va_arg(args, int);
				append(cha);
				break;

			case 'p':
				pointer = va_arg(args, uint32_t);
				for (len = 0; pointer; pointer /= 16, ++len)
					buffer[len] = HEX_CHARACTERS[pointer % 16]; // 逆序的
				for (int k = 0; k < BIT_WIDE_HEX - len; ++k)
					append('0');

				for (int k = len - 1; k >= 0; --k)
					append(buffer[k]);
				break;

			case 'x':
				unum = va_arg(args, unsigned int);
				if (unum == 0)
				{
					append('0');
					break;
				}
				for (len = 0; unum; unum >>= 4, ++len)
					buffer[len] = HEX_CHARACTERS[unum & 0xF]; // 逆序的

				for (int k = len - 1; k >= 0; --k)
					append(buffer[k]);
				break;

			default:
				assert(0);
			}
			state = 0;
			break;

		default:
			assert(0);
		}
	}
	out[j] = '\0';
	return j;
}

#endif

// //TODO:target1
// int int2str(char *buf, int num){
// 	if (num == -2147483648){
// 		strcat(buf, "-2147483648");
// 		return 11;
// 	}
// 	int ret = 0;
// 	if(num < 0){
// 		*buf++ = '-';
// 		ret++;
// 		num = -num;
// 	}
// 	char *p = buf;
// 	char *q = buf;
// 	do {ret++;
// 		*q++ = (char)(num%10 + '0');
// 		num /= 10;
// 	}while(num);
// 	*q = '\0';
// 	q--;
// 	while(q > p){
// 		char temp = *p;
// 		*p++ = *q;
// 		*q-- = temp;
// 	}
// 	return ret;
// }

// int x2str(char *buf, unsigned num){
// 	char xtab[] = "0123456789abcdef";
// 	int ret = 0;
// 	char *p = buf;
// 	char *q = buf;
// 	while(num > 15)
// 	{
// 		*q++ = xtab[num%16];
// 		ret++;
// 		num /= 16;
// 	}
// 	*q++ = xtab[num%16];
// 	ret++;
// 	*q = '\0';
// 	q--;
// 	while(q > p){
// 		char temp = *p;
// 		*p++ = *q;
// 		*q-- = temp;
// 	}
// 	return ret;
// }

// int str2str(char *buf, char *src){

// 	strcat(buf, src);
// 	return strlen(src);
// }

// int float2str(char *buf, float flt, int len){
// 	int ret = 0;
// 	if (flt < 0){
// 		ret++;
// 		*buf++ = '-';
// 	}
// 	int flt_int = (int)flt;
// 	float flt_frac = flt - flt_int;
// 	int tmp_ret = int2str(buf, flt_int);
// 	ret += tmp_ret;
// 	buf += tmp_ret;
// 	ret ++;
// 	*buf++ = '.';
// 	while(len--){
// 		flt_frac *= 10;
// 	}
// 	int flt_frac_int = (int)flt_frac;
// 	ret += int2str(buf, flt_frac_int);
// 	return ret;

// }

// int vsprintf(char *out, const char *fmt, va_list ap) {
// 	/* panic("Not implemented"); */

// 	*out = '\0';
// 	int ret = 0;
// 	while(*fmt != '\0'){
// 		if(*fmt != '%'){
// 			ret++;
// 			*out++ = *fmt++;
// 			*out = '\0';
// 		}
// 		else{
// 			fmt++;
// 			switch(*fmt){
// 				case 's':{	 char *valstr = va_arg(ap, char*);
// 							 int sub_ret = str2str(out, valstr);
// 							 ret += sub_ret;
// 							 out += sub_ret;
// 							 fmt++;
// 							 assert(*out=='\0');
// 							 break;
// 						 }
// 				case 'd':{	 int valint = va_arg(ap, int);
// 							 int sub_ret = int2str(out, valint);
// 							 ret += sub_ret;
// 							 out += sub_ret;
// 							 fmt++;
// 							 break;
// 						 }
// 				/* case 'f':{ */
// 				/* 			 float valflt  = va_arg(ap, double); */
// 				/* 			 int sub_ret = float2str(out, valflt, 6); */
// 				/* 			 ret += sub_ret; */
// 				/* 			 out += sub_ret; */
// 				/* 			 fmt++; */
// 				/* 			 /1* *out = '\0'; *1/ */
// 				/* 			 break; */
// 				/* 		 } */
// 				case 'x':{
// 							 int val  = va_arg(ap , int);
// 							 int sub_ret = x2str(out, val);
// 							 ret += sub_ret;
// 							 out += sub_ret;
// 							 fmt++;
// 							 /* *out = '\0'; */
// 							 break;
// 						 }
// 				case 'p':{
// 							 ret += 2;
// 							 *out++ = '0';
// 							 *out++ = 'x';
// 							 int val  = va_arg(ap , int);
// 							 char addr[10]="\0";
// 							 int sub_ret = x2str(addr, val);
// 							 for(int k = 8-sub_ret; k > 0; k--){
// 								*out++ = '0';
// 							 }
// 							 *out = '\0';
// 							 strcat(out, addr);
// 							 ret += 8;
// 							 out += sub_ret;
// 							 fmt++;
// 							 /* *out = '\0'; */
// 							 break;
// 						 }
// 				case 'c':{
// 							char val = va_arg(ap, int);
// 							ret++;
// 							*out++ = val;
// 							fmt++;
// 							break;
// 						 }
// 				default: {	 ret++;
// 							 *out++ = *fmt++;
// 							 *out = '\0';
// 						 }
// 			}
// 		}

// 		assert(*out == '\0');
// 	}
// 	assert(*out == '\0');
// 	return ret;
// }

// int sprintf(char *out, const char *fmt, ...) {
// 	/* panic("Not implemented"); */
// 	va_list ap;
// 	va_start(ap, fmt);
// 	int ret = vsprintf(out, fmt, ap);
// 	va_end(ap);
// 	return ret;
// }

// int printf(const char *fmt, ...) {
// 	/* panic("Not implemented"); */
// 	char out[4095];
// 	va_list ap;
// 	va_start(ap, fmt);
// 	int ret = vsprintf(out, fmt, ap);
// 	va_end(ap);
// 	/* putch('A'); */
// 	/* for(int k = 0; k < 20; k++){ */
// 	/* putch(out[k]); */
// 	/* putch('\n'); */
// 	/* } */
// 	putstr(out);
// 	return ret;
// }

// int snprintf(char *out, size_t n, const char *fmt, ...) {
// 	panic("Not implemented");
// }

// int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
// 	panic("Not implemented");
// }

/*TODO:fix this biggggggg BUG!!!*/
// int vsprintf(char *out, const char *fmt, va_list ap);
// int vsnprintf(char *out, size_t n, const char *fmt, va_list ap);

// int printf(const char *fmt, ...) {
//   char buffer[2048];
//   va_list arg;
//   va_start (arg, fmt);

//   int done = vsprintf(buffer, fmt, arg);

//   putstr(buffer);

//   va_end(arg);
//   return done;
// }

// static char HEX_CHARACTERS[] = "0123456789ABCDEF";
// #define BIT_WIDE_HEX 8

// int vsprintf(char *out, const char *fmt, va_list ap) {
//   return vsnprintf(out, -1, fmt, ap);
// }

// int sprintf(char *out, const char *fmt, ...) {
//   va_list valist;
//   va_start(valist, fmt);

//   int res = vsprintf(out ,fmt, valist);
//   va_end(valist);
//   return res;
// }

// int snprintf(char *out, size_t n, const char *fmt, ...) {
//   va_list arg;
//   va_start (arg, fmt);

//   int done = vsnprintf(out, n, fmt, arg);

//   va_end(arg);
//   return done;
// }

// #define append(x) {out[j++]=x; if (j >= n) {break;}}

// int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
//   char buffer[128];
//   // char cha;
//   char *txt, cha;
//   // int len;
//   int num, len;
//   unsigned int unum;
//   uint32_t pointer;

//   int state = 0, i, j;//模仿一个状态机
//   for (i = 0, j = 0; fmt[i] != '\0'; ++i){
//     switch (state)
//     {
//     case 0:
//       if (fmt[i] != '%'){
//         append(fmt[i]);
//       } else
//         state = 1;
//       break;

//     case 1:
//       switch (fmt[i])
//       {
//       case 's':
//         txt = va_arg(ap, char*);
//         for (int k = 0; txt[k] !='\0'; ++k)
//           append(txt[k]);
//         break;

//       case 'd':
//         num = va_arg(ap, int);
//         if(num == 0){
//           append('0');
//           break;
//         }
//         if (num < 0){
//           append('-');
//           num = 0 - num;
//         }
//         for (len = 0; num ; num /= 10, ++len)
//           //buffer[len] = num % 10 + '0';//逆序的
//           buffer[len] = HEX_CHARACTERS[num % 10];//逆序的
//         for (int k = len - 1; k >= 0; --k)
//           append(buffer[k]);
//         break;

//       case 'c':
//         cha = (char)va_arg(ap, int);
//         append(cha);
//         break;

//       case 'p':
//         pointer = va_arg(ap, uint32_t);
//         for (len = 0; pointer ; pointer /= 16, ++len)
//           buffer[len] = HEX_CHARACTERS[pointer % 16];//逆序的
//         for (int k = 0; k < BIT_WIDE_HEX - len; ++k)
//           append('0');

//         for (int k = len - 1; k >= 0; --k)
//           append(buffer[k]);
//         break;

//       case 'x':
//         unum = va_arg(ap, unsigned int);
//         if(unum == 0){
//           append('0');
//           break;
//         }
//         for (len = 0; unum ; unum >>= 4, ++len)
//           buffer[len] = HEX_CHARACTERS[unum & 0xF];//逆序的

//         for (int k = len - 1; k >= 0; --k)
//           append(buffer[k]);
//         break;

//       default:
//         assert(0);
//       }
//       state = 0;
//       break;

//     }
//   }

//   out[j] = '\0';
//   return j;
// }

// #endif

// #define LEN 10
// #define BUFSIZE
// char *itoa();
// int printf(const char *fmt, ...)
// {
//   // putch('0');
//   char buf[BUFSIZE];
//   memset(buf, '\0',BUFSIZE);
//   va_list args;

//   va_start(args, fmt);
//   // printf("Target1\n");
//   if(sprintf(buf, fmt, args) == 0){
//     // printf("Debug:\n", buf);
//     putstr(buf);

//   }else{
//     // printf("Fail to printf beacuse sprintf fail!\n");
//     return -1;
//   }
//   va_end(args);
//   free(buf);
//   return 0;
//   panic("Not implemented");
// }

// int vsprintf(char *out, const char *fmt, va_list ap)
// {
//   return vsnprintf(out, -1, fmt, ap);
//   // panic("Not implemented");
// }

// int sprintf(char *out, const char *fmt, ...)
// {
//   // TODO:recode this code becase of terrible abstract!
//   va_list ap;
//   int d;
//   char c;
//   char *s;
//   char *ret_poi = out;
//   va_start(ap, fmt);
//   while (*fmt)
//   {
//     switch (*fmt)
//     {
//     case '%':
//         *out = '\0';
//       switch (*(fmt + 1))
//       {
//       case 's':
//         s = va_arg(ap, char *);
//         out = strcat(out, s);
//         out += strlen(s);
//         break;
//       case 'd':
//         char snum[LEN];

//         d = va_arg(ap, int);
//         itoa(d, snum);
//         out = strcat(out, snum);
//         out += strlen(snum);
//         break;

//       case 'c':
//         // need a cast here becase va_arg only takes fully promoted types
//         c = (char)va_arg(ap, int);
//         out = strcat(out, &c);
//         out += 1;
//         break;
//       }
//       fmt++;
//       break;

//     default:
//       *out = *fmt;
//       out += 1;
//       break;
//     }
//     fmt++;
//   }
//   *out= '\0';
//   out = ret_poi;
//   va_end(ap);
//   return 0;
//   // panic("Not implemented");
// }

// int snprintf(char *out, size_t n, const char *fmt, ...)
// {
//   panic("Not implemented");
// }

// int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
// {
//   //TODO: quickly implementit!!!
//   panic("Not implemented");
// }

// // dec for common use
// char *itoa(int val, char *str)
// {
//   char *ret = str;
//   if (val < 0)
//   {
//     *str = '-';
//     str++;
//     val = -val;
//   }
//   int num = val;
//   int cnt = -1;
//   while (num > 0)
//   {
//     num /= 10;
//     cnt++;
//   }
//   int len = cnt;
//   while (cnt >= 0)
//   {
//     *(str + cnt) = '0' + val % 10;
//     cnt--;
//     val /= 10;
//   }
//   //   *str = '0' + val%10;
//   *(str + len + 1) = '\0';
//   return ret;
// }
// #endif
