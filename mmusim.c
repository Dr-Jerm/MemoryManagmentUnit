/* Jeremy Bernstein
   CS481 Spring 2013
   Lab 3 - MMU
*/

#include <math.h>

#ifndef MMUSIM
#define MMUSIM
#include "memStruct.h"
#include <time.h>
#include <limits.h>

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

  // Setup process list
  sim->processes = new_dllist();

  // Setup TLB and Page table

  TLB* tlb = malloc(sizeof(TLB));
  TLB_Entry* tlbArray = malloc(sim->tlbSz * sizeof(TLB_Entry));
  tlb->cache = tlbArray;
  tlb->size = sim->tlbSz;
  sim->tlb = tlb;
  
/*  
  PageTable* pageTable = malloc(sizeof(PageTable));
  PageTableEntry* pageArray = malloc(sim->pageEntries * sizeof(PageTableEntry));
  pageTable->table = pageArray;
  pageTable->size = sim->pageEntries;
  sim->pgtbl = pageTable;
*/
  // Setup phyiscal frames
  sim->usedPhysicalFrames = new_dllist();
  sim->freePhysicalFrames = new_dllist();
  int frameCount = sim->numFrames;

  int i;
  for (i = 0; i < frameCount; i++){
    PhysicalFrame* pf = malloc(sizeof(PhysicalFrame));
    pf->num = i;
    dll_append(sim->freePhysicalFrames, new_jval_v(pf));
  }
}

