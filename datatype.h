
/**
The data type of this term.

The integer representation of the type is subdivided
into an upper byte (MSB) which signifies the general
type category, and a lower byte (LSB) which signifies
a specific type within that category. (This allows
for easy bit tests to answer questions like "is this
a primitive" or "is this a number".)

(NOTE: Currently there's only one kind of number.
       There will be a numeric tower in the future.)
*/
typedef enum {
  /* These are produced by the parser. */
  T_CONS        = 0x0100,
  T_STRING      = 0x0200,
  T_NUMBER      = 0x0400,
  T_SYMBOL      = 0x0800,
  /* These arise at evaluation time. */
  T_PRIM_NIL    = 0x1001, /* NOTE: Nil is a null pointer but it has a type. */
  T_PRIM_FUN    = 0x1002,
  T_PRIM_QUOTE  = 0x1003,
  T_PRIM_BEGIN  = 0x1004,
  T_FUN_NATIVE  = 0x2001,
  T_FUN_USER    = 0x2002,
  T_FUN_MACRO   = 0x2003,
} DataType;

#define TYPE_CATEGORY_NUMBER  0x0400
#define TYPE_CATEGORY_PRIM    0x1000
#define TYPE_CATEGORY_FUN     0x2000

#define TYPE_IS_NIL(TYPE) ((TYPE) == T_PRIM_NIL)
#define TYPE_IS_ATOM(TYPE) ((TYPE) != T_CONS)
#define TYPE_IS_CONS(TYPE) ((TYPE) == T_CONS)
#define TYPE_IS_STRING(TYPE) ((TYPE) == T_STRING)
#define TYPE_IS_SYMBOL(TYPE) ((TYPE) == T_SYMBOL)
#define TYPE_IS_NUMBER(TYPE) ((TYPE) & TYPE_CATEGORY_NUMBER)
#define TYPE_IS_PRIM(TYPE) ((TYPE) & TYPE_CATEGORY_PRIM)
#define TYPE_IS_FUN(TYPE) ((TYPE) & TYPE_CATEGORY_FUN)
#define TYPE_IS_FUN_NATIVE(TYPE) ((TYPE) == T_FUN_NATIVE)
#define TYPE_IS_FUN_USER(TYPE) ((TYPE) == T_FUN_USER)
#define TYPE_IS_FUN_MACRO(TYPE) ((TYPE) == T_FUN_USER)

#define IS_NIL(TERM)        (!(TERM))
#define IS_ATOM(TERM)       (!(TERM) || TYPE_IS_ATOM((TERM)->type))
#define IS_CONS(TERM)       ((TERM) && TYPE_IS_CONS((TERM)->type))
#define IS_LIST(TERM)       (!(TERM) || TYPE_IS_CONS((TERM)->type))
#define IS_STRING(TERM)     ((TERM) && TYPE_IS_STRING((TERM)->type))
#define IS_SYMBOL(TERM)     ((TERM) && TYPE_IS_SYMBOL((TERM)->type))
#define IS_NUMBER(TERM)     ((TERM) && TYPE_IS_NUMBER((TERM)->type))
#define IS_PRIM(TERM)       ((TERM) && TYPE_IS_PRIM((TERM)->type))
#define IS_FUN(TERM)        ((TERM) && TYPE_IS_FUN((TERM)->type))
#define IS_FUN_NATIVE(TERM) ((TERM) && TYPE_IS_FUN_NATIVE((TERM)->type))
#define IS_FUN_USER(TERM)   ((TERM) && TYPE_IS_FUN_USER((TERM)->type))
#define IS_FUN_MACRO(TERM)  ((TERM) && TYPE_IS_FUN_MACRO((TERM)->type))

/* Use this check around a pointer to ensure that the term it
   points to has the type that you expect. It returns null if
   the term has the wrong type, otherwise it returns the term.

   This check will force a segfault whenever you try to access
   a term using the wrong type accessor. A segfault is an ugly
   way to deal with this, but it's easy to trigger (with a null
   dereference) and better than allowing control to continue
   unchecked. */
#define CHECK_TYPE(TERM, TYPE_PREDICATE) \
  (TYPE_PREDICATE(TERM) ? TERM : (Term*)0)

#define HEAD(TERM) (CHECK_TYPE(TERM, IS_CONS)->value.list.head)
#define TAIL(TERM) (CHECK_TYPE(TERM, IS_CONS)->value.list.tail)

// This isn't used yet.
typedef struct GCInfo {
  unsigned int bits;
} GCInfo;

struct Env;

typedef struct Term {
  DataType type;
  GCInfo gcInfo;
  union {
    struct {
      struct Term* head;
      struct Term* tail;
    } list;
    struct {
      const char* text;
      int len;
    } string;
    struct {
      int n;
    } number;
    struct {
      const char* funName;  /* Function name (null-terminated string). */
      struct Term* (*funPtr)(struct Term*);
    } bif;
    struct {
      //struct Term* funName; /* Function name (a symbol). */
      struct Term* funBody; /* Function code (a list). */
      struct Term* funArgs; /* List of symbols (arg names). */
      struct Env* funEnv;   /* Closure environment. */
    } udf;
  } value;
} Term;

typedef struct Env {
  struct Env* next;
  const char* nameText;
  int nameLen;
  Term* value;
} Env;

Env* BuiltinEnvironment();
Term* GetSymbol(const char* name);
Term* Interpret(Term* iTerm);
Term* EnvLookup(Env* env, const char* name, int len);

#define ENV_LOOKUP_FAILED ((Term*)4)

void Die(const char* message, ...)
  __attribute__((noreturn));
void DieShowingTerm(const char* message, Term* term, ...)
  __attribute__((noreturn));

void PrintTerm(FILE* f, Term* atom);

typedef struct MemPool MemPool;
Env* EnvBind(MemPool* pool, Env* env, Term* argNameSymbol, Term* value);
Term* NewCons(MemPool* pool, Term* head, Term* tail);
Term* NewAtom(MemPool* pool, DataType type);

#ifdef _WIN32
typedef DWORD PageSize;
#else
typedef long PageSize;
#endif

extern PageSize pageSize;

