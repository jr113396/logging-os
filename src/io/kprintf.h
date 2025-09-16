#ifndef kprintfh
#define kprintfh

// printk.c
char * itoa   ( unsigned long );
int  strlen   ( char * );
void kprintf  ( char *, ... );
char *outstr(char *, int, char );
char *_itoa(unsigned long, int, unsigned int);


#endif