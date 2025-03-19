#include "vm.h"

#include <assert.h>
#include <stdio.h>

void objectPrint(object* object) {
  switch (object->type) {
    case INT:
      printf("%d", object->val);
      break;

    case PAIR:
      printf("(");
      objectPrint(object->first);
      printf(", ");
      objectPrint(object->second);
      printf(")");
      break;
  }
}

void test1() {
  printf("Test 1: Objects on stack are preserved.\n");
  VM* vm = init_vm();
  push_obj(vm, INT, 1);
  push_obj(vm, INT, 2);

  garbage_collector(vm);
  assert(vm->obj_count == 2 && "Should have preserved objects.");
  free_gc(vm);
}

void test2() {
  printf("Test 2: Unreached objects are collected.\n");
  VM* vm = init_vm();
  push_obj(vm, INT, 1);
  push_obj(vm, INT, 2);
  pop_obj(vm);
  pop_obj(vm);

  garbage_collector(vm);
  assert(vm->obj_count == 0 && "Should have collected objects.");
  free_gc(vm);
}

void test3() {
  printf("Test 3: Reach nested objects.\n");
  VM* vm = init_vm();
  push_obj(vm, INT, 1);
  push_obj(vm, INT, 2);
  push_obj(vm, PAIR, 0);
  push_obj(vm, INT, 3);
  push_obj(vm, INT, 4);
  push_obj(vm, PAIR, 0);
  push_obj(vm, PAIR, 0);

  garbage_collector(vm);
  assert(vm->obj_count == 7 && "Should have reached objects.");
  free_gc(vm);
}

void test4() {
  printf("Test 4: Handle cycles.\n");
  VM* vm = init_vm();
  push_obj(vm, INT, 1);
  push_obj(vm, INT, 2);
  object* a = push_obj(vm, PAIR, 0);
  push_obj(vm, INT, 3);
  push_obj(vm, INT, 4);
  object* b = push_obj(vm, PAIR, 0);

  /* Set up a cycle, and also make 2 and 4 unreachable and collectible. */
  a->second = b;
  b->second = a;

  garbage_collector(vm);
  assert(vm->obj_count == 4 && "Should have collected objects.");
  free_gc(vm);
}

void perfTest() {
  printf("Performance Test.\n");
  VM* vm = init_vm();

  for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 20; j++) {
      push_obj(vm, INT, i);
    }

    for (int k = 0; k < 20; k++) {
      pop_obj(vm);
    }
  }
  free_gc(vm);
}

int main() {
  test1();
  test2();
  test3();
  test4();
  perfTest();
}
