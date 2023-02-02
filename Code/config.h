#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>

#include "Utils/Standard/rngs.h"
#include "Utils/Standard/rvgs.h"

// Method to initialize the configuration of the network
void init_config();

// Simulation parameters
#define NUM_NODES 4     // Number of nodes
#define MAX_SERVERS 50  // Max number of servers of each node
#define START 0.0       // Start time
#define DEBUG true     // Enable debug functions

// Server states
#define BUSY 1  // Server busy
#define IDLE 0  // Server busy

#define ACTIVATED   1   // Server activated
#define DEACTIVATED 0   // Server deactivated

#define USED     1  // Server used
#define NOT_USED 0  // Server not used

// Configuration of time slots
#define NUM_SLOTS   3                                           // Number of time slots
#define TIME_SLOT_1 10800                                           // Duration of first time slot
#define TIME_SLOT_2 39600                                           // Duration of second time slot
#define TIME_SLOT_3 18000                                           // Duration of third time slot
#define SLOTS_SUM   (TIME_SLOT_1 + TIME_SLOT_2 + TIME_SLOT_3)   // Sum of durations of the time slots

// Arrival rate for each slot
#define LAMBDA_1 0.405556  // Arrival rate first time slot
#define LAMBDA_2 0.829545  // Arrival rate second time slot
#define LAMBDA_3 0.365     // Arrival rate third time slot

// Services time
#define SERV_TIPOUNO     15  // Service time first node
#define SERV_TIPODUE     90  // Service time second node
#define SERV_TIPOTRE     10  // Service time third node
#define SERV_TIPOQUATTRO 25  // Service time fourth node

// Routing probability
#define P_1 0.2
#define P_2 48.19841
#define P_3 25.95299
#define P_4 25.6486
#define P_5 21.0324

// Monthly costs per server
#define COST_TIPOUNO     300
#define COST_TIPODUE     200
#define COST_TIPOTRE     50
#define COST_TIPOQUATTRO 1300

// Repetitions number and batch
#define NUM_REPS 128    // Number of run in finite horizon simulation
#define BATCH_B  1024   // Batch size in infinite horizon simulation
#define BATCH_K  128    // Number of batch in infinite horizon simulation