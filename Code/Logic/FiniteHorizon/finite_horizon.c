#include "./../../config.h"
#include "./../../Utils/utils.h"

#include "./../logic.h"
#include "./../var.h"

#include "finite_horizon.h"

// Start a finite horizon simulation
void finite_horizon_simulation(int reps) {
    printf("\n==== Finite Horizon Simulation | simulation_time %d | num_repetitions %d ====\n", SLOTS_SUM, NUM_REPS);

    // Initialize configuration
    init_config();
    print_configuration(&config);
    print_line();

    // Repeat a finite horizon run reps times
    for (int rep = 0; rep < reps; rep++) {

        // Launch a run
        finite_horizon_run(rep);

        // Utilization computed for the first repetition
        if (rep == 0) {
            print_csv_utilization_finite_horizon(&network, clock.current, 2);
        }

        if (DEBUG && rep == reps-1) {
            print_statistics(&network, nodes, clock.current);
        }

        // Clear the environment after every run
        clear_environment(); 
        //print_progress_bar(rep, reps, rep - 1);
    }

    // Print results
    print_line();
    print_csv_rt_finite_horizon();
    print_result_finite_horizon();
}

// Executes a finite horizon run
void finite_horizon_run(int rep) {

    // Initialize the network
    init_network();
    
    // Repeat until the end of the simulation
    int n = 1;
    while (clock.arrival <= SLOTS_SUM) {

        // Set current time slot
        set_time_slot(rep);

        // Find the next completion and compute next clock
        struct completion *next_completion = &completions_list.list[0];
        clock.next = min(next_completion->value, clock.arrival);
        
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
        }

        // The current event is a completion
        else {
            process_completion(*next_completion);
        }

        // Compute clock statistics every 5 minutes (300 seconds)
        if (clock.current >= (n-1)*300 && clock.current < n*300 && job_completed > 16 && clock.arrival < SLOTS_SUM) {
            compute_continuous_statistics(&network, nodes, clock.current);
            n++;
        }
    }

    // Compute the statistics of the run and update servers online time
    compute_statistics_finite_horizon(&network, nodes, clock.current, finite_horizon_statistics, rep);
    update_server_online_time();
    rep_cost[rep] = compute_cost(&network);
}

// Compute statistics of the finite horizon simulation run
void compute_statistics_finite_horizon(network_struct *networkStruct, struct node nodesStruct[], double current_clock, double rt_arr[][3], int rep) {
    double visit_rt = 0;
    int timeSlot = networkStruct->time_slot;
    double m = 0.0;

    for (int i = 0; i < NUM_NODES; i++) {
        if (timeSlot == 0) {
            m = networkStruct->online_servers[i];
        }

        else if (timeSlot == 1) {
            double s1 = networkStruct->config->slot_config[0][i];
            double s2 = networkStruct->online_servers[i];
            m = (3.0 * s1 + 11.0 * s2) / 14.0;
        }

        else if (timeSlot == 2) {
            double s1 = networkStruct->config->slot_config[0][i];
            double s2 = networkStruct->config->slot_config[1][i];
            double s3 = networkStruct->online_servers[i];
            m = (3.0 * s1 + 11.0 * s2 + 5.0 * s3) / 19.0;
        }

        int arrivals = nodesStruct[i].total_arrivals;
        int real_arrivals = arrivals - nodesStruct[i].total_losses;
        double inter = current_clock / nodesStruct[i].total_arrivals;

        double wait = nodesStruct[i].time.node / arrivals;
        double service = nodesStruct[i].time.server / real_arrivals;

        double external_arrival_rate = 1 / (current_clock / nodesStruct[NODO_UNO].total_arrivals);
        double lambda_i = 1 / inter;
        double mu = 1 / service;
        double thr = min(m * mu, lambda_i);

        if (i == NODO_QUATTRO) {
            thr = lambda_i;
        }

        double visit = thr / external_arrival_rate;
        visit_rt += wait * visit;
    }

    rt_arr[rep][timeSlot] = visit_rt;
}

