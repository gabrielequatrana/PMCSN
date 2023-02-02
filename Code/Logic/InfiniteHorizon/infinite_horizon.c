#include "./../../config.h"
#include "./../../Utils/utils.h"

#include "./../logic.h"
#include "./../var.h"

#include "infinite_horizon.h"

// Start an infinite horizon simulation
void infinite_horizon_simulation(int timeSlot) {
    printf("\n==== Infinite Horizon Simulation slot %d | num batch %d ====", timeSlot, BATCH_K);
    
    // Initialize configuration
    init_config();
    print_configuration(&config);
    print_line();
    
    // Set arrival rate based on the time slot chosen
    switch (timeSlot) {
        case 0:
            arrival_rate = LAMBDA_1;
        case 1:
            arrival_rate = LAMBDA_2;
        case 2:
            arrival_rate = LAMBDA_3;
        default:
            break; 
    }

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
        infinite_horizon_batch(timeSlot, k);

        if (k == 0) {
            print_statistics(&network, nodes, clock.current);
        }
        
        // Reset network statistics after every run
        reset_nodes_parameters();
        //print_progress_bar(k, BATCH_K, k-1);
    }

    // Print results
    print_csv_rt_infinite_horizon(timeSlot);
    update_server_online_time();
    print_results_infinite_horizon(timeSlot);
    }

// Run an infinite horizon batch
void infinite_horizon_batch(int timeSlot, int k) {

    network.time_slot = timeSlot;

    // Repeat for BATCH_B times
    int arr = 0, compl = 0;
    while (arr < BATCH_B || compl < BATCH_B) {

        // Find the next completion
        struct completion *next_completion = &completions_list.list[0];

        // Compute next clock
        if (arr >= BATCH_B) {
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
    double current_clock = clock.current - clock.current_batch;
    compute_statistics_infinite_horizon(&network, nodes, current_clock, infinite_horizon_statistics, k, infinite_horizon_delay);
    compute_utilization_infinite_horizon(k);
}

// Compute statistics of the infinite horizon simulation batch
void compute_statistics_infinite_horizon(network_struct *networkStruct, struct node nodesStruct[], double current_clock, double rt_arr[], int pos, double dl_arr[][NUM_NODES]) {
    double visit_rt = 0;

    for (int i = 0; i < NUM_NODES; i++) {
        int arr = nodesStruct[i].total_arrivals;
        int r_arr = arr - nodesStruct[i].total_losses;
        double inter = current_clock / arr;

        double wait = nodesStruct[i].time.node / arr;
        double delay = nodesStruct[i].time.queue / r_arr;
        double service = nodesStruct[i].time.server / r_arr;

        double external_arrival_rate = 1 / (current_clock / nodesStruct[NODO_UNO].total_arrivals);
        double lambda_i = 1 / inter;
        double mu = 1 / service;
        double thr = min(networkStruct->online_servers[i] * mu, lambda_i);

        if (i == NODO_QUATTRO) {
            thr = lambda_i;
        }

        double visit = thr / external_arrival_rate;
        visit_rt += visit * wait;
        dl_arr[pos][i] += delay;
    }

    rt_arr[pos] = visit_rt;
}

// Compute utilization of the infinite horizon simulation batch
void compute_utilization_infinite_horizon(int k) {
    for (int i = 0; i < NUM_NODES; i++) {
        double p = 0;
        int n = 0;
        for (int j = 0; j < MAX_SERVERS; j++) {
            if (network.server_list[i][j].used == USED) {
                p += (network.server_list[i][j].service.service_time / clock.current);
                n++;
            }
        }

        if (i == NODO_QUATTRO) {
            double loss_percentage = (float)nodes[i].total_losses / (float)nodes[i].total_arrivals;
            infinite_horizon_loss[k] = loss_percentage;
        }

        infinite_horizon_means_utilization[k][i] = p / n;
    }
}

// Write in a csv file the infinite horizon statistics
void print_csv_rt_infinite_horizon(int slot) {
    char filename[100];
    char filename_p_loss[100];

    snprintf(filename, 100, "results/infinite_horizon/rt_infinite_slot_%d.csv", slot);
    snprintf(filename_p_loss, 100, "results/infinite_horizon/p_loss_infinite_slot_%d.csv", slot);

    FILE *csv;
    FILE *csv_p_loss;

    csv = open_csv(filename);
    csv_p_loss = open_csv(filename_p_loss);

    for (int i = 0; i < BATCH_K; i++) {
        append_on_csv_3(csv, i, infinite_horizon_statistics[i], 0);
        append_on_csv_3(csv_p_loss, i, infinite_horizon_loss[i], 0);
    }

    fclose(csv);
    fclose(csv_p_loss);

    for (int i = 0; i < NUM_NODES - 1; i++) {
        char filename_delays[100];
        snprintf(filename_delays, 100, "results/infinite_horizon/delay_%d_infinite_slot_%d.csv", i, slot);
        FILE *csv_delays;
        csv_delays = open_csv(filename_delays);

        for (int j = 0; j < BATCH_K; j++) {
            append_on_csv_3(csv_delays, j, infinite_horizon_delay[j][i], 0);
        }

        fclose(csv_delays);
    }
}

// Print on the stdout the results of the infinite horizon simulation
void print_results_infinite_horizon(int slot) {
    double cost = compute_cost(&network);
    printf("\n\nSlot %d configuration cost: %f\n", slot, cost);

    double l = 0;
    for (int i = 0; i < NUM_NODES; i++) {
        double p = 0;
        for (int j = 0; j < BATCH_K; j++) {
            p += infinite_horizon_means_utilization[j][i];
            if (i == NODO_TRE) {
                l += infinite_horizon_loss[j];
            }
        }
        printf("\nMean utilization for node %s: %f", string_node_type(i), p / BATCH_K);
    }

    printf("\nLoss perc %f\n", l / BATCH_K);
}
