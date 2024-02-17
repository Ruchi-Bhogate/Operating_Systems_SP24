#include <barelib.h>
#include <thread.h>

extern char (*sys_putc_hook)(char);
extern char (*sys_getc_hook)(void);
extern void (*sys_syscall_hook)(void);
extern char (*oldhook_in)(void);
extern char (*oldhook_out)(char);
extern volatile uint32 *clint_timer_addr;
extern const uint32 timer_interval;
char putc_tester(char);
char getc_tester(void);
void syscall_tester(void);
void __ms4_runner(void);

interrupt __wrap_handle_clk(void)
{
  *clint_timer_addr += timer_interval;
}

byte __enable_resched = 0;
int32 __real_resched(void);
int32 __wrap_resched(void)
{
  if (__enable_resched)
  {
    return __real_resched();
  }
  else
  {
    if (current_thread != 0)
    {
      uint32 old = current_thread;
      if (thread_table[current_thread].state == TH_RUNNING)
        thread_table[current_thread].state = TH_READY;
      thread_table[0].state = TH_RUNNING;
      current_thread = 0;
      ctxsw(&(thread_table[0].stackptr), &(thread_table[old].stackptr));
    }
  }
  return 0;
}

byte __wrap_shell(char *arg)
{
#if MILESTONE == 4
  __ms4_runner();
#elif MILESTONE == 5
  __ms5_runner();
#elif MILESTONE == 6
  __ms6_runner();
#elif MILESTONE == 7
  __ms7_runner();
#elif MILESTONE == 8
  __ms8_runner();
#elif MILESTONE == 9
  __ms9_runner();
#elif MILESTONE == 10
  __ms10_runner();
#endif
  return 0;
}

void __real_initialize(void);
void __wrap_initialize(void)
{
  oldhook_out = sys_putc_hook;
  oldhook_in = sys_getc_hook;
  sys_putc_hook = putc_tester;
  sys_getc_hook = getc_tester;
  sys_syscall_hook = syscall_tester;
#if MILESTONE >= 4
  __real_initialize();
#endif
}