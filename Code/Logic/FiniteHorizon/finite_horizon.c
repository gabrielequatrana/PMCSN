#include "./../../config.h"
#include "./../../Utils/utils.h"

#include "./../logic.h"
#include "./../var.h"

#include "finite_horizon.h"

double rep_cost[NUM_REPS];                  // Costs of each repetition of the finite horizon simulation
double finite_horizon_rt[NUM_REPS][3];      // Finite horizon simulation response time statistics
double finite_horizon_p_loss[NUM_REPS][3];  // Finite horizon simulation p loss statistics

// Start a finite horizon simulation
void finite_horizon_simulation(int reps) {
    printf("\n==== Finite Horizon Simulation | Simulation Time: %d | Number of Reps: %d ====\n", SLOTS_SUM, reps);

    // Delete continuous finite csv file
    remove("Results/Finite_Horizon/continuous_finite.csv");

    // Initialize configuration
    init_config();
    print_configuration();
    print_line();

    // Repeat a finite horizon run reps times
    for (int rep = 0; rep < reps; rep++) {

        // Launch a run
        finite_horizon_run(rep);

        // Clear the environment after every run
        clear_environment();
    }

    // Simulation completed
    printf("Simulation Completed");
    print_line();

    // Save and print results
    print_csv_rt_finite_horizon();
    print_csv_p_loss_finite_horizon();
    print_result_finite_horizon();
}

// Executes a finite horizon run
void finite_horizon_run(int rep) {

    // Initialize the network
    init_network();
    
    // Repeat until the end of the simulation rep
    int n = 1;
    while (clock.arrival <= SLOTS_SUM) {

        // Set current time slot
        set_time_slot(rep);

        // Find the next completion and compute next clock
        struct completion *next_completion = &completions_list.list[0];
        clock.next = min(next_completion->value, clock.arrival);
        
        // Update nodes time
        for (int i = 0; i < NUM_NODES; i++) {
            if (nodes[i].node_jobs > 0) {
                nodes[i].time.node += (clock.next - clock.current) * nodes[i].node_jobs;
                nodes[i].time.queue += (clock.next - clock.current) * nodes[i].queue_jobs;
            }
        }

        // Update current clock
        clock.current = clock.next;

        // The current event is an arrival
        if (clock.current == clock.arrival) {
            process_arrival();
        }

        // The current event is a completion
        else {
            process_completion(*next_completion);
        }

        // Compute clock statistics every 5 minutes (300 seconds)
        if (rep == 0 && clock.current >= (n-1)*300 && clock.current < n*300 && job_completed > 16 && clock.arrival < SLOTS_SUM) {
            compute_continuous_statistics_finite_horizon();
            n++;
        }
    }

    // Compute the statistics of the run and update servers online time
    compute_statistics_finite_horizon(rep);
    update_server_online_time();
    rep_cost[rep] = compute_cost(&network);
}

// Compute response time
double compute_response_time(int timeSlot) {

    // Initialize variables
    double response_time = 0;
    double m = 0.0;

    // Repeat for each node
    for (int i = 0; i < NUM_NODES; i++) {

        // Compute mean number of servers online
        if (timeSlot == 0) {
            m = network.online_servers[i];
        }
        else if (timeSlot == 1) {
            double s1 = network.config->slot_config[0][i];
            double s2 = network.online_servers[i];
            m = (7.0 * s1 + 12.0 * s2) / 19.0;
        }
        else if (timeSlot == 2) {
            double s1 = network.config->slot_config[0][i];
            double s2 = network.config->slot_config[1][i];
            double s3 = network.online_servers[i];
            m = (7.0 * s1 + 12.0 * s2 + 5.0 * s3) / 24.0;
        }

        // Compute statistics
        int arrivals = nodes[i].total_arrivals;
        int real_arrivals = arrivals - nodes[i].total_losses;
        double inter_arr = clock.current / nodes[i].total_arrivals;

        double wait = nodes[i].time.node / arrivals;
        double service = nodes[i].time.server / real_arrivals;

        double external_arrival_rate = 1 / (clock.current / nodes[REQUEST_ACCEPTANCE].total_arrivals);
        double lambda_i = 1 / inter_arr;
        double mu = 1 / service;
        double thr = min(m * mu, lambda_i);

        if (i == RESPONSE_VALIDATION) {
            thr = lambda_i;
        }

        double visit = thr / external_arrival_rate;
        response_time += wait * visit;
    }

    return response_time;
}

