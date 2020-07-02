/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 * stupid library routines.. The optimized versions should generally be found
 * as inline code in <asm-xx/string.h>
 *
 * These are buggy as well..
 *
 * * Fri Jun 25 1999, Ingo Oeser <ioe@informatik.tu-chemnitz.de>
 * -  Added strsep() which will replace strtok() soon (because strsep() is
 *    reentrant and should be faster). Use only strsep() in new code, please.
 */

/* We don't want strings.h stuff being user by user stuff by accident */

#include <linux/types.h>

extern char *___strtok;
extern char *strpbrk(const char *, const char *);
extern char *strtok(char *, const char *);
extern char *strsep(char **, const char *);
extern size_t strspn(const char *, const char *);

extern void puts(const char *str);
extern int putchar(int c);

/*
 * Include machine specific inline routines
 */
//#include <asm/string.h>

#ifndef __HAVE_ARCH_STRCPY
extern char *strcpy(char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRNCPY
extern char *strncpy(char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRLCPY
extern size_t strlcpy(char *dest, const char *src, size_t size);
#endif

#ifndef __HAVE_ARCH_STRCAT
extern char *strcat(char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRNCAT
extern char *strncat(char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRCMP
extern int strcmp(const char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRNCMP
extern int strncmp(const char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRNICMP
extern int strnicmp(const char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRCHR
extern char *strchr(const char *, int);
#endif
#ifndef __HAVE_ARCH_STRRCHR
extern char *strrchr(const char *, int);
#endif
#ifndef __HAVE_ARCH_STRSTR
extern char *strstr(const char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRLEN
extern size_t strlen(const char *);
#endif
#ifndef __HAVE_ARCH_STRNLEN
extern size_t strnlen(const char *, size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET
extern void *memset(void *, int, size_t);
#endif
#ifndef __HAVE_ARCH_MEMCPY
extern void *memcpy(void *, const void *, size_t);
#endif
#ifndef __HAVE_ARCH_MEMMOVE
extern void *memmove(void *, const void *, size_t);
#endif
#ifndef __HAVE_ARCH_MEMSCAN
extern void *memscan(void *, int, size_t);
#endif
#ifndef __HAVE_ARCH_MEMCMP
extern int memcmp(const void *, const void *, size_t);
#endif
#ifndef __HAVE_ARCH_MEMCHR
extern void *memchr(const void *, int, size_t);
#endif
