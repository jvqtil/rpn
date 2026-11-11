#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <readline/readline.h>

#define MAX 128

float stk[MAX];
int top = 0;

void push(float val) {
  assert(top < MAX);
  stk[top++] = val;
}

float pop(void) {
  assert(top > 0);
  return stk[--top];
}

void prn(void) {
  printf("stack: [");
  for (int idx = 0; idx < top; idx++) {
    printf(" %g", stk[idx]);
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

int p_op(const char* str) {
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
  if (op == CLR) { top = 0; return; }
  if (op == DRP) { pop(); return; }
  if (op == DUP) {
    if (top < MAX) push(stk[top - 1]);
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
    if (top < MAX) push(op == PI ? (float)M_PI : (float)M_E);
    else printf("stack full\n");
    return;
  }

  int cnt = req(op);
  float rhs = (cnt == 2) ? pop() : 0.0f;
  float lhs = pop();
  float res = 0.0f;

  switch (op) {
    case ADD: res = lhs + rhs; break;
    case SUB: res = lhs - rhs; break;
    case MUL: res = lhs * rhs; break;
    case DIV:
    case REM:
      if (rhs == 0.0f) {
        printf("can't divide by zero\n");
        push(lhs);
        push(rhs);
        return;
      }
      res = (op == DIV) ? (lhs / rhs) : fmodf(lhs, rhs);
      break;
    case POW: res = powf(lhs, rhs); break;
    case HYP: res = hypotf(lhs, rhs); break;
    case AT2: res = atan2f(lhs, rhs); break;

    case SQT: res = sqrtf(lhs); break;
    case CBT: res = cbrtf(lhs); break;
    case ABS: res = fabsf(lhs); break;
    case NEG: res = -lhs; break;
    case SIN: res = sinf(lhs); break;
    case COS: res = cosf(lhs); break;
    case TAN: res = tanf(lhs); break;
    case ASN: res = asinf(lhs); break;
    case ACS: res = acosf(lhs); break;
    case ATN: res = atanf(lhs); break;
    case SNH: res = sinhf(lhs); break;
    case CSH: res = coshf(lhs); break;
    case TNH: res = tanhf(lhs); break;
    case EXP: res = expf(lhs); break;
    case LOG: res = logf(lhs); break;
    case L10: res = log10f(lhs); break;
    case LG2: res = log2f(lhs); break;
    case CEL: res = ceilf(lhs); break;
    case FLR: res = floorf(lhs); break;
    case RND: res = roundf(lhs); break;
    default: assert(!"Invalid operation");
  }

  if (isnan(res) || isinf(res)) {
    printf("math error\n");
    push(lhs);
    if (cnt == 2) push(rhs);
    return;
  }

  push(res);
}

int p_flt(const char* str, float* out) {
  char* end;
  float val = strtof(str, &end);
  if (end == str || *end != '\0') return 0;
  *out = val;
  return 1;
}

int main(void) {
  while (1) {
    prn();
    char* inp = readline("> ");
    if (!inp) break;

    int op = p_op(inp);
    float num;

    if (op == INV) {
      if (p_flt(inp, &num)) {
        if (top < MAX) push(num);
        else printf("stack full\n");
      }
    } else {
      if (top >= req(op)) {
        run(op);
      } else {
        printf("not enough operands\n");
      }
    }

    free(inp);
  }
  return 0;
}
