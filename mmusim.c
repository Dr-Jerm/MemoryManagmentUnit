/* Jeremy Bernstein
   CS481 Spring 2013
   Lab 3 - MMU
*/

#include <math.h>

#ifndef MMUSIM
#define MMUSIM
#include "memStruct.h"
#include <time.h>

void setupSim(Config* conf, MMUSim* sim){
  
  
  // Page Bits
  int physSize = conf->pms;
  int frameSize = conf->frame;
  long addressSpaceSize = pow(2,32);
  int pages = addressSpaceSize / frameSize;
  int pageBits = log10(pages)/log10(2);
  printf("Page bits: %d\n", pageBits);
  sim->pageBits = pageBits;

  // Page offset
  int offsetBits = 32 - pageBits;
  printf("Offset bits: %d\n", offsetBits);
  sim->offsetBits = offsetBits;

  // TLB Size
  int tlbSize = conf->tlbSz;
  printf("TLB size: %d\n", tlbSize);
  sim->tlbSz = tlbSize;

  // TLB Latency
  int tlbLatency = conf->tlbLat;
  float tlbLatInMs = tlbLatency * pow(10, -6);
  printf("TLB latency (milliseconds): %f\n", tlbLatInMs);
  sim->tlbLat = tlbLatInMs;

  // Physical Memory Size
  printf("Physical memory (bytes): %d\n", physSize);
  sim->physMemSz = physSize;

  // Physical Frame Size
  printf("Physical frame size (bytes): %d\n", frameSize);
  sim->physFrameSz = frameSize;

  // Number of Physical Frames
  int numFrames = ceil(((float) physSize) / ((float)frameSize));
  printf("Number of physical frames: %d\n", numFrames);
  sim->numFrames = numFrames;
 
  // Memory Latency
  int memLatency = conf->memLat;
  float memLatInMs = memLatency * pow(10, -6);
  printf("Memory latency (milliseconds): %f\n", memLatInMs);
  sim->memLat = memLatInMs;

  // Page Table Entries
  int pageTableEntries = pow(2, 32 - offsetBits);
  printf("Number of page table entries: %d\n", pageTableEntries);
  sim->pageEntries = pageTableEntries;

  // Page Table Replacement Strategy
  int rep = conf->rep;
  printf("Page replacement strategy: ");
  if(rep == 1) {printf("FIFO");}
  else if(rep == 2) {printf("LRU");}
  else if(rep == 3) {printf("LFU");}
  else if(rep == 4) {printf("MFU");}
  else if(rep == 5) {printf("RANDOM");}
  printf("\n");
  sim->rep = rep;

  // Disk latency
  int diskLat = conf->diskLat;
  printf("Disk latency (milliseconds): %d\n", diskLat);
  sim->diskLat = diskLat;

  // Logging
  int logging = conf->log;
  printf("Logging: ");
  if(logging == 0) {printf("off");}
  else if(logging == 1) {printf("on");}
  printf("\n\n");
  sim->log = logging;

  // Setup processStats
  sim->processStats = new_dllist();

  // Setup TLB and Page table
  TLB* tlb = malloc(sizeof(TLB));
  TLB_Entry* tlbArray = malloc(sim->tlbSz * sizeof(TLB_Entry));
  tlb->cache = tlbArray;
  tlb->size = sim->tlbSz;
  sim->tlb = tlb;
  
  
  PageTable* pageTable = malloc(sizeof(PageTable));
  PageTableEntry* pageArray = malloc(sim->pageEntries * sizeof(PageTableEntry));
  pageTable->table = pageArray;
  pageTable->size = sim->pageEntries;
  sim->pgtbl = pageTable;

  // Setup phyiscal frames
  sim->freePhysicalFrames = new_dllist();
  int frameCount = sim->numFrames;

  int i;
  for (i = 0; i < frameCount; i++){
    dll_append(sim->freePhysicalFrames, new_jval_i(i));
  }
}

ProcessStats* getProcessStats(MMUSim* sim, int* pid){
  Dllist pStats;
  pStats = sim->processStats;
  Dllist nil;
  nil = dll_nil(pStats);
  Dllist s;
  s = dll_first(pStats);

  while(s != nil){
    ProcessStats* ps;
    ps = s->val.v;

    if(ps->pid == pid){
      return ps;
    }
    s = s->flink;
  }

  ProcessStats* ps;
  ps = malloc(sizeof(ProcessStats));
  ps->pid = pid;
  dll_append(pStats, new_jval_v(ps));
  return ps;
}

void getPageNumOffset(MMUSim* sim, unsigned int addr, unsigned int* pageNum, unsigned int* pageOffset){
  int pageBits = sim->pageBits;
  int offsetBits = sim->offsetBits;

  unsigned int page = addr;
  page = page >> offsetBits;
  *pageNum = page;

  unsigned int offset = addr;
  offset = offset << pageBits;
  offset = offset >> pageBits;
  *pageOffset = offset;  
}

unsigned int checkTLB(MMUSim* sim, unsigned int addr){
  TLB* tlb;
  tlb = sim->tlb;
  TLB_Entry* cache = tlb->cache;

  int i;
  for( i = 0; i < tlb->size; i ++){
    TLB_Entry entry;
    entry = cache[i];
    if(entry.virtAddress == addr){
      clock_t curr;
      curr = clock();
      entry.lastUsed = curr;
      return 1;
    }
  }
  return 0; 
}

PageTableEntry* checkPageTable(MMUSim* sim, Trace* trace){
  PageTable* pageTable;
  pageTable = sim->pgtbl;
  PageTableEntry* table = pageTable->table;

  int offsetBits = sim->offsetBits;
  unsigned int pageIndex = trace->address;
  pageIndex = pageIndex >> offsetBits;
   
  return &table[pageIndex];
}

