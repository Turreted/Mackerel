#ifndef UTIL_H
#define UTIL_H

#define INF 2147483647

unsigned int prng();
void prng_seed(unsigned int seed);
char *tokenize(char *str, const char *delim, char **key);
char *strip(char *str);
int starts_with(const char *str, const char *prefix);
double now();

int min(int, int);
int max(int, int);

#endif
