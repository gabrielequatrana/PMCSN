#include "./../config.h"

#include "structure.h"

extern char *simulation_mode;  // Finite/Infinite horizon simulation

extern struct clock_struct clock;      // Clock to keep track of time
extern struct node nodes[NUM_NODES];   // List of nodes in the network

extern double arrival_rate;    // Current arrival rate
extern int job_completed;      // Number of job completed
extern bool slot_switched[3];  // Used to check if the simulation switched to another time slot in finite horizon simulation

extern network_struct network;                         // Network information
extern network_configuration config;                   // Network configuration
extern completions_list_struct completions_list;       // List of job completed during the simulation