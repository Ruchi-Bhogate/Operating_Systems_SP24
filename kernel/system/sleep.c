#include <interrupts.h>
#include <queue.h>
#include <thread.h>
#include <syscall.h>

/*  Places the thread into a sleep state and inserts it into the sleep delta list. */

int32 sleep(uint32 threadid, uint32 delay) {
  char mask;
  mask = disable_interrupts();
  if (delay == 0){
    raise_syscall(RESCHED);
    restore_interrupts(mask);
    return 0;
  }

  if (thread_table[threadid].state == TH_READY || thread_table[threadid].state == TH_RUNNING){
    if (thread_table[threadid].state == TH_READY){
      thread_remove(threadid); // does this remove from ready_list?
    }
    thread_table[threadid].state = TH_SLEEP;
    int key_value = delay;
    // if sleep_list is empty
    // if (thread_queue[sleep_list].qnext == sleep_list){
    //   thread_queue[sleep_list].qnext = threadid;
    //   thread_queue[sleep_list].qprev = threadid;
    //   thread_queue[threadid].qprev = sleep_list;
    //   thread_queue[threadid].qnext = sleep_list;
    //   thread_queue[threadid].key = key_value;
    //   raise_syscall(RESCHED);
    //   restore_interrupts(mask);
    //   return 0;
    // }
    // 21 -> 1 (100) -> 2 (200) example sleep_list
    int current_index = thread_queue[sleep_list].qnext;
    while ((current_index != sleep_list) && (key_value <= thread_queue[current_index].key)){
      key_value = key_value - thread_queue[current_index].key;
      current_index = thread_queue[current_index].qnext;
    }
    //if we need to insert at end of sleep_list
    // if (thread_queue[current_index].qnext == sleep_list){
    //   key_value = key_value - thread_queue[current_index].key;
    //   thread_queue[current_index].qnext = threadid;
    //   thread_queue[threadid].qprev = current_index;
    //   thread_queue[threadid].qnext = sleep_list;
    //   thread_queue[sleep_list].qprev = threadid;
    //   thread_queue[threadid].key = key_value;
    // }
    //if we need to insert in middle of sleep_list
    //else{
      int prev_node = thread_queue[current_index].qprev;
      thread_queue[prev_node].qnext = threadid;
      thread_queue[current_index].qprev = threadid;
      thread_queue[threadid].qprev = prev_node;
      thread_queue[threadid].qnext = current_index;
      thread_queue[threadid].key = key_value;
      thread_queue[current_index].key = thread_queue[current_index].key - key_value;
    //}

  }

  // IS THIS THE CORRECT PLACE TO RAISE RESCHED CALL??
  raise_syscall(RESCHED);
  restore_interrupts(mask);
  return 0;
}

/*  If the thread is in the sleep state, remove the thread from the  *
 *  sleep queue and resumes it.                                      */
// CHECK ---------
int32 unsleep(uint32 threadid) {
  char mask;
  mask = disable_interrupts();
  int id = thread_queue[threadid].qnext;
  if (thread_table[threadid].state != TH_SLEEP || id == threadid){
    return -1;
  }
  int key_value = thread_queue[threadid].key;
  thread_remove(threadid); // how will this remove from sleep_list?
  thread_enqueue(ready_list,threadid);
  thread_queue[id].key = thread_queue[id].key + key_value;
  raise_syscall(RESCHED);
  
  restore_interrupts(mask);
  return 0;
}
