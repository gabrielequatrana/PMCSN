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
bool slot_switched[3];  // Used to check if the simulation switched to another time slot in finite horizon simulation

network_struct network;                     // Network information
network_configuration config;               // Network configuration
completions_list_struct completions_list;   // List of job completed during the simulation

// Main function to start the simulation
int main(int argc, char *argv[]) {

    // Two parameters are needed: type of simulation and time slot number (used only in infinite horizon simulation)
    if (argc != 3) {
        printf("Usage: ./simulator <FINITE/INFINITE> <TIME_SLOT>\n");
        exit(0);
    }

    // Assign simulation parameters to the variables
    simulation_mode = argv[1];
    time_slot = strtol(argv[2], NULL, 0);

    // Wrong time slot parameter
    if (time_slot > 2) {
        printf("Time slot has to be between 0 and 2\n");
        exit(0);
    }

    // Finite horizon simulation
    if (strcmp(simulation_mode, "FINITE") == 0) {
        // Set the random seed outside the simulation cycle
        PlantSeeds(123123123);
        finite_horizon_simulation(NUM_REPS);
    }

    // Infinite horizon simulation
    else if (strcmp(simulation_mode, "INFINITE") == 0) {
        // Set the random seed outside the simulation cycle
        PlantSeeds(123123123);
        infinite_horizon_simulation(time_slot);
    }

    // Wrong simulation mode parameter
    else {
        printf("Mode has to be FINITE or INFINITE\n");
        exit(0);
    }
}