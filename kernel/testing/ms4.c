#include <barelib.h>
#include <syscall.h>
#include <queue.h>

#define test_count(x) sizeof(x) / sizeof(x[0])
#define init_tests(x, c)      \
  for (int i = 0; i < c; i++) \
  x[i] = "OK"
#define assert(test, ls, err) \
  if (!(test))                \
  ls = err

void _psudoprint(const char *);
void test_runner(const char *, uint32, void (*)(void));
void test_printer(const char *, char **, const char **, uint32);
extern byte __enable_resched;

static const char *general_prompt[] = {
    "  Program Compiles:             "};
static const char *init_prompt[] = {
    "    List initialization:        ",
};
static const char *enqueue_prompt[] = {
    "    List integrity [1-thread]:  ",
    "    List integrity [2-threads]: ",
    "    List integrity [3-threads]: ",
};
static const char *dequeue_prompt[] = {
    "    First pop:                  ",
    "    First return:               ",
    "    Second pop:                 ",
    "    Second return:              ",
    "    Third pop:                  ",
    "    Third return:               ",
};
static const char *order_prompt[] = {
    "    Enqueue first:              ",
    "    Enqueue second:             ",
    "    Enqueue third:              ",
    "    Dequeue first:              ",
    "    Dequeue second:             ",
    "    Dequeue third:              ",
};
static const char *resched_prompt[] = {
    "  Enqueue on resume:            ",
    "  Dequeue on suspend:           ",
    "  Resched dequeues/enqueues:    ",
};
static char *general[test_count(general_prompt)];
static char *init[test_count(init_prompt)];
static char *enqueue[test_count(enqueue_prompt)];
static char *dequeue[test_count(dequeue_prompt)];
static char *order[test_count(order_prompt)];
static char *resched[test_count(resched_prompt)];

static byte dummy_thread(char *arg)
{
  while (1)
    raise_syscall(RESCHED);
  return 0;
}

static byte resched_thread(char *arg)
{
  assert(thread_queue[ready_list].qnext == 0, resched[2], "FAIL - 'ready_list.qnext' does not point to shell after resched");
  assert(thread_queue[ready_list].qprev == 0, resched[2], "FAIL - 'ready_list.qprev' does not point to shell after resched");
  return 0;
}

static void general_tests(void)
{
  return;
}

static void reset(void)
{
  thread_queue[ready_list].qnext = thread_queue[ready_list].qprev = ready_list;
  for (int i = 1; i < NTHREADS; i++)
    thread_table[i].state = TH_FREE;
}

static void initialize_tests(void)
{
  assert(thread_queue[ready_list].qnext == ready_list, init[0], "FAIL - 'ready_list.qnext' does not point to itself");
  assert(thread_queue[ready_list].qprev == ready_list, init[0], "FAIL - 'ready_list.qprev' does not point to itself");
  reset();
}

