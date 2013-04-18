/* Jeremy Bernstein
   CS481 Spring 2013
   Lab 3 - MMU
*/

#ifndef MEMSTRUCT
#define MEMSTRUCT

#include <time.h>
#include "dllist.c"
#include "jval.c"


// CONFIG
typedef struct {
  int pms; // physical-memory-size
  int frame; // frame-size
  int memLat; // memory-latency
  int rep; //page-replacement: 1-FIFO, 2-LRU, 3-LFU, 4-MFU, 5-RANDOM
  int tlbSz; //table-size;
  int tlbLat; //table-latency
  int diskLat; //disk-latency
  int log; //logging-output: 0-off, 1-on
} Config;

// TRACE
typedef struct{
  int pid; //is the process id of the relevant process
  int op; // 1-R, 2-W, 3-I
  unsigned int address; // the 32-bit hexadecimal address being referenced by the process. 
} Trace;

// TLB ENTRY
typedef struct{
  clock_t lastUsed;
  unsigned int* virtAddress;
  unsigned int physAddress;
} TLB_Entry;

// TLB
typedef struct{
  int size;
  int count;
  TLB_Entry* cache;
} TLB;

// PAGE TABLE ENTRY
typedef struct{
  unsigned long* virtAddress;
  int pid; // process ID of entry
  int lastUsed; // time of last use
  int useCount; // number of times used
} PageTableEntry;

// PAGE TABLE
typedef struct{
  int size;
  int count;
  PageTableEntry* table; // array of page table entries
} PageTable;

// PROCESS STATS
typedef struct{
  int pid;
  int memoryRef;
  int tlbMisses;
  int pageFaults;
  int cleanEvict;
  int dirtyEvict;
  float percentDirtyEvict;
} ProcessStats;

// SIM STATS
typedef struct{
  float overallLat;
  float avMemAccessLat;
  float slowdown;
  int memoryRef;
  int tlbMisses;
  int pageFaults;
  int cleanEvict;
  int dirtyEvict;
  float percentDirtyEvict;
} SimStats;

// MMUSIM
typedef struct{
  int pageBits;
  int offsetBits;
  int tlbSz;
  float tlbLat;
  int physMemSz;
  int physFrameSz;
  int numFrames;
  float memLat;
  int pageEntries;
  int rep; //page-replacement: 1-FIFO, 2-LRU, 3-LFU, 4-MFU, 5-RANDOM
  float diskLat;
  int log; // 1-on, 2-off

  Dllist processStats; // array of process stats
  SimStats simStats;

  TLB* tlb; // pointer to the TLB
  PageTable* pgtbl; // Page table
} MMUSim;


#endif /* MEMSTRUCT */
