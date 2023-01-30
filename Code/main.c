#include "config.h"

#include "./Logic/var.h"
#include "./Logic/FiniteHorizon/finite_horizon.h"
#include "./Logic/InfiniteHorizon/infinite_horizon.h"

char *simulation_mode;  // Finite/Infinite horizon simulation
int time_slot;          // Time slot used in the infinite horizon simulation

struct clock_struct clock;      // Clock to keep track of time
struct node nodes[NUM_NODES];   // List of nodes in the network

double arrival_rate;    // Current arrival rate
int job_completed;      // Number of job completed
int job_dropped;        // Number of job dropped
int job_lost;           // Number of job lost
bool slot_switched[3];  // Used to check if the simulation switched to another time slot in finite horizon simulation

network_struct network;                         // Network information
network_configuration config;                   // Network configuration
network_configuration *config_ptr;              // Network configuration pointer
completions_list_struct completions_list;       // List of job completed during the simulation

double rep_cost[NUM_REPS];                          // Costs of each repetition of the finite horizon simulation
double finite_horizon_statistics[NUM_REPS][3];      // Finite horizon simulation statistics
double infinite_horizon_statistics[BATCH_K];        // Infinite horizon simulation statistics
double infinite_horizon_delay[BATCH_K][NUM_NODES];  // Infinite horizon simulation delays

double finite_horizon_means_utilization[NUM_REPS][3][NUM_NODES];    // Average utilization of finite horizon simulation
double infinite_horizon_means_utilization[BATCH_K][NUM_NODES];      // Average utilization of infinite horizon simulation
double infinite_horizon_loss[BATCH_K];                              // Jobs lost during infinite horizon simulation

int streamID;                   // Current streamID of the simulation
server *next_completion_server;

// Main function to start the simulation
int main(int argc, char *argv[]) {

    // Two parameters are needed: type of simulation and time slot number (used only in infinite horizon simulation)
    if (argc != 3) {
        printf("Usage: ./simulate <FINITE/INFINITE> <TIME_SLOT>\n");
        exit(0);
    }

    // Assign simulation parameters to the variables
    simulation_mode = argv[1];
    time_slot = strtol(argv[2], NULL, 0);

    // Wrong time slot parameter
    if (time_slot > 2) {
        printf("Specify time slot between 0 and 2\n");
        exit(0);
    }

    // Finite horizon simulation
    if (strcmp(simulation_mode, "FINITE") == 0) {
        PlantSeeds(231232132); //TODO
        finite_horizon_simulation(NUM_REPS);
    }

    // Infinite horizon simulation
    else if (strcmp(simulation_mode, "INFINITE") == 0) {
        PlantSeeds(231232132);
        infinite_horizon_simulation(time_slot);
    }

    // Wrong simulation mode parameter
    else {
        printf("Specify mode FINITE or INFINITE\n");
        exit(0);
    }
}