static void enqueue_tests(void)
{
  int32 tid1, tid2, tid3;
  tid1 = create_thread(dummy_thread, "", 0);
  thread_enqueue(ready_list, tid1);

  assert(thread_queue[ready_list].qnext == tid1, enqueue[0], "FAIL - 'ready_list.qnext' does not point to new thread");
  assert(thread_queue[ready_list].qprev == tid1, enqueue[0], "FAIL - 'ready_list.qprev' does not point to new thread");
  assert(thread_queue[tid1].qprev == ready_list, enqueue[0], "FAIL - new thread's 'qprev' does not point to 'ready_list'");
  assert(thread_queue[tid1].qnext == ready_list, enqueue[0], "FAIL - new thread's 'qnext' does not point to 'ready_list'");

  tid2 = create_thread(dummy_thread, "", 0);
  thread_enqueue(ready_list, tid2);

  assert(thread_queue[ready_list].qnext == tid1, enqueue[1], "FAIL - 'ready_list.qnext' does not point to first thread");
  assert(thread_queue[ready_list].qprev == tid2, enqueue[1], "FAIL - 'ready_list.qprev' does not point to new thread");
  assert(thread_queue[tid1].qnext == tid2, enqueue[1], "FAIL - first thread's 'qnext' does not point to new thread");
  assert(thread_queue[tid1].qprev == ready_list, enqueue[1], "FAIL - first thread's 'qprev' does not point to 'ready_list'");
  assert(thread_queue[tid2].qnext == ready_list, enqueue[1], "FAIL - new thread's 'qnext' does not point to 'ready_list'");
  assert(thread_queue[tid2].qprev == tid1, enqueue[1], "FAIL - new thread's 'qprev' does not point to first thread");

  tid3 = create_thread(dummy_thread, "", 0);
  thread_enqueue(ready_list, tid3);

  assert(thread_queue[ready_list].qnext == tid1, enqueue[2], "FAIL - 'ready_list.qnext' does not point to first thread");
  assert(thread_queue[ready_list].qprev == tid3, enqueue[2], "FAIL - 'ready_list.qprev' does not point to new thread");
  assert(thread_queue[tid1].qnext == tid2, enqueue[2], "FAIL - first thread's 'qnext' does not point to second thread");
  assert(thread_queue[tid1].qprev == ready_list, enqueue[2], "FAIL - first thread's 'qprev' does not point to 'ready_list'");
  assert(thread_queue[tid2].qnext == tid3, enqueue[2], "FAIL - second thread's 'qnext' does not point to new thread");
  assert(thread_queue[tid2].qprev == tid1, enqueue[2], "FAIL - second thread's 'qprev' does not point to first thread");
  assert(thread_queue[tid3].qnext == ready_list, enqueue[2], "FAIL - new thread's 'qnext' does not point to 'ready_list'");
  assert(thread_queue[tid3].qprev == tid2, enqueue[2], "FAIL - new thread's 'qprev' does not point to second thread");

  reset();
}

static void dequeue_tests(void)
{
  int32 tid1, tid2, tid3;
  int32 ret1, ret2, ret3;
  tid1 = create_thread(dummy_thread, "", 0);
  tid2 = create_thread(dummy_thread, "", 0);
  tid3 = create_thread(dummy_thread, "", 0);
  thread_enqueue(ready_list, tid1);
  thread_enqueue(ready_list, tid2);
  thread_enqueue(ready_list, tid3);

  ret1 = thread_dequeue(ready_list);
  assert(thread_queue[ready_list].qnext == tid2, dequeue[0], "FAIL - 'ready_list.qnext' does not point to second thread");
  assert(thread_queue[ready_list].qprev == tid3, dequeue[0], "FAIL - 'ready_list.qprev' does not point to third thread");
  assert(thread_queue[tid2].qnext == tid3, dequeue[0], "FAIL - second thread's 'qnext' does not point to third thread");
  assert(thread_queue[tid2].qprev == ready_list, dequeue[0], "FAIL - second thread's 'qprev' does not point to 'ready_list'");
  assert(thread_queue[tid3].qnext == ready_list, dequeue[0], "FAIL - third thread's 'qnext' does not point to 'ready_list'");
  assert(thread_queue[tid3].qprev == tid2, dequeue[0], "FAIL - third thread's 'qprev' does not point to first thread");
  assert(ret1 = tid1, dequeue[1], "FAIL - return value is not the first enqueued thread'");

  ret2 = thread_dequeue(ready_list);
  assert(thread_queue[ready_list].qnext == tid3, dequeue[2], "FAIL - 'ready_list.qnext' does not point to third thread");
  assert(thread_queue[ready_list].qprev == tid3, dequeue[2], "FAIL - 'ready_list.qprev' does not point to third thread");
  assert(thread_queue[tid3].qnext == ready_list, dequeue[2], "FAIL - third thread's 'qnext' does not point to 'ready_list'");
  assert(thread_queue[tid3].qprev == ready_list, dequeue[2], "FAIL - third thread's 'qprev' does not point to 'ready_list'");
  assert(ret2 = tid2, dequeue[3], "FAIL - return value is not the second enqueued thread'");

  ret3 = thread_dequeue(ready_list);
  assert(thread_queue[ready_list].qnext == ready_list, dequeue[4], "FAIL - 'ready_list.qnext' does not point to 'ready_list'");
  assert(thread_queue[ready_list].qprev == ready_list, dequeue[4], "FAIL - 'ready_list.qprev' does not point to 'ready_list'");
  assert(ret3 = tid3, dequeue[5], "FAIL - return value is not the third enqueued thread'");

  reset();
}

