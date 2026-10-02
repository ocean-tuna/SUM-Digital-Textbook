/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>

typedef struct LinkedList * LinkedListPtr[256];

struct LinkedList {
  unsigned long id;
  unsigned char flag;
  LinkedListPtr next;
};

typedef struct LinkedList LinkedListType;

LinkedListType lst[] = {
  {
      1u,
      'c',
      { &lst[2u] }
  },
  {
      4u,
      'z',
      { &lst[0u], &lst[3u] }
  },
  {
       7u,
      'd',
      { &lst[3u] }
  },
  {
       0u,
      'g',
      { &lst[4u] }
  },
  {
       12u,
      'a',
      { &lst[1] }
  }
};

int main()
{
    printf("| %ld | %c |\n      |--->%ld %c\n      |--->%ld %c\n", 
        lst[1u].id, 
        lst[1u].flag, 
        lst[1u].next[0]->id, 
        lst[1u].next[0]->flag,
        lst[1u].next[1u]->id, 
        lst[1u].next[1u]->flag);

    return 0;
}
