/*
 * Use local definitions of C library macros and functions
 * NOTE: The function implementations may not be as efficient
 * as an inline or assembly code implementation provided by a
 * native C library.
 */

#include <linux/types.h>
#include <common.h>

ulong simple_strtoul(const char *cp, char **endp, unsigned int base);

/**
 * strict_strtoul - convert a string to an unsigned long strictly
 * @param cp	The string to be converted
 * @param base	The number base to use
 * @param res	The converted result value
 * @return 0 if conversion is successful and *res is set to the converted
 * value, otherwise it returns -EINVAL and *res is set to 0.
 *
 * strict_strtoul converts a string to an unsigned long only if the
 * string is really an unsigned long string, any string containing
 * any invalid char at the tail will be rejected and -EINVAL is returned,
 * only a newline char at the tail is acceptible because people generally
 * change a module parameter in the following way:
 *
 *      echo 1024 > /sys/module/e1000/parameters/copybreak
 *
 * echo will append a newline to the tail.
 *
 * simple_strtoul just ignores the successive invalid characters and
 * return the converted value of prefix part of the string.
 *
 * Copied this function from Linux 2.6.38 commit ID:
 * 521cb40b0c44418a4fd36dc633f575813d59a43d
 *
 */
int strict_strtoul(const char *cp, unsigned int base, unsigned long *res);
unsigned long long simple_strtoull(const char *cp, char **endp,
					unsigned int base);
long simple_strtol(const char *cp, char **endp, unsigned int base);

/**
 * trailing_strtol() - extract a trailing integer from a string
 *
 * Given a string this finds a trailing number on the string and returns it.
 * For example, "abc123" would return 123.
 *
 * @str:	String to exxamine
 * @return training number if found, else -1
 */
long trailing_strtol(const char *str);

/**
 * trailing_strtoln() - extract a trailing integer from a fixed-length string
 *
 * Given a fixed-length string this finds a trailing number on the string
 * and returns it. For example, "abc123" would return 123. Only the
 * characters between @str and @end - 1 are examined. If @end is NULL, it is
 * set to str + strlen(str).
 *
 * @str:	String to exxamine
 * @end:	Pointer to end of string to examine, or NULL to use the
 *		whole string
 * @return training number if found, else -1
 */
long trailing_strtoln(const char *str, const char *end);

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
int snprintf(char *buf, size_t size, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
int vsscanf(const char *buf, const char *fmt, va_list args);
int sscanf(const char *buf, const char *fmt, ...);
