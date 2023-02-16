#include "./../../config.h"
#include "./../../Utils/utils.h"

#include "./../logic.h"
#include "./../var.h"

#include "infinite_horizon.h"

double infinite_horizon_rt[BATCH_K];                        // Infinite horizon simulation response time statistics
double infinite_horizon_utilization[BATCH_K][NUM_NODES];    // Average utilization of infinite horizon simulation
double infinite_horizon_p_loss[BATCH_K];                    // Infinite horizon simulation p loss statistics

// Start an infinite horizon simulation
void infinite_horizon_simulation(int timeSlot) {
    printf("\n==== Infinite Horizon Simulation slot %d | num_batches %d ====", timeSlot, BATCH_K);
    
    // Initialize configuration
    init_config();
    print_configuration();
    print_line();
    
    // Set arrival rate based on the time slot chosen
    arrival_rate = find_lambda(timeSlot);

    // Clear the environment
    clear_environment();
    
    // Initialize the network
    init_network();
    network.time_slot = timeSlot;

    // Update the network
    update_network();

    // Launch k batches
    for (int k = 0; k < BATCH_K; k++) {

        // Launch a batch
        infinite_horizon_batch(timeSlot, k, BATCH_B);
        
        // Reset network statistics after every run
        reset_nodes_parameters();

    }

    // Simulation Completed
    printf("Simulation Completed");
    print_line();

    // Update csv and print results
    print_csv_rt_infinite_horizon(timeSlot);
    print_csv_p_loss_infinite_horizon(timeSlot);
    update_server_online_time();
    print_results_infinite_horizon(timeSlot);
}

// Run an infinite horizon batch
void infinite_horizon_batch(int timeSlot, int k, int b) {

    // Get current time slot
    network.time_slot = timeSlot;

    // Repeat for BATCH_B times
    int arr = 0, compl = 0;
    while (arr < b || compl < b) {

        // Find the next completion
        struct completion *next_completion = &completions_list.list[0];

        // Compute next clock
        if (arr >= b) {
            clock.next = next_completion->value;
            if (clock.next == INFINITY) {
                break;
            }
        }

        // Compute next clock (get next event)
        else {
            clock.next = min(next_completion->value, clock.arrival);
        }

        // Update nodes time sum
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
            arr++;
        }

        // The current event is a completion
        else {
            process_completion(*next_completion);
            compl++;
        }
    }

    // Compute the statistics of the batch
    compute_statistics_infinite_horizon(k);
    compute_utilization_infinite_horizon(k);
}

// Find the lambda (arrival rate) based on time slot
double find_lambda(int timeSlot) {
    switch (timeSlot) {
        case 0:
            return LAMBDA_1;
        case 1:
            return LAMBDA_2;
        case 2:
            return LAMBDA_3;
        default:
            return 0;
    }
}

// Compute statistics of the infinite horizon simulation batch
void compute_statistics_infinite_horizon(int batch) {

    // Initialize variable
    double response_time = 0;
    double current_clock = clock.current - clock.current_in_batch;

    // Repeat for each node
    for (int i = 0; i < NUM_NODES; i++) {

        // Compute statistics
        int arrivals = nodes[i].total_arrivals;
        int real_arrivals = arrivals - nodes[i].total_losses;
        double inter_arr = current_clock / arrivals;

        double wait = nodes[i].time.node / arrivals;
        double service = nodes[i].time.server / real_arrivals;

        double external_arrival_rate = 1 / (current_clock / nodes[REQUEST_ACCEPTANCE].total_arrivals);
        double lambda_i = 1 / inter_arr;
        double mu = 1 / service;
        double thr = min(network.online_servers[i] * mu, lambda_i);

        if (i == RESPONSE_VALIDATION) {
            thr = lambda_i;
        }

        double visit = thr / external_arrival_rate;
        response_time += visit * wait;
    }

    infinite_horizon_rt[batch] = response_time;
}

// Compute utilization of the infinite horizon simulation batch
void compute_utilization_infinite_horizon(int batch) {
    for (int i = 0; i < NUM_NODES; i++) {
        double p = 0;
        int n = 0;
        for (int j = 0; j < MAX_SERVERS; j++) {
            if (network.server_list[i][j].used == USED) {
                p += (network.server_list[i][j].service.service_time / clock.current);
                n++;
            }
        }

        if (i == RESPONSE_VALIDATION) {
            double loss_percentage = (float)nodes[i].total_losses / (float)nodes[i].total_arrivals;
            infinite_horizon_p_loss[batch] = loss_percentage;
        }

        infinite_horizon_utilization[batch][i] = p / n;
    }
}

// Write in a csv file the infinite horizon response time
void print_csv_rt_infinite_horizon(int slot) {

    // Open file
    char filename[100];
    snprintf(filename, 100, "Results/Infinite_Horizon/rt_infinite_slot_%d.csv", slot);
    FILE *csv;
    csv = open_csv(filename);

    // Update results
    for (int i = 0; i < BATCH_K; i++) {
        append_on_csv_2(csv, i, infinite_horizon_rt[i]);
    }

    // Close file
    fclose(csv);
}

// Write in a csv file the infinite horizon p loss
void print_csv_p_loss_infinite_horizon(int slot) {

    // Open file
    char filename[100];
    snprintf(filename, 100, "Results/Infinite_Horizon/p_loss_infinite_slot_%d.csv", slot);
    FILE *csv;
    csv = open_csv(filename);

    // Update results
    for (int i = 0; i < BATCH_K; i++) {
        append_on_csv_2(csv, i, infinite_horizon_rt[i]);
    }

    // Close file
    fclose(csv);
}

// Print on the stdout the results of the infinite horizon simulation
void print_results_infinite_horizon(int slot) {

    // Compute costs
    double cost = compute_cost(&network);
    printf("\n\nSlot %d configuration cost: %f\n", slot, cost);

    // Compute and print utilization and loss percentage
    double l = 0;
    for (int i = 0; i < NUM_NODES; i++) {
        double p = 0;
        for (int j = 0; j < BATCH_K; j++) {
            p += infinite_horizon_utilization[j][i];
            if (i == NON_SUBSCRIBER_REQUEST_PROCESSING) {
                l += infinite_horizon_p_loss[j];
            }
        }
        printf("\nAverage utilization for node %s: %f", string_node_type(i), p / BATCH_K);
    }

    printf("\n\nLoss percentage: %f\n", l / BATCH_K);
}