static void order_tests(void)
{
  int32 tid1, tid2, tid3;
  tid1 = create_thread(dummy_thread, "", 0);
  tid2 = create_thread(dummy_thread, "", 0);
  tid3 = create_thread(dummy_thread, "", 0);

  thread_enqueue(ready_list, tid3);
  assert(thread_queue[ready_list].qnext == tid3, order[0], "FAIL - 'ready_list.qnext' does not point to out-of-order entry 1");
  assert(thread_queue[ready_list].qprev == tid3, order[0], "FAIL - 'ready_list.qprev' does not point to out-of-order entry 1");

  thread_enqueue(ready_list, tid1);
  assert(thread_queue[ready_list].qnext == tid3, order[1], "FAIL - 'ready_list.qnext' does not point to out-of-order entry 1");
  assert(thread_queue[ready_list].qprev == tid1, order[1], "FAIL - 'ready_list.qprev' does not point to out-of-order entry 2");
  assert(thread_queue[tid3].qnext == tid1, order[1], "FAIL - out-of-order entry 1 'qnext' does not point to out-of-order entry 2");
  assert(thread_queue[tid3].qprev == ready_list, order[1], "FAIL - out-of-order entry 1 'qprev' does not point to 'ready_list'");
  assert(thread_queue[tid1].qnext == ready_list, order[1], "FAIL - out-of-order entry 2 'qnext' does not point to 'ready_list'");
  assert(thread_queue[tid1].qprev == tid3, order[1], "FAIL - out-of-order entry 2 'qprev' does not point to out-of-order entry 1");

  thread_enqueue(ready_list, tid2);
  assert(thread_queue[ready_list].qnext == tid3, order[2], "FAIL - 'ready_list.qnext' does not point to out-of-order entry 1");
  assert(thread_queue[ready_list].qprev == tid2, order[2], "FAIL - 'ready_list.qprev' does not point to out-of-order entry 3");
  assert(thread_queue[tid3].qnext == tid1, order[2], "FAIL - out-of-order entry 1 'qnext' does not point to out-of-order entry 2");
  assert(thread_queue[tid3].qprev == ready_list, order[2], "FAIL - out-of-order entry 1 'qprev' does not point to 'ready_list'");
  assert(thread_queue[tid1].qnext == tid2, order[2], "FAIL - out-of-order entry 2 'qnext' does not point to out-of-order entry 3");
  assert(thread_queue[tid1].qprev == tid3, order[2], "FAIL - out-of-order entry 2 'qprev' does not point to out-of-order entry 1");
  assert(thread_queue[tid2].qnext == ready_list, order[2], "FAIL - out-of-order entry 3 'qnext' does not point to 'ready_list'");
  assert(thread_queue[tid2].qprev == tid1, order[2], "FAIL - out-of-order entry 3 'qprev' does not point to out-of-order entry 2");

  thread_dequeue(ready_list);
  assert(thread_queue[ready_list].qnext == tid1, order[3], "FAIL - 'ready_list.qnext' does not point to out-of-order entry 2");
  assert(thread_queue[ready_list].qprev == tid2, order[3], "FAIL - 'ready_list.qprev' does not point to out-of-order entry 3");
  assert(thread_queue[tid1].qnext == tid2, order[3], "FAIL - out-of-order entry 2 'qnext' does not point to out-of-order entry 3");
  assert(thread_queue[tid1].qprev == ready_list, order[3], "FAIL - out-of-order entry 2 'qprev' does not point to 'ready_list'");
  assert(thread_queue[tid2].qnext == ready_list, order[3], "FAIL - out-of-order entry 3 'qnext' does not point to 'ready_list'");
  assert(thread_queue[tid2].qprev == tid1, order[3], "FAIL - out-of-order entry 3 'qprev' does not point to out-of-order entry 2");

  thread_dequeue(ready_list);
  assert(thread_queue[ready_list].qnext == tid2, order[4], "FAIL - 'ready_list.qnext' does not point to out-of-order entry 3");
  assert(thread_queue[ready_list].qprev == tid2, order[4], "FAIL - 'ready_list.qprev' does not point to out-of-order entry 3");
  assert(thread_queue[tid2].qnext == ready_list, order[4], "FAIL - out-of-order entry 3 'qnext' does not point to 'ready_list'");
  assert(thread_queue[tid2].qprev == ready_list, order[4], "FAIL - out-of-order entry 3 'qprev' does not point to 'ready_list'");

  thread_dequeue(ready_list);
  assert(thread_queue[ready_list].qnext == ready_list, order[5], "FAIL - 'ready_list.qnext' does not point to 'ready_list'");
  assert(thread_queue[ready_list].qprev == ready_list, order[5], "FAIL - 'ready_list.qprev does not point to 'ready_list'");

  reset();
}

