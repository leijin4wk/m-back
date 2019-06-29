#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


int
m_split_string(char *input, const char *delim, char **out, size_t ele)
{
	int		count;
	char		**ap;

	if (ele == 0)
		return (0);

	count = 0;
	for (ap = out; ap < &out[ele - 1] &&
	    (*ap = strsep(&input, delim)) != NULL;) {
		if (**ap != '\0') {
			ap++;
			count++;
		}
	}

	*ap = NULL;
	return (count);
}

char *
m_time_to_date(time_t now)
{
	struct tm		*tm;
	static time_t		last = 0;
	static char		tbuf[32];

	if (now != last) {
		last = now;
		tm = gmtime(&now);
		if (!strftime(tbuf, sizeof(tbuf), "%a, %d %b %Y %T GMT", tm)) {
			return (NULL);
		}
	}

	return (tbuf);
}

u_int64_t
m_time_ms(void)
{
	struct timespec		ts;

	(void)clock_gettime(CLOCK_MONOTONIC, &ts);

	return ((u_int64_t)(ts.tv_sec * 1000 + (ts.tv_nsec / 1000000)));
}


void *
m_mem_find(void *src, size_t slen, const void *needle, size_t len)
{
	size_t		pos;

	for (pos = 0; pos < slen; pos++) {
		if ( *((u_int8_t *)src + pos) != *(const u_int8_t *)needle)
			continue;

		if ((slen - pos) < len)
			return (NULL);

		if (!memcmp((u_int8_t *)src + pos, needle, len))
			return ((u_int8_t *)src + pos);
	}

	return (NULL);
}

char *
m_text_trim(char *string, size_t len)
{
	char		*end;

	if (len == 0)
		return (string);

	end = (string + len) - 1;
	while (isspace(*(unsigned char *)string) && string < end)
		string++;

	while (isspace(*(unsigned char *)end) && end > string)
		*(end)-- = '\0';

	return (string);
}

char *
m_read_line(FILE *fp, char *in, size_t len)
{
	char	*p, *t;

	if (fgets(in, len, fp) == NULL)
		return (NULL);

	p = in;
	in[strcspn(in, "\n")] = '\0';

	while (isspace(*(unsigned char *)p))
		p++;

	if (p[0] == '#' || p[0] == '\0') {
		p[0] = '\0';
		return (p);
	}

	for (t = p; *t != '\0'; t++) {
		if (*t == '\t')
			*t = ' ';
	}

	return (p);
}
