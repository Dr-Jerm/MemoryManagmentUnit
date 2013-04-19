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

// PHYSICAL FRAME
typedef struct{
  int num;
  int pid;
  int lastUsed;
  int useCount;
} PhysicalFrame;

// PAGE TABLE ENTRY
typedef struct{
  unsigned long* virtAddress;
  unsigned int page; // page number
  int present; // 1-yes, 0-no
  int dirty; // 1-yes, 0-no
  int pid; // process ID of entry
  PhysicalFrame* physicalFrame; 
} PageTableEntry;

// TLB ENTRY
typedef struct{
  unsigned int lastUsed;
  int used;
  unsigned int* virtAddress;
  PageTableEntry* page;
} TLB_Entry;

// TLB
typedef struct{
  int size;
  int count;
  TLB_Entry* cache;
} TLB;

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

// MMUProcess
typedef struct{
  int pid;
  PageTable* pgtbl;
  ProcessStats stats;
} MMUProcess;


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

  Dllist processes; // array of process stats
  SimStats simStats;

  TLB* tlb; // pointer to the TLB
  PageTable* pgtbl; // Page table

  // Physical frame managment
  Dllist freePhysicalFrames;
  Dllist usedPhysicalFrames;
} MMUSim;


#endif /* MEMSTRUCT */
