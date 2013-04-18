

typedef struct {
  int pms; // physical-memory-size
  int frame; // frame-size
  int memLat; // memory-latency
  int rep; //page-replacement: 1-FIFO, 2-LRU, 3-LFU, 4-MFU, 5-RANDOM
  int tblSz; //table-size;
  int tblLat; //table-Latency
  int diskLat; //disk-latency
  int log; //logging-output: 0-OFF, 1-ON
} Config;
