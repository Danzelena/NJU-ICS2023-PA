#include <am.h>
#include <nemu.h>
#include <stdio.h>


//(uint32_t)
//  >0x8000: keydown
//  else: up
#define KEYDOWN_MASK 0x8000


void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  int key = (int32_t)inl(KBD_ADDR);
  printf("key=%d\n", key);
  kbd->keydown = key > KEYDOWN_MASK;
  kbd->keycode = key > KEYDOWN_MASK ? (key - KEYDOWN_MASK) : key;
//   kbd->keydown = 0;
//   kbd->keycode = AM_KEY_NONE;
}