// Compute statistics for continuous analysis
void compute_continuous_statistics(network_struct *networkStruct, struct node nodesStruct[], double current_clock) {
    char filename[100];
    snprintf(filename, 100, "results/finite_horizon/continuos_finite.csv");
    FILE *csv;
    csv = open_csv_append_mode(filename);

    double visit_rt = 0;
    int timeSlot = networkStruct->time_slot;
    double m = 0.0;
    for (int i = 0; i < NUM_NODES; i++) {
        if (timeSlot == 0) {
            m = networkStruct->online_servers[i];
        }

        else if (timeSlot == 1) {
            double s1 = networkStruct->config->slot_config[0][i];
            double s2 = networkStruct->online_servers[i];
            m = (3.0 * s1 + 11.0 * s2) / 14.0;
        }

        else if (timeSlot == 2) {
            double s1 = networkStruct->config->slot_config[0][i];
            double s2 = networkStruct->config->slot_config[1][i];
            double s3 = networkStruct->online_servers[i];
            m = (3.0 * s1 + 11.0 * s2 + 5.0 * s3) / 19.0;
        }

        int arrivals = nodesStruct[i].total_arrivals;
        int real_arrivals = arrivals - nodesStruct[i].total_losses;
        double inter = current_clock / nodesStruct[i].total_arrivals;

        double wait = nodesStruct[i].time.node / arrivals;
        double service = nodesStruct[i].time.server / real_arrivals;

        double external_arrival_rate = 1 / (current_clock / nodesStruct[NODO_UNO].total_arrivals);
        double lambda_i = 1 / inter;
        double mu = 1 / service;
        double thr = min(m * mu, lambda_i);

        if (i == NODO_QUATTRO) {
            thr = lambda_i;
        }

        double visit = thr / external_arrival_rate;
        visit_rt += wait * visit;
    }

    append_on_csv_2(csv, visit_rt, current_clock);
    fclose(csv);
}

// Write in a csv file the finite horizon statistics
void print_csv_rt_finite_horizon() {
    FILE *csv;
    char filename[100];
    for (int i = 0; i < 3; i++) {
        snprintf(filename, 100, "results/finite_horizon/rt_finite_horizon_slot_%d.csv", i);
        csv = open_csv(filename);
        for (int j = 0; j < NUM_REPS; j++) {
            append_on_csv_3(csv, i, finite_horizon_statistics[j][i], 0);
        }
        fclose(csv);
    }
}

// Write in a csv file the utilization
void print_csv_utilization_finite_horizon(network_struct *networkStruct, double current_clock, int slot) {
    FILE *csv;
    char filename[100];

    for (int i = 0; i < NUM_NODES; i++) {
        double utilization;
        snprintf(filename, 100, "results/finite_horizon/u_%d_finite_horizon_slot_%d.csv", i, slot);
        csv = open_csv(filename);
        for (int j = 0; j < MAX_SERVERS; j++) {
            server *s = &networkStruct->server_list[i][j];
            if (!s->used) {
                break;
            }
            else {
                utilization = (s->service.service_time / current_clock);
                append_on_csv_1(csv, utilization);
            }
        }
    }
    fclose(csv);
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
        compute_statistics_finite_horizon(&network, nodes, clock.current, finite_horizon_statistics, rep);

        // Utilization computed for the first repetition
        if (rep == 0) {
            print_csv_utilization_finite_horizon(&network, clock.current, network.time_slot);
        }

        network.time_slot = 1;      // Second time slot
        arrival_rate = LAMBDA_2;    // Arrival rate second time slot
        slot_switched[1] = true;    // Switched to the second time slot
        update_network();           // Update network configuration
    }

    // Simulation switched to the third time slot
    if (clock.current >= TIME_SLOT_1 + TIME_SLOT_2 && !slot_switched[2]) {

        // Compute statistics
        compute_statistics_finite_horizon(&network, nodes, clock.current, finite_horizon_statistics, rep);

        // Utilization computed for the first repetition
        if (rep == 0) {
            print_csv_utilization_finite_horizon(&network, clock.current, network.time_slot);
        }

        network.time_slot = 2;      // Third time slot
        arrival_rate = LAMBDA_3;    // Arrival rate third time slot
        slot_switched[2] = true;    // Switched to the third time slot
        update_network();           // Update network configuration
    }
}