// Compute statistics of the finite horizon simulation run
void compute_statistics_finite_horizon(int rep) {

    // Compute response time
    int timeSlot = network.time_slot;
    double response_time = compute_response_time(timeSlot);

    // Get arrivals and losses
    int losses = nodes[RESPONSE_VALIDATION].total_losses;
    int arrivals = nodes[RESPONSE_VALIDATION].total_arrivals;

    // Update statistics
    finite_horizon_p_loss[rep][timeSlot] = losses / (double)arrivals;
    finite_horizon_rt[rep][timeSlot] = response_time;
}

// Compute statistics for continuous analysis and update csv file
void compute_continuous_statistics_finite_horizon() {

    // Open csv file
    char filename[100];
    snprintf(filename, 100, "Results/Finite_Horizon/continuous_finite.csv");
    FILE *csv;
    csv = open_csv_append_mode(filename);

    // Compute response time
    double response_time = compute_response_time(network.time_slot);

    // Update csv file
    append_on_csv_3(csv, response_time, clock.current);
    fclose(csv);
}

// Write in a csv file the finite horizon response time
void print_csv_rt_finite_horizon() {
    FILE *file;
    char filename[100];
    for (int i = 0; i < 3; i++) {
        snprintf(filename, 100, "Results/Finite_Horizon/rt_finite_horizon_slot_%d.csv", i);
        file = open_csv(filename);
        for (int j = 0; j < NUM_REPS; j++) {
            append_on_csv_1(file, finite_horizon_rt[j][i]);
        }
        fclose(file);
    }
}

// Write in a csv file the finite horizon p loss
void print_csv_p_loss_finite_horizon() {
    FILE *file;
    char filename[100];
    for (int i = 0; i < 3; i++) {
        snprintf(filename, 100, "Results/Finite_Horizon/p_loss_finite_horizon_slot_%d.csv", i);
        file = open_csv(filename);
        for (int j = 0; j < NUM_REPS; j++) {
            append_on_csv_1(file, finite_horizon_p_loss[j][i]);
        }
        fclose(file);
    }
}

// Print on the stdout the results of the finite horizon simulation
void print_result_finite_horizon() {
    double total = 0;
    for (int i = 0; i < NUM_REPS; i++) {
        total += rep_cost[i];
    }
    printf("\nTotal mean config cost: %f\n", total / NUM_REPS);
}

// Set the time slot based on the current repetition of the finite horizon simulation
void set_time_slot(int rep) {

    // The simulation has started
    if (clock.current == START) {
        network.time_slot = 0;      // First time slot
        arrival_rate = LAMBDA_1;    // Arrival rate first time slot
        slot_switched[0] = true;    // Switched to the first time slot
        update_network();           // Update network configuration
    }

    // Simulation switched to the second time slot
    if (clock.current >= TIME_SLOT_1 && clock.current < TIME_SLOT_1 + TIME_SLOT_2 && !slot_switched[1]) {

        // Compute statistics
        compute_statistics_finite_horizon(rep);

        network.time_slot = 1;      // Second time slot
        arrival_rate = LAMBDA_2;    // Arrival rate second time slot
        slot_switched[1] = true;    // Switched to the second time slot
        update_network();           // Update network configuration
    }

    // Simulation switched to the third time slot
    if (clock.current >= TIME_SLOT_1 + TIME_SLOT_2 && !slot_switched[2]) {

        // Compute statistics
        compute_statistics_finite_horizon(rep);

        network.time_slot = 2;      // Third time slot
        arrival_rate = LAMBDA_3;    // Arrival rate third time slot
        slot_switched[2] = true;    // Switched to the third time slot
        update_network();           // Update network configuration
    }
}
