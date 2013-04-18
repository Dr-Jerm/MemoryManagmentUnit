/* Jeremy Bernstein
   CS481 Spring 2013
   Lab 3 - MMU
*/

#include <string.h>
#include <stdio.h>
#ifndef PARSER
#define PARSER
#include "memStruct.h"

void parseConf(Config* conf , char* confFile){

  FILE *file = fopen (confFile, "r");

  if (file == NULL){
    printf("error opening conf file\n");
    exit(1);
  }

  char line [256];

  while( fgets( line, sizeof line, file) != NULL){
    char paramName[128];
    char *param;

    strcpy (paramName, line);
    strtok_r(paramName, " ", &param);

    if(!strcmp(paramName, "physical-memory-size:")){ conf->pms = atoi(param);} 
    else if(!strcmp(paramName, "frame-size:")){conf->frame = atoi(param);}
    else if(!strcmp(paramName, "memory-latency:")){conf->memLat = atoi(param);}
    else if(!strcmp(paramName, "page-replacement:")){
      if(!strcmp(param, "FIFO\n")){conf->rep = 1;}
      else if(!strcmp(param, "LRU\n")){conf->rep = 2;}
      else if(!strcmp(param, "LFU\n")){conf->rep = 3;}
      else if(!strcmp(param, "MFU\n")){conf->rep = 4;}
      else if(!strcmp(param, "RANDOM\n")){conf->rep = 5;}
      else{
        printf("unrecognized replacement type.\n");
        exit(1);
      }
    }
    else if(!strcmp(paramName, "tlb-size:")){conf->tlbSz = atoi(param);}
    else if(!strcmp(paramName, "tlb-latency:")){conf->tlbLat = atoi(param);}
    else if(!strcmp(paramName, "disk-latency:")){conf->diskLat = atoi(param);}
    else if(!strcmp(paramName, "logging-output:")){
      if(!strcmp(param, "on\n")){conf->log = 1;}
      else if(!strcmp(param, "off\n")){conf->log = 0;}
      else{
        printf("unrecognized loggging mode.\n");
        exit(1);
      }
    }
  }
}

void parseTrace(Dllist* stack, char* traceFile){

  FILE *file = fopen(traceFile, "r");
  if(file == NULL){
    printf("error opening trace file\n");
    exit(1);
  }

  char line [24]; 

  while( fgets( line, sizeof line, file) != NULL){
    int pid;
    int op;
    char addr[14];
  
    
    char *token;
    char *delim = " ";
    
    token = strtok(line, delim);
    if(!strcmp(token, "") || !strcmp(token, "\n")){break;}
    pid = atoi(token);
    
    token = strtok(NULL, delim);
    if(!strcmp(token, "R")){op = 1;}
    else if(!strcmp(token, "W")){op = 2;}
    else if(!strcmp(token, "I")){op = 3;}
    else{
      printf("unrecognized traceFunction.\n");
      exit(1);
    }
    token = strtok(NULL, delim);
    strcpy(addr, token);

    unsigned int addressHex;
    addressHex = strtoul(addr, NULL, 16);

    Trace* t = malloc(sizeof(Trace));
    t->pid = pid;
    t->op = op;
    t->address = addressHex;
    
    dll_prepend(stack, new_jval_v((void*) t));
  }
}
//  printf("traceComplete\n"); }


#endif
