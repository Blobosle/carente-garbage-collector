#include "vm.h"

#include <assert.h>
#include <stdlib.h>

VM *init_vm() {
  VM *vm = calloc(1, sizeof(VM));
  vm->gc_thresh = GC_THRESH;
  return vm;
}

void push(VM *vm, object *data) {
  assert(vm);
  assert(data);
  assert(vm->stack_size < STACK_MAX && "STACK OVERFLOW");

  if (vm->obj_count >= vm->gc_thresh) {
    garbage_collector(vm);
  }

  vm->stack[vm->stack_size++] = data;
}

object *pop_obj(VM *vm) {
  assert(vm);
  assert(vm->stack_size > 0 && "STACK UNDERFLOW");

  object *popped = vm->stack[--vm->stack_size];
  return popped;
}

object *push_obj(VM *vm, type_t type, int val) {
  assert(vm);

  object *new = calloc(1, sizeof(object));
  new->type = type;

  if (type == PAIR) {
    new->second = pop_obj(vm);
    new->first = pop_obj(vm);
  }
  else {
    new->val = val;
  }
  push(vm, new);

  new->next = vm->head;

  vm->head = new;
  vm->obj_count++;

  return new;
}

void mark(object *obj) {
  assert(obj);

  /* For handling cyclical references */
  if (obj->mark) {
    return;
  }

  obj->mark = 1;

  if (obj->type == PAIR) {
    mark(obj->first);
    mark(obj->second);
  }
}

void mark_all(VM *vm) {
  assert(vm);

  for (int i = 0; i < vm->stack_size; i++) {
    mark(vm->stack[i]);
  }
}

void sweep(VM *vm) {
  assert(vm);

  for (object **cur = &vm->head; *cur;) {
    if (!(*cur)->mark) {
      object *del = *cur;
      *cur = del->next;
      free(del);
      vm->obj_count--;
      continue;
    }

    (*cur)->mark = 0;
    cur = &(*cur)->next;
  }
}

void garbage_collector(VM *vm) {
  assert(vm);

  mark_all(vm);
  sweep(vm);

  vm->gc_thresh = vm->obj_count * 2;
}

void free_gc(VM *vm) {
  assert(vm);

  garbage_collector(vm);
  free(vm);
  vm = NULL;
}
