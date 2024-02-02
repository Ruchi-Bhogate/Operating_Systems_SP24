#include <barelib.h>
#include <thread.h>
#include <interrupts.h>
#include <syscall.h>

/*  Takes an index into the thread_table.  If the thread is TH_DEFUNCT,  *
 *  mark  the thread  as TH_FREE  and return its  `retval`.   Otherwise  *
 *  raise RESCHED and loop to check again later.                         */
byte join_thread(uint32 threadid)
{
  char mask = disable_interrupts();
  byte result;

  if (thread_table[threadid].state == TH_FREE)
  {
    restore_interrupts(mask);
    return NTHREADS;
  }

  while (thread_table[threadid].state != TH_DEFUNCT)
  {
    raise_syscall(RESCHED);
  }
  thread_table[threadid].state = TH_FREE;
  result = thread_table[threadid].retval;
  restore_interrupts(mask);
  return result;
}
