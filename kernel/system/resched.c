#include <barelib.h>
#include <thread.h>
#include <queue.h>
// #include <syscall.h>

/*  'resched' places the current running thread into the ready state  *
 *  and  places it onto  the tail of the  ready queue.  Then it gets  *
 *  the head  of the ready  queue  and sets this  new thread  as the  *
 *  'current_thread'.  Finally,  'resched' uses 'ctxsw' to swap from  *
 *  the old thread to the new thread.                                 */
int32 resched(void)
{
  int current_index = current_thread;
  // int next_index = (current_index + 1) % NTHREADS;
  int next_index = thread_dequeue(ready_list);
  if (next_index == NTHREADS)
  {
    return 0;
  }
  // int i = next_index;
  // for (; i != current_index; i = (i + 1) % NTHREADS)
  // {
  // if (thread_table[i].state == TH_READY)
  //   {
  if (thread_table[current_index].state == TH_READY || thread_table[current_index].state == TH_RUNNING)
  {
    thread_table[current_index].state = TH_READY;
    // thread_table[i].state = TH_RUNNING;

    thread_enqueue(ready_list, current_index);
    // current_thread = i;
  }
  thread_table[next_index].state = TH_RUNNING;
  current_thread = next_index;
  ctxsw(&thread_table[current_thread].stackptr, &thread_table[current_index].stackptr);
  //   if (i == current_index)
  //   {
  //     return 0;
  // }
  return 0;
}
