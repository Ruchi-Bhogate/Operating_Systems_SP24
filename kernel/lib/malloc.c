#include <barelib.h>
#include <malloc.h>
#include <thread.h>

extern uint32* mem_start;
extern uint32* mem_end;
static alloc_t* freelist;

/*  Sets the 'freelist' to 'mem_start' and creates  *
 *  a free allocation at that location for the      *
 *  entire heap.                                    */
//--------- This function is complete --------------//
void heap_init(void) {
  freelist = (alloc_t*)mem_start;
  freelist->size = get_stack(NTHREADS) - mem_start - sizeof(alloc_t);
  freelist->state = M_FREE;
  freelist->next = NULL;
}

/*  Locates a free block large enough to contain a new    *
 *  allocation of size 'size'.  Once located, remove the  *
 *  block from the freelist and ensure the freelist       *
 *  contains the remaining free space on the heap.        *
 *  Returns a pointer to the newly created allocation     */
void* malloc(uint64 size) {
  alloc_t* curr = freelist;
  alloc_t* prev = NULL;

  if (curr == NULL){
    return 0;
  }

  while(curr->size < size){
    if (curr->next == NULL){
      return 0;
    }
    prev = curr;
    curr = curr->next;
  }

  if(curr->size - size >= sizeof(alloc_t) + 1){
    alloc_t* new_curr = (alloc_t*)((void*)curr + sizeof(alloc_t) + size);
    new_curr->size = curr->size - size - sizeof(alloc_t);
    new_curr->state = M_FREE;
    new_curr->next = curr->next;
    curr->size = size;
    curr->next = new_curr;
    //freelist = new_curr;
  }

  if (prev!=NULL){
    prev->next = curr->next;
  }
  else{
    freelist = curr->next;
  }

  curr->state = M_USED;
  curr->next = NULL;

  return (void*)(curr + 1);
}

alloc_t* merge(alloc_t* addr, alloc_t* to_addr){
  if (((alloc_t*)((void*)addr + sizeof(alloc_t) + addr->size)) == to_addr){
    addr->size = addr->size + sizeof(alloc_t) + to_addr->size;
    addr->next = to_addr->next;
    return addr;
  }else{
    addr->next = to_addr;
    return addr;
  }
}

/*  Free the allocation at location 'addr'.  If the newly *
 *  freed allocation is adjacent to another free          *
 *  allocation, coallesce the adjacent free blocks into   *
 *  one larger free block.                                */
void free(void* addr) {
  alloc_t* header_block = ((alloc_t*)addr - 1);//alloc_t* header_block = addr - 1;//(alloc_t*)addr - sizeof(alloc_t);
  header_block->state = M_FREE;
  alloc_t* curr = freelist;

  // if we want to place addr before free list
  if (header_block < curr){
    header_block->next = freelist;
    freelist = header_block;
    merge(header_block,freelist);
    return;
  }

  while( !(curr < header_block) || (curr->next != NULL && curr->next <= header_block)){
    curr = curr->next;
  }
  header_block->next = curr->next;
  alloc_t* temp = merge(header_block, curr->next);
  merge(curr,temp);
  return;
}
