// this is the "user code" that calls things that are exposed from runtime, but also has access to same WASI, directly.

typedef struct {
  unsigned int r;
  unsigned int g;
  unsigned int b;
  unsigned int a;
} Color;

Color RED = (Color) {.r=255, .g=0, .b=0, .a=255};

// these are callbacks in the cart
#define CART(n) __attribute__((export_name(n)))

// these are host callbacks that can be called from inside cart
#define HOST(n) __attribute__((import_module("host"), import_name(n))) extern

HOST("clear_screen")
void clear_screen(Color color);

HOST("add")
int add(int a, int b);

CART("cart_update")
void update();

CART("cart_load")
void load();


// ---- user code

#include <stdio.h>

void load() {
  printf("hello from cart. 2+2=%d\n", add(2,2));
}

void update() {
  clear_screen(RED);
}

