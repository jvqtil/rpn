#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <readline/readline.h>

#define STACK_SIZE 128

float stack[STACK_SIZE];
int stack_top = 0;

void push(float val) {
  assert(stack_top < STACK_SIZE);
  stack[stack_top++] = val;
}

float pop(void) {
  assert(stack_top > 0);
  return stack[--stack_top];
}

void print_stack(void) {
  printf("stack: [");
  for (int i = 0; i < stack_top; i++) {
    printf(" %g", stack[i]);
  }
  printf(" ]\n");
}

enum {
  INV = -1,
  ADD, SUB, MUL, DIV, REM, POW, HYP, AT2,
  SQT, CBT, ABS, NEG,
  SIN, COS, TAN, ASN, ACS, ATN,
  SNH, CSH, TNH,
  EXP, LOG, L10, LG2,
  CEL, FLR, RND,
  PI, EUL, CLR, DRP, DUP, SWP
};

int parse_op(const char* str) {
  if (!strcmp(str, "+"))     return ADD;
  if (!strcmp(str, "-"))     return SUB;
  if (!strcmp(str, "*"))     return MUL;
  if (!strcmp(str, "/"))     return DIV;
  if (!strcmp(str, "%"))     return REM;
  if (!strcmp(str, "^") || !strcmp(str, "pow"))   return POW;
  if (!strcmp(str, "hypot")) return HYP;
  if (!strcmp(str, "atan2")) return AT2;

  if (!strcmp(str, "sqrt"))  return SQT;
  if (!strcmp(str, "cbrt"))  return CBT;
  if (!strcmp(str, "abs"))   return ABS;
  if (!strcmp(str, "neg"))   return NEG;
  if (!strcmp(str, "sin"))   return SIN;
  if (!strcmp(str, "cos"))   return COS;
  if (!strcmp(str, "tan"))   return TAN;
  if (!strcmp(str, "asin"))  return ASN;
  if (!strcmp(str, "acos"))  return ACS;
  if (!strcmp(str, "atan"))  return ATN;
  if (!strcmp(str, "sinh"))  return SNH;
  if (!strcmp(str, "cosh"))  return CSH;
  if (!strcmp(str, "tanh"))  return TNH;
  if (!strcmp(str, "exp"))   return EXP;
  if (!strcmp(str, "ln") || !strcmp(str, "log"))    return LOG;
  if (!strcmp(str, "log10")) return L10;
  if (!strcmp(str, "log2"))  return LG2;
  if (!strcmp(str, "ceil"))  return CEL;
  if (!strcmp(str, "floor")) return FLR;
  if (!strcmp(str, "round")) return RND;

  if (!strcmp(str, "pi"))    return PI;
  if (!strcmp(str, "e"))     return EUL;
  if (!strcmp(str, "clear")) return CLR;
  if (!strcmp(str, "drop"))  return DRP;
  if (!strcmp(str, "dup"))   return DUP;
  if (!strcmp(str, "swap"))  return SWP;

  return INV;
}

int req(int op) {
  if (op >= ADD && op <= AT2) return 2;
  if (op == SWP) return 2;
  if ((op >= SQT && op <= RND) || op == DRP || op == DUP) return 1;
  return 0;
}

void run(int op) {
  if (op == CLR) { stack_top = 0; return; }
  if (op == DRP) { pop(); return; }
  if (op == DUP) {
    if (stack_top < STACK_SIZE) push(stack[stack_top - 1]);
    else printf("stack full\n");
    return;
  }
  if (op == SWP) {
    float v1 = pop();
    float v2 = pop();
    push(v1);
    push(v2);
    return;
  }
  if (op == PI || op == EUL) {
    if (stack_top < STACK_SIZE) push(op == PI ? (float)M_PI : (float)M_E);
    else printf("stack full\n");
    return;
  }

  int count = req(op);
  float right = (count == 2) ? pop() : 0.0f;
  float left = pop();
  float result = 0.0f;

  switch (op) {
    case ADD: result = left + right; break;
    case SUB: result = left - right; break;
    case MUL: result = left * right; break;
    case DIV:
    case REM:
      if (right == 0.0f) {
        printf("can't divide by zero\n");
        push(left);
        push(right);
        return;
      }
      result = (op == DIV) ? (left / right) : fmodf(left, right);
      break;
    case POW: result = powf(left, right); break;
    case HYP: result = hypotf(left, right); break;
    case AT2: result = atan2f(left, right); break;

    case SQT: result = sqrtf(left); break;
    case CBT: result = cbrtf(left); break;
    case ABS: result = fabsf(left); break;
    case NEG: result = -left; break;
    case SIN: result = sinf(left); break;
    case COS: result = cosf(left); break;
    case TAN: result = tanf(left); break;
    case ASN: result = asinf(left); break;
    case ACS: result = acosf(left); break;
    case ATN: result = atanf(left); break;
    case SNH: result = sinhf(left); break;
    case CSH: result = coshf(left); break;
    case TNH: result = tanhf(left); break;
    case EXP: result = expf(left); break;
    case LOG: result = logf(left); break;
    case L10: result = log10f(left); break;
    case LG2: result = log2f(left); break;
    case CEL: result = ceilf(left); break;
    case FLR: result = floorf(left); break;
    case RND: result = roundf(left); break;
    default: assert(!"Invalid operation");
  }

  if (isnan(result) || isinf(result)) {
    printf("math error\n");
    push(left);
    if (count == 2) push(right);
    return;
  }

  push(result);
}

int parse_float(const char* str, float* out) {
  char* end;
  float val = strtof(str, &end);
  if (end == str || *end != '\0') return 0;
  *out = val;
  return 1;
}

int main(void) {
  while (1) {
    print_stack();
    char* input = readline("> ");
    if (!input) break;

    int op = parse_op(input);
    float num;

    if (op == INV) {
      if (parse_float(input, &num)) {
        if (stack_top < STACK_SIZE) push(num);
        else printf("stack full\n");
      }
    } else {
      if (stack_top >= req(op)) {
        run(op);
      } else {
        printf("not enough operands\n");
      }
    }

    free(input);
  }
  return 0;
}