static void resched_tests(void)
{
  int32 tid;
  tid = create_thread(dummy_thread, "", 0);

  resume_thread(tid);
  assert(thread_queue[ready_list].qnext == tid, resched[0], "FAIL - 'ready_list.qnext' does not point to new thread");
  assert(thread_queue[ready_list].qprev == tid, resched[0], "FAIL - 'ready_list.qprev' does not point to new thread");

  suspend_thread(tid);
  assert(thread_queue[ready_list].qnext == ready_list, resched[1], "FAIL - 'ready_list.qnext' does not point to 'ready_list'");
  assert(thread_queue[ready_list].qprev == ready_list, resched[1], "FAIL - 'ready_list.qnext' does not point to 'ready_list'");

  reset();

  tid = create_thread(resched_thread, "", 0);
  __enable_resched = 1;
  resume_thread(tid);
  join_thread(tid);
}

void __ms4_runner(void)
{
  init_tests(general, test_count(general_prompt));
  init_tests(init, test_count(init_prompt));
  init_tests(enqueue, test_count(enqueue_prompt));
  init_tests(dequeue, test_count(dequeue_prompt));
  init_tests(order, test_count(order_prompt));
  init_tests(resched, test_count(resched_prompt));

  _psudoprint("\n");
  test_runner("general", test_count(general_prompt), general_tests);
  test_runner("initialization", test_count(init_prompt), initialize_tests);
  test_runner("enqueue", test_count(enqueue_prompt), enqueue_tests);
  test_runner("dequeue", test_count(dequeue_prompt), dequeue_tests);
  test_runner("order", test_count(order_prompt), order_tests);
  test_runner("resched", test_count(resched_prompt), resched_tests);
  _psudoprint("\n----------------------------\n");

  test_printer("\nGeneral Tests:", general, general_prompt, test_count(general_prompt));
  _psudoprint("\nQueue Tests:\n");
  test_printer("  Initialization:", init, init_prompt, test_count(init_prompt));
  test_printer("  Enqueue:", enqueue, enqueue_prompt, test_count(enqueue_prompt));
  test_printer("  Dequeue:", dequeue, dequeue_prompt, test_count(dequeue_prompt));
  test_printer("  Out of Order:", order, order_prompt, test_count(order_prompt));
  test_printer("\nScheduling Tests:", resched, resched_prompt, test_count(resched_prompt));
  _psudoprint("\n");
}