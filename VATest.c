
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <windows.h>

DWORD pageSize;

DWORD GetPageSize() {
  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);
  return sysInfo.dwPageSize;
}

void MemInit() {
  pageSize = GetPageSize();
}

void* AllocPage() {
  void* mem;
  int failure;
  mem = VirtualAlloc(NULL, pageSize, MEM_COMMIT | MEM_RESERVE, 0);
  failure = (mem == 0);
  if (failure) {
    fprintf(stderr, "Out of memory.\n");
    exit(1);
  }
  return mem;
}

void PrintLastError(FILE* f) {
  TCHAR errorMessage[1024];
  DWORD msgSize = FormatMessage(
      FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
      LANG_NEUTRAL, errorMessage, 1024, NULL);
  if (!msgSize || errorMessage == NULL) {
    fprintf(f, "Unable to get Windows error message.\n");
  } else {
    fprintf(f, errorMessage);
  }
}

int main(int argc, char** argv) {
  MemInit();
  printf("Page size: 0x%X (%d)\n", pageSize, pageSize);
  size_t reserveSize = 0x100000; // 1 MB
  reserveSize = 0x100000 << 0x0A; // 1 GB
  printf("Reserve size: 0x%IX (%Id)\n", reserveSize, reserveSize);
  void* mem = VirtualAlloc(NULL, reserveSize, MEM_RESERVE, PAGE_READWRITE);
  printf("Reserve address: 0x%X\n", mem);
  if (mem == NULL) {
    PrintLastError(stdout);
  }
  for (int i=0; i<8; i++) {
    void* mem2 = VirtualAlloc(
        NULL, reserveSize, MEM_RESERVE, PAGE_READWRITE);
    printf("Reserve address: 0x%X\n", mem2);
    if (mem2 == NULL) {
      PrintLastError(stdout);
    }
  }
  void* com = VirtualAlloc(mem, pageSize, MEM_COMMIT, PAGE_READWRITE);
  printf("Commit address: 0x%X\n", com);
  if (com == NULL) {
    PrintLastError(stdout);
  }
}

