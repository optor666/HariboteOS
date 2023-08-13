#include "api.h"
#include "console.h"

void hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax) {
  int cs_base = *((int *) 0xfe8); // code segement address
  struct Console *cons = (struct Console *) *((int *)0x0fec);

  if (edx == 1) {
    cons_putchar(cons, eax & 0xff, 1);
  } else if (edx == 2) {
    cons_putstr(cons, (char *)ebx + cs_base);
  } else if (edx == 3) {
    cons_putnstr(cons, (char *)ebx + cs_base, ecx);
  }
}