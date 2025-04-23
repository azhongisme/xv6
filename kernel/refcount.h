#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "spinlock.h"

struct refcount
{
  struct spinlock lock;
  int ref_array[PHYSTOP / PGSIZE + 1];
};
extern struct refcount refcount;