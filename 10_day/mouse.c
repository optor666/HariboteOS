#include "fifo.h"
#include "io.h"
#include "keyboard.h"
#include "mouse.h"

struct FIFO8 mousefifo;
unsigned char mousebuf[MOUSE_FIFO_BUF_SIZE];

/* 鼠标有效 */
void enable_mouse(struct MouseDec *mdec) {
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
    /* 顺利的话，ACK(0xfa)会被送过来 */ 
    mdec->phase = 0;    /* 等待0xfa的阶段 */
}

int mouse_decode(struct MouseDec *mdec, unsigned char dat) {
  if (mdec->phase == 0) {
    // 等待鼠标的0xfa状态
    if (dat == 0xfa) {
      mdec->phase = 1;
    }
    return 0;
  } else if (mdec->phase == 1) {
    // 等待鼠标的第一个字节
    if ((dat & 0xc8) == 0x08) {
      // 如果第一个字节正确
      mdec->buf[0] = dat;
      mdec->phase = 2;
    }
    return 0;
  } else if (mdec->phase == 2) {
    // 等待鼠标的第二个字节
    mdec->buf[1] = dat;
    mdec->phase = 3;
    return 0;
  } else if (mdec->phase == 3) {
    // 等待鼠标的第三个字节
    mdec->buf[2] = dat;
    mdec->phase = 1;

    mdec->btn = mdec->buf[0] & 0x07;
    mdec->x = mdec->buf[1];
    mdec->y = mdec->buf[2];

    if ((mdec->buf[0] & 0x10) != 0) {
      mdec->x |= 0xffffff00;
    }

    if ((mdec->buf[0] & 0x20) != 0) {
      mdec->y |= 0xffffff00;
    }

    mdec->y = -mdec->y; // 鼠标的y方向与画面符号相反

    return 1;
  }
  return -1;
}