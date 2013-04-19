/* Jeremy Bernstein
   CS481 Spring 2013
   Lab 3 - MMU
*/

#ifndef TLBandPGTBL
#define TLBandPGTBL 

#include <limits.h>
#include <stdio.h>
#include <time.h>

#include "memStruct.h"

PageTableEntry* checkTLB(MMUSim* sim, unsigned int pageNum){
  TLB* tlb;
  tlb = sim->tlb;
  TLB_Entry* cache = tlb->cache;

  int i;
  for( i = 0; i < tlb->size; i++){
    TLB_Entry* entry;
    entry = &cache[i];
    if(entry->used == 1 && entry->page->page == pageNum){
      clock_t curr;
      curr = clock();
      entry->lastUsed = curr;
      PhysicalFrame* frame;
      frame = entry->page->physicalFrame;
      frame->lastUsed = curr;
      return entry->page;
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
  if(count >= tlb->size){
    if(sim->log){ fprintf(stderr, "\tTLB eviction? yes\n" );}
    TLB_Entry* evicted;
    evicted = &cache[oldestIndex];

    if(sim->log){ fprintf(stderr, "\tpage %d evicted from TLB\n",evicted->page->page );}

    evicted->virtAddress = addr;
    evicted->page = pte;
    evicted->lastUsed = clock();

  }

  return -1;

}

PageTableEntry* checkPageTable(MMUSim* sim, PageTable* pageTable, unsigned int pageNum){
  PageTableEntry* table = pageTable->table;

  return &table[pageNum];
}




#endif
