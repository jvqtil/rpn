#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <readline/readline.h>

#define STACK_SIZE 128
float   stack[STACK_SIZE];
int     stack_i = 0;

void stack_push(float n) {
  assert(stack_i < STACK_SIZE);
  stack[stack_i] = n;
  stack_i++;
}

float stack_pop() {
  assert(stack_i > 0);
  stack_i--;
  return stack[stack_i];
}

void stack_print() {
  printf("stack: [");
  for (int i = 0; i < stack_i; i++) {
    printf(" %g", stack[i]);
  }
  printf(" ]\n");
}

#define OP_INVALID -1
#define OP_ADD 1
#define OP_SUB 2
#define OP_MUL 3
#define OP_DIV 4
#define OP_REM 5

int parse_op(char* str) {
  if (strcmp(str, "+") == 0) return OP_ADD;
  if (strcmp(str, "-") == 0) return OP_SUB;
  if (strcmp(str, "*") == 0) return OP_MUL;
  if (strcmp(str, "/") == 0) return OP_DIV;
  if (strcmp(str, "%") == 0) return OP_REM;
  return OP_INVALID;
}

void perform_op(int op) {
  float right = stack_pop();
  float left = stack_pop();
  float result = 0;

  switch (op) {
  case OP_ADD: result = left + right; break;
  case OP_SUB: result = left - right; break;
  case OP_MUL: result = left * right; break;
  case OP_DIV:
  case OP_REM:
    if (right == 0) {
      printf("can't divide by zero\n");
      stack_push(left);
      stack_push(right);
      return;
    }
    result = (op == OP_DIV) ? left / right : fmodf(left, right); break;
  default: assert(!"Invalid operation");
  }

  stack_push(result);
}

int parse_float(char* s, float* out) {
  char* end;
  float n = strtof(s, &end);
  if (end == s || *end != '\0') return 0;
  *out = n;
  return 1;
}

int main() {
  while (1) {
    stack_print();
    char* c = readline("> ");
    if (c == NULL) break;

    int op = parse_op(c);
    float n;
    
    if (op == OP_INVALID) {
      if (parse_float(c, &n)) {
	if (stack_i < STACK_SIZE) stack_push(n);
	else printf("stack full\n");
      }
    } else {
      if (stack_i >= 2) {
	perform_op(op);
      } else {
	printf("not enough operands\n");
      }
    }
    
    free(c);
  }
}