MMUProcess* getProcess(MMUSim* sim, int* pid){
  Dllist processList;
  processList = sim->processes;
  Dllist nil;
  nil = dll_nil(processList);
  Dllist s;
  s = dll_first(processList);

  while(s != nil){
    MMUProcess* proc;
    proc = s->val.v;

    if(proc->pid == pid){
      return proc;
    }
    s = s->flink;
  }

  MMUProcess* ps;
  ps = malloc(sizeof(MMUProcess));
  ps->pid = pid;
  dll_append(processList, new_jval_v(ps));
  PageTable* pageTable = malloc(sizeof(PageTable));
  PageTableEntry* pageArray = malloc(sim->pageEntries * sizeof(PageTableEntry));
  pageTable->table = pageArray;
  pageTable->size = sim->pageEntries;
  ps->pgtbl = pageTable;

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

PhysicalFrame* checkTLB(MMUSim* sim, unsigned int addr){
  TLB* tlb;
  tlb = sim->tlb;
  TLB_Entry* cache = tlb->cache;

  int i;
  for( i = 0; i < tlb->size; i++){
    TLB_Entry* entry;
    entry = &cache[i];
    if(entry->virtAddress == addr && entry->used == 1){
      clock_t curr;
      curr = clock();
      entry->lastUsed = curr;
      PhysicalFrame* frame;
      frame = entry->page->physicalFrame;
      frame->lastUsed = curr;
      return frame;
    }
  }
  return -1; 
}

int addPageToTlb(MMUSim* sim, unsigned int addr, PageTableEntry* pte){
  TLB* tlb;
  tlb = sim->tlb;
  
  TLB_Entry* cache = tlb->cache;

  unsigned int oldest = UINT_MAX;
  int oldestIndex;
  int count = 0;

  int i;
  for( i = 0; i < tlb->size; i++){
    TLB_Entry* entry;
    entry = &cache[i];
    if(entry->used){
      if(entry->lastUsed < oldest){
        oldest = entry->lastUsed;
        oldestIndex = i;
      }
      count++;
    } else{
      if(sim->log){ fprintf(stderr, "\tTLB eviction? no\n" );}

      entry->used = 1;
      entry->page = pte;
      entry->virtAddress = addr;
      entry->lastUsed = clock();
      
      return 1;
    }
  }
  if(count >= sim->tlbSz){
    if(sim->log){ fprintf(stderr, "\tTLB eviction? yes\n" );}
    TLB_Entry* evicted;
    evicted = &cache[oldestIndex];

    if(sim->log){ fprintf(stderr, "\tpage %d evicted from TLB\n",evicted->page->page );}

  }

  return -1; 

}

PageTableEntry* checkPageTable(MMUSim* sim, PageTable* pageTable, Trace* trace){
  PageTableEntry* table = pageTable->table;

  int offsetBits = sim->offsetBits;
  unsigned int pageIndex = trace->address;
  pageIndex = pageIndex >> offsetBits;
   
  return &table[pageIndex];
}

PhysicalFrame* findFreePhysicalPage(MMUSim* sim){
  Dllist freeFrames;
  freeFrames = sim->freePhysicalFrames;
  Dllist usedFrames;
  usedFrames = sim->usedPhysicalFrames;
  Dllist nil;
  nil = dll_nil(freeFrames);
  Dllist ffp;
  ffp = dll_first(freeFrames);

  if(ffp != nil){
    PhysicalFrame* freeFrame;
    freeFrame = ffp->val.v;
    dll_delete_node(ffp);
    dll_append(usedFrames, new_jval_v(freeFrame));
    return freeFrame;
  } else {
    return -1;
  }

}

PhysicalFrame* findPtEvict(MMUSim* sim){
  Dllist freeFrames;
  Dllist usedFrames;
  freeFrames = sim->freePhysicalFrames;
  usedFrames = sim->usedPhysicalFrames;
  Dllist uNil;
  Dllist fNil;
  uNil = dll_nil(usedFrames);
  fNil = dll_nil(freeFrames);
  Dllist uFrame;
  Dllist fFrame;
  uFrame = dll_first(usedFrames);

  int rep = sim->rep;

  PhysicalFrame* evictFrame;

  if(rep == 1){ // FIFO

  } else if(rep == 2){ // LRU

  } else if(rep == 3){ // LFU

  } else if(rep == 4){ // MFU

  } else{ // RANDOM
    int max = sim->pageEntries;
    int randomIndex = ((double) rand() / (((double)RAND_MAX)+1)) * (max+1);

    
    int i;
    for( i  = 0; i < randomIndex; i++){
      uFrame = uFrame->flink;
    }
    if(uFrame == uNil){
      uFrame = uFrame->flink;
    }
    evictFrame = uFrame->val.v;
    
  }

  return evictFrame;

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

    MMUProcess* proc;
    proc = getProcess(sim, trace->pid);

    ProcessStats* pStat;
    pStat = &proc->stats;
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

    PhysicalFrame* tlbHit;
    tlbHit = checkTLB(sim, trace->address);
    if((int)tlbHit != -1){
      if(sim->log) {fprintf(stderr, "\tTLB hit? yes\n");}
      simStats->overallLat = simStats->overallLat + sim->memLat;
      // if write, mark dirty
      // return frame number
      
    }else{
      if(sim->log) {fprintf(stderr, "\tTLB hit? no\n");}
      pStat->tlbMisses = pStat->tlbMisses + 1;
      simStats->tlbMisses = simStats->tlbMisses + 1;
      
      PageTableEntry* pte;
      pte = checkPageTable(sim, proc->pgtbl, trace);

      if(pte->present){
        if(sim->log) {fprintf(stderr, "\tPage fault? no\n");}
        addPageToTlb(sim, trace->address, pte);
        // do tlb load
        // retry
      } else {
        if(sim->log) {fprintf(stderr, "\tPage fault? yes\n");}
        pStat->pageFaults = pStat->pageFaults + 1;
        simStats->pageFaults = simStats->pageFaults + 1;

        // Is there room?
        PhysicalFrame* pfn;
        pfn = findFreePhysicalPage(sim);
        if(pfn != -1){
          if(sim->log) {fprintf(stderr, "\tMain memory eviction? no\n");}
          simStats->overallLat = simStats->overallLat + sim->diskLat;
          pte->page = pageNum;
          pte->physicalFrame = pfn;
          pte->pid = proc->pid;
          pte->present = 1;
          pfn->pid = proc->pid;
          // update PTE (present bit = 1) + memory access
          // update tlb
          addPageToTlb(sim, trace->address, pte);
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
