#include "refcount.h"
#include "defs.h"
#include "types.h"

void
increment(int idx) {
  acquire(&refcount.lock);
  refcount.ref_array[idx]++;
  release(&refcount.lock);
}

void
decrement(int idx) {
  acquire(&refcount.lock);
  refcount.ref_array[idx]--;
  release(&refcount.lock);
}

int
getref(int idx) {
  int temp;
  acquire(&refcount.lock);
  temp = refcount.ref_array[idx];
  release(&refcount.lock);
  return temp;
}

int
cowhandler(pagetable_t pagetable, uint64 va) {
  // printf("cowhandler\n");
  uint64 newpa;
  uint flags;
  if (va > MAXVA) {
    printf("va: %d > MAXVA: %d", va, MAXVA);
    return -1;
  }
  pte_t *pte = walk(pagetable, va, 0);
  if (pte == 0) 
    return -1;
  if ((*pte & PTE_F) == 0 || (*pte & PTE_V ) == 0) {
    return -1;
  }
  uint64 pa = PTE2PA(*pte);
  int ref = getref(pa / PGSIZE);
  if (ref == 1) {
    *pte |= PTE_W;
    *pte &= ~PTE_F;
    // printf("ref = 0, cowhandler retrurn 0\n");
    return 0;
  }
  if (ref > 1) {
    newpa = (uint64)kalloc();
    if (newpa == 0) 
      return -1;
    memmove((void *)newpa, (void *)pa, PGSIZE);
    flags = PTE_FLAGS(*pte);
    *pte = (PA2PTE(newpa) | flags | PTE_W);
    *pte &= ~PTE_F;
    kfree((void *)pa);
    // printf("ref > 1, cowhandler retrurn 0\n");
    return 0;
  }
  return -1;
}