
#include "datatype.h"

typedef struct MemPoolCell {
  struct MemPoolCell* prevCell;
  Term firstObject[1];
} MemPoolCell;

typedef struct MemPool {
  struct MemPoolCell* currentCell;
  Term* nextAlloc;
  unsigned freeSpace;
} MemPool;

typedef struct MemPoolAllocUnit {
  MemPool pool;
  MemPoolCell cell;
} MemPoolAllocUnit;

#define MEMPOOL_BLOCK_SIZE

// FIXME: Don't use malloc/realloc at all.

void* Alloc(size_t size) {
  void* p = malloc(size);
  if (!p) {
    fprintf(stderr, "Out of memory.\n");
    exit(1);
  }
  return p;
}

void* Realloc(void* p, size_t size) {
  p = realloc(p, size);
  if (!p) {
    fprintf(stderr, "Out of memory.\n");
    exit(1);
  }
  return p;
}

Env* EnvBind(MemPool* pool, Env* env, Term* argNameSymbol, Term* value) {
  Env* newEnv = (Env*)Alloc(sizeof(Env));
  newEnv->next = env;
  newEnv->nameText = argNameSymbol->value.string.text;
  newEnv->nameLen = argNameSymbol->value.string.len;
  newEnv->value = value;
  return newEnv;
}

Term* NewCons(MemPool* pool, Term* head, Term* tail) {
  Term* newNode = (Term*)Alloc(sizeof(Term));
  newNode->type = T_CONS;
  HEAD(newNode) = head;
  TAIL(newNode) = tail;
  return newNode;
}

Term* NewAtom(MemPool* pool, DataType type) {
  assert(TYPE_IS_ATOM(type));
  assert(!TYPE_IS_NIL(type));
  Term* newAtom = (Term*)Alloc(sizeof(Term));
  newAtom->type = type;
  return newAtom;
}

PageSize pageSize;
void* memBase;
size_t memSize;

static PageSize GetPageSize() {
#ifdef _WIN32
  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);
  return sysInfo.dwPageSize;
#else
  return sysconf(_SC_PAGESIZE);
#endif
}

static void* ReserveMemory(size_t size) {
#ifdef _WIN32
  void* mem = VirtualAlloc(0, size, MEM_RESERVE, 0);
  int failure = (mem == 0);
#else
  void* mem = mmap(0, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  int failure = (mem == MAP_FAILED);
#endif
  if (failure) {
    fprintf(stderr, "Failed to reserve memory.\n");
    exit(1);
  }
  return mem;
}

void MemInit() {
  pageSize = GetPageSize();
  // memSize = 0x40000000; // 1 GB
  memSize = 0x1000000; // 16 MB
  memBase = ReserveMemory(memSize);
}

void* AllocPage() {
  void* mem;
  int failure;
#ifdef _WIN32
  mem = VirtualAlloc(0, pageSize, MEM_COMMIT | MEM_RESERVE, 0);
  failure = (mem == 0);
#else
  mem = mmap(0, pageSize, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  failure = (mem == MAP_FAILED);
#endif
  if (failure) {
    fprintf(stderr, "Out of memory.\n");
    exit(1);
  }
  return mem;
}

MemPool* NewMemPool() {
  MemPoolAllocUnit* allocUnit = AllocPage();
  MemPool* pool = &allocUnit->pool;
  pool->currentCell = &allocUnit->cell;
  pool->currentCell->prevCell = 0;
  Term* firstObject = allocUnit->cell.firstObject;
  pool->nextAlloc = firstObject;
  pool->freeSpace =
    ((void*)allocUnit + pageSize - (void*)firstObject)
      / sizeof(Term);
  return pool;
}

// TODO: Support different object sizes?
Term* NewTermFromMemPool(MemPool* pool) {
  if (pool->freeSpace == 0) {
    MemPoolCell* newCell = AllocPage();
    newCell->prevCell = pool->currentCell;
    pool->nextAlloc = newCell->firstObject;
    pool->freeSpace =
      ((void*)newCell + pageSize - (void*)pool->nextAlloc)
        / sizeof(Term);
    pool->currentCell = newCell;
  }
  Term* newTerm = pool->nextAlloc;
  pool->nextAlloc++;
  pool->freeSpace--;
  return newTerm;
}

void FreeMemPool(MemPool* pool) {
  MemPoolCell* cell = pool->currentCell;
  while (cell) {
    MemPoolCell* prevCell = cell->prevCell;
#ifdef _WIN32
    // TODO: Check unmap failure.
    VirtualFree(cell, pageSize, MEM_DECOMMIT);
#else
    munmap(cell, pageSize);
#endif
    cell = prevCell;
  }
}

