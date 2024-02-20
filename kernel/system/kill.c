#include <barelib.h>
#include <thread.h>
#include <interrupts.h>
#include <syscall.h>
#include <queue.h>

/*  Takes an index into the thread_table.  If that thread is not free (in use),  *
 *  sets the thread to defunct and raises a RESCHED syscall.                     */
int32 kill_thread(uint32 threadid)
{
  char mask;
  if (threadid >= NTHREADS || thread_table[threadid].state == TH_FREE) /*                                                             */
    return -1;                                                         /*  Return if the requested thread is invalid or already free  */

  mask = disable_interrupts(); /*  Ensure cleanup cannot be interrupted  */
  for (int i = 0; i < NTHREADS; i++)
  {                                         /*                                        */
    if (thread_table[i].parent == threadid) /*  Identify all children of the thread   */
      thread_table[i].state = TH_FREE;      /*  Reap running children threads         */
  }
  thread_table[threadid].state = TH_DEFUNCT; /*  Set the thread's state to TH_DEFUNCT  */

  thread_remove(threadid);

  // int prev_node = thread_queue[threadid].qprev;
  // int next_node = thread_queue[threadid].qnext;

  // // thread_queue[threadid].key = threadid;
  // thread_queue[threadid].qprev = threadid;
  // thread_queue[threadid].qnext = threadid;

  // thread_queue[prev_node].qnext = next_node;
  // thread_queue[next_node].qprev = prev_node;

  restore_interrupts(mask); /*  Restore the interrupt mask            */
  raise_syscall(RESCHED);   /*  schedules another thread              */
  return 0;
}
