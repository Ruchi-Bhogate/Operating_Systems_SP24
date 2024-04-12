#include <barelib.h>
#include <interrupts.h>
#include <bareio.h>
#include <shell.h>
#include <thread.h>
#include <queue.h>
#include <malloc.h>
#include <tty.h>
#include<fs.h>
/*
 *  This file contains the C code entry point executed by the kernel.
 *  It is called by the bootstrap sequence once the hardware is configured.
 *      (see bootstrap.s)
 */

extern uint32 *text_start; /*                                             */
extern uint32 *data_start; /*  These variables automatically contain the  */
extern uint32 *bss_start;  /*  lowest  address of important  segments in  */
extern uint32 *mem_start;  /*  memory  created  when  the  kernel  boots  */
extern uint32 *mem_end;    /*    (see mmap.c and kernel.ld for source)    */

void ctxload(uint64 **);

uint32 boot_complete = 0;

void initialize(void)
{
  char mask;
  mask = disable_interrupts();
  uart_init();
  tty_init();

  printf("Kernel start: %x\n", text_start);
  printf("--Kernel size: %d\n", (data_start - text_start));
  printf("Globals start: %x\n", data_start);
  printf("Heap/Stack start: %x\n", mem_start);
  printf("--Free Memory Available: %d\n", (mem_end - mem_start));
  // shell(NULL);

  thread_queue[ready_list].qnext = thread_queue[ready_list].qprev = ready_list;
  thread_queue[sleep_list].qnext = thread_queue[sleep_list].qprev = sleep_list;

  for (int i = 0; i < NTHREADS; i++)
  {
    thread_table[i].state = TH_FREE;
  }

  for (int i = 0; i < NTHREADS + 2; i++)
  {
    thread_queue[i].qprev = i;
    thread_queue[i].qnext = i;
  }

  // Initializing heap
  heap_init();

  // Initializing file system
  bs_mk_ramdisk(MDEV_BLOCK_SIZE,MDEV_NUM_BLOCKS);
  fs_mkfs();
  fs_mount();

  restore_interrupts(mask);
  boot_complete = 1;


  int shell_id = create_thread(shell, NULL, 0);
  current_thread = shell_id;
  thread_table[current_thread].state = TH_RUNNING;
  ctxload(&thread_table[current_thread].stackptr);
  // printf("shell thread is ready");
}
