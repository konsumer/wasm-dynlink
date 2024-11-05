// this contains helper wrappers around WASI file access. I this case, drawing utils for working with a WASI framebuffer.

#include <stdio.h>
#include <stdlib.h>

// these are callbacks in the cart
#define CART(n) __attribute__((import_module("cart"), import_name(n))) extern

// these are host callbacks that the wasm host or cart will call
#define HOST(n) __attribute__((export_name(n)))

typedef struct {
  unsigned int r;
  unsigned int g;
  unsigned int b;
  unsigned int a;
} Color;

HOST("load")
void load() {
  printf("hello from host.\n");
}

HOST("update")
void update() {
}

HOST("clear_screen")
void clear_screen(Color color) {
  printf("Host clear_screen called with (%u, %u, %u, %u)\n", color.r, color.g, color.b, color.a);
}

// this shows that passing ints works
HOST("add")
int add(int a, int b) {
  return a+b;
}