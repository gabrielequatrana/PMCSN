#include "./../config.h"

#include "structure.h"

//TODO risistemare ordine, vedere se descrizioni giuste...

extern char *simulation_mode;  // Finite/Infinite horizon simulation
extern int time_slot;          // Time slot used in the infinite horizon simulation

extern struct clock_struct clock;      // Clock to keep track of time
extern struct node nodes[NUM_NODES];   // List of nodes in the network

extern double arrival_rate;    // Current arrival rate
extern int job_completed;      // Number of job completed
extern int job_dropped;        // Number of job dropped
extern int job_lost;           // Number of job lost
extern bool slot_switched[3];  // Used to check if the simulation switched to another time slot in finite horizon simulation

extern network_struct network;                         // Network information
extern network_configuration config;                   // Network configuration
extern network_configuration *config_ptr;              // Network configuration pointer
extern completions_list_struct completions_list;       // List of job completed during the simulation

static const network_struct empty_network;                      // Used to reset the network when switching time slot
static const completions_list_struct empty_completions_list;    // Used to reset the completions list when switching time slot

extern double rep_cost[NUM_REPS];                          // Costs of each repetition of the finite horizon simulation
extern double finite_horizon_statistics[NUM_REPS][3];      // Finite horizon simulation statistics
extern double infinite_horizon_statistics[BATCH_K];        // Infinite horizon simulation statistics
extern double infinite_horizon_delay[BATCH_K][NUM_NODES];  // Infinite horizon simulation delays

extern double finite_horizon_means_utilization[NUM_REPS][3][NUM_NODES];    // Average utilization of finite horizon simulation
extern double infinite_horizon_means_utilization[BATCH_K][NUM_NODES];      // Average utilization of infinite horizon simulation
extern double infinite_horizon_loss[BATCH_K];                              // Jobs lost during infinite horizon simulation

extern int streamID;                   // Current streamID of the simulation
extern server *next_completion_server; // Pointer to the server that will process the next completion in the simulation