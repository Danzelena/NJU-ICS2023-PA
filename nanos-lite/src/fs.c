
#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

// 文件(文件记录表中的文件)
typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;   // 读函数指针,NULL for normal files
  WriteFn write; // 写函数指针,NULL for normal files
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
/* 文件记录表 file_table 是 类型为Finfo的数组,*/
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
    [FD_FB] = {"/dev/fb", 0, 0, 0,invalid_read, fb_write},
    {"/dev/events", 0, 0, 0, events_read, invalid_write},
    {"/proc/dispinfo", 0, 0, 0,dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs()
{
  // tODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = config.width * config.height * sizeof(uint32_t);

  for (int i = 6; i < sizeof(file_table) / sizeof(Finfo); ++i){
    // 我不确定会不会自动刷为0，不如再做一次
    file_table[i].open_offset = 0;
    file_table[i].write = NULL;
    file_table[i].read = NULL;
  }
}

int f_find(const char *pathname)
{
  int ft_num = sizeof(file_table) / sizeof(Finfo);
  int ret = -1;
  bool found = false;
  
  for (int i = 0; i < ft_num; i++)
  {
    // printf("file:%s\n", file_table[i].name);

    if (strcmp(pathname, file_table[i].name) == 0)
    {
      // printf("(f_find)path=%s,tab=%s\n",pathname, file_table[i].name);
      if (!found)
      {
        ret = i;
        found = true;
      }
      else
      {
        panic("Sorry!has many(>1)same name files\n");
      }
    }
  }
  return ret;
}
int fs_open(const char *pathname, int flags, int mode)
{
  // ignored flags and mode

  // search the file table
  int ret = f_find(pathname);
  if (ret == -1)
  {
    printf("(fs_open)Sorry! file %s not found!\n",pathname);
    // panic("(fs_open)Sorry! file not found!\n");
  }
  return ret;
}

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
/**
 * return:
 * -1 :error
 * 0:no bytes to read
 * else: the number of bytes read
 */
size_t fs_read(int fd, void *buf, size_t len)
{
  // Finfo *info = &file_table[fd];
  // size_t real_len;
  
  // //assert(info->open_offset + len <= info->size);
  // if (info->read){
  //   real_len = info->read(buf, info->open_offset, len);
  //   info->open_offset += real_len;
  // }else {
  //   real_len = info->open_offset + len <= info->size ?
  //   len : info->size - info->open_offset;
  //   ramdisk_read(buf, info->disk_offset + info->open_offset, real_len);
  //   info->open_offset += real_len;
  // }

  // return real_len;
  
  if (file_table[fd].read != NULL)
  {
   int ret = file_table[fd].read(buf, 0, len);
   if(ret > len){
    panic("ret > len\n");
   }
   file_table[fd].open_offset += ret;
   return ret;
  }
  else
  { 
    // printf("png_off=%d\n",file_table[66].open_offset);
    if (file_table[fd].open_offset > file_table[fd].size)
    {
      panic("(fs_read)fd=%d,op_off=%d,size=%d,len=%d, no bytes to read\n",fd,file_table[fd].open_offset ,file_table[fd].size,len);
      return 0;
    }else if(file_table[fd].open_offset == file_table[fd].size){
      return 0;
    }// FORWARD:code is repeated
    if (file_table[fd].disk_offset + file_table[fd].open_offset + len > file_table[fd + 1].disk_offset)
    {
      // panic("(fs_read)Don't have enough size to read\n");
      len = file_table[fd].size - file_table[fd].open_offset;
      // return 0;
    }
    size_t ret = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    // printf("ret:%d\n",ret);
    return ret;
  }




  // printf("fd=%d,len=%d\n",fd,len);
  // printf("plus1:%d,plus2:%d\n",file_table[fd].disk_offset + file_table[fd].open_offset + len , file_table[fd + 1].disk_offset);
  // check if out of boundary

  // check open_offset

  // If the file offset is at od past the end of file, return 0
}

// default whence: SEEK_SET
int fs_lseek(int fd, size_t offset, int whence)
{
  // printf("whence:%d\n",whence);
  if (whence != SEEK_SET && whence != SEEK_END && whence != SEEK_CUR)
  {
    panic("(fs_lseek)Sorry! whence is not SEEK_SET\n");
  }
  if (whence == SEEK_SET)
  {
    if (offset <= file_table[fd].size)
    {
      file_table[fd].open_offset = offset;
      return file_table[fd].open_offset;
    }
    panic("(fs_lseek)Sorry! fs_lseek() offset > size\n");
    return -1;
  }
  else if (whence == SEEK_END)
  {
    file_table[fd].open_offset = offset + file_table[fd].size;
    return file_table[fd].open_offset;
  }
  else if (whence == SEEK_CUR)
  {
    file_table[fd].open_offset += offset;
    return file_table[fd].open_offset;
  }
  panic("(fs_lseek)\n");
  // return offset;
}

size_t fs_write(int fd, void *buf, size_t len)
{
  // handle stdout and stderrir( use`putch()`)
  printf("(fs_write)(%d, %d)\n", fd, len);
  if (file_table[fd].write != NULL)
  {
    int ret = file_table[fd].write(buf, file_table[fd].open_offset, len);
    if(ret > len){
      panic("fd:%d\nret:%d > len:%d\n",fd,ret,len);
    }
    file_table[fd].open_offset += ret;
    return ret;
  }

  else
  {
    if (file_table[fd].open_offset >= file_table[fd].size)
    {
      panic("(fs_write)no bytes to write\n");
      return 0;
    }
    if (file_table[fd].disk_offset + file_table[fd].open_offset + len > file_table[fd + 1].disk_offset)
    {
      panic("(fs_write)Don't have enough size to write\n");
      len = file_table[fd].size - file_table[fd].open_offset;
      return 0;
    }
    size_t ret = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return ret;
  }
  printf("(fs_write)finish\n", fd, len);
  return len;
}

size_t fs_close(int fd)
{
  file_table[fd].open_offset = 0;
  return 0;
}