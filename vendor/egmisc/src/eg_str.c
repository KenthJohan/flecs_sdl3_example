#include "egmisc/eg_str.h"

void eg_str_replace_ab(char *str, char a, char b)
{
	char *p = str;
	while (p[0]) {
		if (p[0] == a) {
			p[0] = b;
		}
		p++;
	}
}