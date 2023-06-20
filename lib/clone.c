#define _GNU_SOURCES

#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <linux/sched.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

const size_t STACK_SIZE = 1024 * 1024;
const int FLAGS = CLONE_VM | CLONE_FS | CLONE_NEWPID | CLONE_NEWIPC |
                  CLONE_NEWNET | CLONE_NEWUTS;

int
__make()
{
  static const value* __make_closure = NULL;
  if (NULL == __make_closure) {
    __make_closure = caml_named_value("__make");
  }

  return Int_val(caml_callback(*__make_closure, Val_int(0)));
}

value
__generate_child_process()
{
  CAMLparam0();

  void* stack = malloc(STACK_SIZE);

  if (NULL == stack) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  CAMLreturn(caml_copy_int32(clone(__make, stack + STACK_SIZE, FLAGS, NULL)));
}