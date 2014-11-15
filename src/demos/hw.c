/*
OMGs: You cannot enter ` nor ~ on the Nascom keyboard?
A: @ is 0x40 - shift-@  (but I don't have a @ key!)
   ` is 0x60 - press Ctrl-space
   ~ is 0x7e - press Ctrl-> (greater-than)
   Instead of # the symbol £ is in its place, but # is at 0x9E
*/
#include <stdio.h>

int main()
{
    int ch, i;
    float f;

    printf("Hello World, again again!\n"
	   "Type here: ");
    for (;;) {
	ch = getchar();

	//printf("<%02x>", ch);
        putchar(ch);

	if (ch == 'q')
	    break;
    }

    printf("Testing floats\n");

    for (i = 0, f = 1.0; f < 2.0; f += 0.1, ++i)
        ;

    printf("\nGoing from 1.0 to 2.0 in 0.1 steps took %d steps\n"
           "Ok?", i);
    getchar();


    putchar('\n');
    for (ch = 0x9E; ch < 256; ++ch)
	putchar(ch);

    getchar();

#if 0
    /* fast output */
    for (ch = 32; ch < 256; ++ch) {
      char *p;

      for (p = (char *) 0x80A; p < (char *) 0xC00; ++p) {
	*p = ch;
	if (((int)p & 0xF) == 47+10)
	  p += 15;
      }

    getchar();
#endif

    return 0;
}

int c;

int foo()
{
    return ++c;
}
