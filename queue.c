#include "RIMS.h"

typedef struct _Q4uc {
   unsigned char buf[4];
   unsigned char cnt;
} Q4uc;

Q4uc btnQ; // new queue to hold buttons

void Q4ucInit(Q4uc *Q) {
    (*Q).cnt=0;
}

Q4ucInit(&btnQ);

unsigned char Q4ucFull(Q4uc Q) {
   return (Q.cnt == 4);
}