/* Jeremy Bernstein
   CS481 Spring 2013
   Lab 3 - MMU
*/

// make clean
// make
// gcc -o mmu mmu.c -lm

#include <stdio.h>
#include <stdlib.h>
#include "memStruct.h"
#include "parser.c"
#include "mmusim.c"


 // Main Config object
Config conf;

Dllist traceStack;

MMUSim mmuSim;


main(int argc, char **argv){
 
  // Check for correct input 
  if(argc != 3){
    printf("usage: mmu <config_file> <trace_file>\n");
    exit(1);
  }
  
  parseConf(&conf , argv[1]);

  traceStack = new_dllist();
  parseTrace(&traceStack, argv[2]);

  setupSim(&conf, &mmuSim);

  runSim(&mmuSim, &traceStack);

  endSim(&mmuSim);
}