int findFreePhysicalPage(MMUSim* sim){
  Dllist freeFrames;
  freeFrames = sim->freePhysicalFrames;
  Dllist nil;
  nil = dll_nil(freeFrames);
  Dllist ffp;
  ffp = dll_first(freeFrames);

  if(ffp != nil){
    int freeFrame;
    freeFrame = ffp->val.i;
    dll_delete_node(ffp);
    return freeFrame;
  } else {
    return -1;
  }

}

PageTableEntry* findPtEvict(MMUSim* sim){
  PageTable* pageTable;
  pageTable = sim->pgtbl;
  PageTableEntry* table = pageTable->table;

  int rep = sim->rep;

  if(rep == 1){ // FIFO

  } else if(rep == 2){ // LRU

  } else if(rep == 3){ // LFU

  } else if(rep == 4){ // MFU

  } else{ // RANDOM
    int max = sim->pageEntries;
    int randomIndex = ((double) rand() / (((double)RAND_MAX)+1)) * (max+1);
    return &table[randomIndex];
  }
}

void runSim(MMUSim* sim, Dllist* traces){
  SimStats* simStats;
  simStats = &(sim->simStats);

  Dllist nil;
  nil = dll_nil(*traces);
  Dllist t;
  t  = dll_first(*traces);

  while(t != nil){
    Trace* trace;
    trace = t->val.v;

    ProcessStats* pStat;
    pStat = getProcessStats(sim, trace->pid);
    pStat->memoryRef = pStat->memoryRef + 1;
    simStats->memoryRef = simStats->memoryRef + 1;
    
    unsigned int pageNum;
    unsigned int pageOffset;
    getPageNumOffset(sim, trace->address, &pageNum, &pageOffset);
    int op = trace->op;

    if(sim->log){
      char* operation;
      if(op == 1) {operation = "Load from";}
      else if(op == 2) {operation = "Store to";}
      else if(op == 3) {operation = "Instruction fetch from";}
    
      fprintf(stderr, "Process[%u]: %s 0x%x (page: %u, offset: %u)\n",
        trace->pid, operation, trace->address, pageNum, pageOffset);
    }

    unsigned int tlbHit;
    tlbHit = checkTLB(sim, trace->address);
    if(tlbHit){
      if(sim->log) {fprintf(stderr, "\tTLB hit? yes\n");}
      simStats->overallLat = simStats->overallLat + sim->memLat;
      // if write, mark dirty
      // return frame number
      
    }else{
      if(sim->log) {fprintf(stderr, "\tTLB hit? no\n");}
      pStat->tlbMisses = pStat->tlbMisses + 1;
      simStats->tlbMisses = simStats->tlbMisses + 1;
      
      PageTableEntry* pte;
      pte = checkPageTable(sim, trace);

      if(pte->present && trace->pid == pte->pid){
        if(sim->log) {fprintf(stderr, "\tPage fault? no\n");}
        // do tlb load
        // retry
      } else {
        if(sim->log) {fprintf(stderr, "\tPage fault? yes\n");}
        pStat->pageFaults = pStat->pageFaults + 1;
        simStats->pageFaults = simStats->pageFaults + 1;

        // Is there room?
        int pfn;
        pfn = findFreePhysicalPage(sim);
//printf("found free frame at: %d\n",pfn);
        if(pfn != -1){
          if(sim->log) {fprintf(stderr, "\tMain memory eviction? no\n");}
          simStats->overallLat = simStats->overallLat + sim->diskLat;
          pte->physFrameNum = pfn;
          pte->present = 1;
          
          // update PTE (present bit = 1) + memory access
          // update tlb
          // retry
        } else {
          if(sim->log) {fprintf(stderr, "\tMain memory eviction? yes\n");}

          PageTableEntry* evPte; // evicted PTE
          evPte = findPtEvict(sim);

          // clean eviction?
          if(pte->dirty){
            pStat->dirtyEvict = pStat->dirtyEvict + 1;
            simStats->dirtyEvict = simStats->dirtyEvict + 1;
            // wite to disk + disk access
            
          } else {
            pStat->cleanEvict = pStat->cleanEvict + 1;
            simStats->cleanEvict = simStats->cleanEvict + 1;
    
          }
          // update old PTE (set present bit to 0)
          // load into memory
          // update new PTE (set present bit to 1)
          // update tlb
          // retry
        }
      }
    }
    
    if(sim->log) {fprintf(stderr, "\n");}
    t = t->flink;
  }
  
}

void endSim(MMUSim* sim){
  SimStats simStats;
  simStats = sim->simStats;
  printf("\n"); 
  printf("Overall latency (milliseconds): %f.\n", simStats.overallLat);
  printf("Average memory access latency (milliseconds/reference): %f.\n", simStats.avMemAccessLat);
  printf("Slowdown: %f.\n", simStats.slowdown);

  printf("\n");
  
  printf("Overall\n");
  printf("\tMemory references: %d\n", simStats.memoryRef);
  printf("\tTLB misses: %d\n", simStats.tlbMisses);
  printf("\tPage faults: %d\n", simStats.pageFaults);
  printf("\tClean evictions: %d\n", simStats.cleanEvict);
  printf("\tDirty evictions: %d\n", simStats.dirtyEvict);
  
  float percentDirty = 0.0;
  if(simStats.cleanEvict > 0 || simStats.dirtyEvict > 0){
    percentDirty = simStats.dirtyEvict / (simStats.cleanEvict + simStats.dirtyEvict);
  }
  printf("\tPercentage dirty evictions: %3.2f%\n", percentDirty*100);
}


#endif /* MMUSIM */
