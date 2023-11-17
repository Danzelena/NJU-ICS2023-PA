#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init()
{
  // int i;
  // int w = (int)(inl(VGACTL_ADDR) >> 16); // TODO: get the correct width
  // int h = (int)(inw(VGACTL_ADDR));       // TODO: get the correct height
  // printf("width: %d, high:%d\n", w, h);  // expected 400*300
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i++)
  //   fb[i] = i + 120;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg)
{
  *cfg = (AM_GPU_CONFIG_T){
      .present = true, .has_accel = false, .width = (int)(inl(VGACTL_ADDR) >> 16), .height = (int)(inw(VGACTL_ADDR)), .vmemsz = 0};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl)
{

  if (ctl->sync)
  {
    printf("Update!\n");
    outl(SYNC_ADDR, 1);
  }
  // }else{

  // printf("NoUpdate\n");
  // }
  int width = (int)(inl(VGACTL_ADDR) >> 16); // TODO: get the correct width
  // int height = (int)(inw(VGACTL_ADDR));      // TODO: get the correct height
  // printf("width: %d, high:%d\n", width, height); // expected 400*300

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  int i, j;
  for (j = ctl->y; j < ctl->y + ctl->h ; j++)
  {
    for (i = ctl->x; i < ctl->x + ctl->w; i++)
    {

      // printf("(gpu_write)x:%d,y:%d,w:%d,h:%d\n", i, j, ctl->w, ctl->h);
      fb[j * width + i] = ((uint32_t *)ctl->pixels)[(j - ctl->y) * 15 + (i - ctl->x)];
    }
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status)
{
  status->ready = true;
}
