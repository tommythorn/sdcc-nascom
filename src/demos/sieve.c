/*
OMGs: You cannot enter ` nor ~ on the Nascom keyboard?
*/
#include <stdio.h>

int main()
{
  int k, f;

  for (k = 2;; ++k) {
    for (f = 2; f < k-1; ++f)
      if (k % f == 0)
	goto skip;
    printf("%5d ", k);
  skip:;
  }
}
