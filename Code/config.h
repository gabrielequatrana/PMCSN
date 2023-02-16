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
#define NUM_NODES   4       // Number of nodes
#define MAX_SERVERS 50      // Max number of servers of each node
#define START       0.0     // Start time

// Server states
#define BUSY 1  // Server busy
#define IDLE 0  // Server busy

#define ACTIVATED   1   // Server activated
#define DEACTIVATED 0   // Server deactivated

#define USED     1  // Server used
#define NOT_USED 0  // Server not used

// Configuration of time slots
#define NUM_SLOTS   3                                           // Number of time slots
#define TIME_SLOT_1 25200                                       // Duration of first time slot
#define TIME_SLOT_2 43200                                       // Duration of second time slot
#define TIME_SLOT_3 18000                                       // Duration of third time slot
#define SLOTS_SUM   (TIME_SLOT_1 + TIME_SLOT_2 + TIME_SLOT_3)   // Sum of durations of the time slots

// Arrival rate for each slot
#define LAMBDA_1 1.607143
#define LAMBDA_2 4.0625
#define LAMBDA_3 3.00

// Services time
#define SERV_REQUEST_ACCEPTANCE                 1
#define SERV_SUBSCRIBER_REQUEST_PROCESSING      5
#define SERV_NON_SUBSCRIBER_REQUEST_PROCESSING  15
#define SERV_RESPONSE_VALIDATION                3

// Routing probability
#define P_EXIT_FROM_ACCEPTANCE  10
#define P_REQUEST_SUBSCRIBER    29.16

// Monthly costs per server
#define COST_REQUEST_ACCEPTANCE                 110
#define COST_SUBSCRIBER_REQUEST_PROCESSING      670
#define COST_NON_SUBSCRIBER_REQUEST_PROCESSING  340
#define COST_RESPONSE_VALIDATION                200

// Repetitions number and batch
#define NUM_REPS 128    // Number of run in finite horizon simulation
#define BATCH_B  1024   // Batch size in infinite horizon simulation
#define BATCH_K  128    // Number of batch in infinite horizon simulation