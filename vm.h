#ifndef VM_H
#define VM_H

#define STACK_MAX (2000)
#define GC_THRESH (50)

/* Possible type categories */
typedef enum {
  INT,
  PAIR,
} type_t;

/* Basic object in toy interpreter, pair or int */
typedef struct object {
  struct object *next;

  union {
    int val;

    struct {
      struct object *first;
      struct object *second;
    };
  };

  type_t type;
  unsigned char mark;
} object;

/* Simple virtual machine implementation */
typedef struct {
  object *stack[STACK_MAX];
  object *head;
  int obj_count;
  int gc_thresh;
  int stack_size;
} VM;

void garbage_collector(VM *);
VM *init_vm();
object *push_obj(VM *, type_t, int);
object *pop_obj(VM *);
void free_gc(VM *);

#endif // VM_H
