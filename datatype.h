
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
  T_LIST      = 0x0100,
  T_STRING    = 0x0200,
  T_NUMBER    = 0x0400,
  T_SYMBOL    = 0x0800,
  /* These arise at evaluation time. */
  T_PRIM_FUN  = 0x1001,
  T_PRIM_LET  = 0x1002,
  T_PRIM_NIL  = 0x1003,
  T_PRIM_OR   = 0x1004,
} DataType;

#define TYPE_CATEGORY_NUMBER  0x0400
#define TYPE_CATEGORY_PRIM    0x1000

#define TYPE_IS_ATOM(TYPE) ((TYPE) != T_LIST)
#define TYPE_IS_LIST(TYPE) ((TYPE) == T_LIST)
#define TYPE_IS_STRING(TYPE) ((TYPE) == T_STRING)
#define TYPE_IS_NUMBER(TYPE) ((TYPE) & TYPE_CATEGORY_NUMBER)
#define TYPE_IS_PRIM(TYPE) ((TYPE) & TYPE_CATEGORY_PRIM)

#define IS_ATOM(TERM) (TYPE_IS_ATOM((TERM)->type))
#define IS_LIST(TERM) (TYPE_IS_LIST((TERM)->type))
#define IS_STRING(TERM) (TYPE_IS_STRING((TERM)->type))
#define IS_NUMBER(TERM) (TYPE_IS_NUMBER((TERM)->type))
#define IS_PRIM(TERM) (TYPE_IS_PRIM((TERM)->type))

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

#define HEAD(TERM) (CHECK_TYPE(TERM, IS_LIST)->value.list.head)
#define TAIL(TERM) (CHECK_TYPE(TERM, IS_LIST)->value.list.tail)

// This isn't used yet.
typedef struct GCInfo {
  unsigned int bits;
} GCInfo;

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
  } value;
} Term;

