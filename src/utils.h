#ifndef UTILS_H_
#define UTILS_H_

void* xmalloc(size_t size);
char* xstrtrim(char *str);
char* xstrcat(const char *str1, const char *str2);
void xstrdel(char *str, size_t pos);
void xstrins(char *str, size_t pos, char c, size_t size);

#endif /*UTILS_H_*/
