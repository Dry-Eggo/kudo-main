

#pragma once

#define NEW_LIST(T)                                                  \
  typedef struct list_##T {                                                 \
    T **data;                                                                  \
    size_t cap;                                                                \
    size_t count;                                                              \
  } list_##T;                                                               \
  static inline int list_##T##_grow(list_##T *);                         \
  static inline int list_##T##_init(list_##T **l) {                      \
    *l = (list_##T *)arena_alloc(main_arena, sizeof(list_##T *));                                  \
    if (!l)                                                                    \
      return -1;                                                               \
    (*l)->data = (T **)arena_alloc(main_arena, 64 * sizeof(T *));                               \
    (*l)->cap = 64;                                                            \
    (*l)->count = 0;                                                           \
    return 0;                                                                  \
  }                                                                            \
  static inline int list_##T##_grow(list_##T *l) {                       \
    size_t ns = l->cap * 2;                                                    \
    l->data = (T **)arena_realloc(main_arena, l->data, l->cap, sizeof(T *) * ns);                                      \
    l->cap = ns;                                                               \
    return 0;                                                                  \
  }                                                                            \
  static inline int list_##T##_add(list_##T *l, T *t) {                  \
    if (t == NULL)                                                             \
      return -1;                                                               \
    if (l->count >= l->cap)                                                    \
      list_##T##_grow(l);                                                   \
    l->data[l->count++] = (t);                                                 \
    return 0;                                                                  \
  }                                                                            \
  static inline T *list_##T##_get(list_##T *l, size_t i) {               \
    if (i >= l->count)                                                         \
      return NULL;                                                             \
    return l->data[i];                                                         \
  }
