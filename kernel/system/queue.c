#include <queue.h>

/*  Queues in bareOS are all contained in the 'thread_queue' array.  Each queue has a "root"
 *  that contains the index of the first and last elements in that respective queue.  These
 *  roots are  found at the end  of the 'thread_queue'  array.  Following the 'qnext' index
 *  of each element, starting at the "root" should always eventually lead back to the "root".
 *  The same should be true in reverse using 'qprev'.                                          */

queue_t thread_queue[NTHREADS + 2]; /*  Array of queue elements, one per thread plus one for the read_queue root */
                                    /* Added one more plus one to accomodate head of sleep_list */
uint32 ready_list = NTHREADS + 0;   /*  Index of the read_list root  */
uint32 sleep_list = NTHREADS + 1;   /*  Index of the sleep_list root */

/*  'thread_enqueue' takes an index into the thread_queue  associated with a queue "root"  *
 *  and a threadid of a thread to add to the queue.  The thread will be added to the tail  *
 *  of the queue,  ensuring that the  previous tail of the queue is correctly threaded to  *
 *  maintain the queue.                                                                    */
void thread_enqueue(uint32 queue, uint32 threadid)
{
  int key_value = thread_table[threadid].priority;
  int id = queue;

  // if (thread_queue[threadid].qnext != threadid)
  // {
  //   return;
  // }

  while (1)
  {
    id = thread_queue[id].qnext;
    if (id == queue)
    {
      break;
    }
    if (key_value > thread_queue[id].key)
    {
      break;
    }
  }
  int prev_node = thread_queue[id].qprev;
  thread_queue[prev_node].qnext = threadid;
  thread_queue[id].qprev = threadid;
  thread_queue[threadid].qprev = prev_node;
  thread_queue[threadid].qnext = id;
  thread_queue[threadid].key = key_value;
  return;
}

/*  'thread_dequeue' takes a queue index associated with a queue "root" and removes the  *
 *  thread at the head of the queue and returns the index of that thread, ensuring that  *
 *  the queue  maintains its structure and the head correctly points to the next thread  *
 *  (if any).                                                                            */
uint32 thread_dequeue(uint32 queue)
{

  int id = thread_queue[queue].qnext;

  if (id == queue)
  {
    return NTHREADS;
  }

  int prev_node = thread_queue[id].qprev;
  int next_node = thread_queue[id].qnext;
  thread_queue[prev_node].qnext = next_node;
  thread_queue[next_node].qprev = prev_node;

  thread_queue[id].qprev = id;
  thread_queue[id].qnext = id;

  return id;
}

void thread_remove(uint32 queue)
{
  int prev_node = thread_queue[queue].qprev;
  int next_node = thread_queue[queue].qnext;

  // thread_queue[queue].key = queue;
  thread_queue[queue].qprev = queue;
  thread_queue[queue].qnext = queue;

  thread_queue[prev_node].qnext = next_node;
  thread_queue[next_node].qprev = prev_node;
  return;
}