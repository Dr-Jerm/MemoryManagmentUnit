/* Jeremy Bernstein
   CS481 Spring 2013
   Lab 3 - MMU
*/

#ifndef MMUUTIL
#define MMUUTIL

#include <limits.h>
#include "memStruct.h"


void printProcSpecs(MMUProcess* proc){
  ProcessStats simStats = proc->stats;
  printf("\n");

  printf("Process %d\n", proc->pid);
  printf("\tMemory references: %d\n", simStats.memoryRef);
  printf("\tTLB misses: %d\n", simStats.tlbMisses);
  printf("\tPage faults: %d\n", simStats.pageFaults);
  printf("\tClean evictions: %d\n", simStats.cleanEvict);
  printf("\tDirty evictions: %d\n", simStats.dirtyEvict);

  float percentDirty = 0.0;
  if(simStats.cleanEvict > 0 || simStats.dirtyEvict > 0){
    percentDirty = (float)simStats.dirtyEvict / (float)(simStats.cleanEvict + simStats.dirtyEvict);
  }
  printf("\tPercentage dirty evictions: %3.2f%\n", percentDirty*100);


}

void sortAndPrintProcStats(Dllist processes){
  Dllist nil;
  nil = dll_nil(processes);
  Dllist p;
  p = dll_first(processes);

  MMUProcess* lowProc;
  Dllist low;
  int lowVal = INT_MAX;

  while(p != nil || p->flink != nil){
    if(p == nil){
      printProcSpecs(lowProc);
      dll_delete_node(low);
      lowVal = INT_MAX;
    }else{

      MMUProcess* proc;
      proc = p->val.v;

      if(proc->pid < lowVal){
        low = p;
        lowVal = proc->pid;
        lowProc = proc;
      }

    }
    p = p->flink;
  }

}


#endif
