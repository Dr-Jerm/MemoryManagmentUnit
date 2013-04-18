

#include <stdio.h>
#include <stdlib.h>
#include "mmu.h"
#include "parser.c"

 // Main Config object

Config conf = {.pms = 0, .frame = 0, .memLat = 0, .rep = 0,
                 .tblSz = 0, .tblLat = 0, .diskLat = 0, .log = 0};


main(int argc, char **argv){
 
  // Check for correct input 
  if(argc != 3){
    printf("usage: mmu <config_file> <trace_file>\n");
    exit(1);
  }
  
  parseConf(&conf , argv[1]);

}

