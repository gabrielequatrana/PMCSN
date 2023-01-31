#include "../config.h"

#include "utils.h"

// Compute the minimum between two double
double min(double x, double y) {
    return (x < y) ? x : y;
}

int routing() {
    double random = Uniform(0, 100);
    if (random < P_1) {
        return EXIT;
    }
    else if (random < P_1 + P_2) {
        return NODO_DUE;
    }
    else {
        return NODO_TRE;
    }
}

// Find the destination node of the job based on the current node
int find_destination_node(enum node_type type) {
    switch (type) {
        case NODO_UNO:
            return routing();
        case NODO_DUE:
        case NODO_TRE:
            return NODO_QUATTRO;
        default:
            return 0;
    }
}

// Return the string name of the node type specified as parameter
char *string_node_type(enum node_type type) {
    char *strings[] = {"NODO_UNO", "NODO_DUE", "NODO_TRE", "NODO_QUATTRO"};
    return strings[type];
}

// TODO Compute statistics of the clock
void compute_clock_statistics(network_struct *networkStruct, struct node nodesStruct[], double current_clock) {
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
        int job_queue = nodesStruct[i].queue_jobs;
        double inter = current_clock / nodesStruct[i].total_arrivals;

        double wait = nodesStruct[i].time.node / arrivals;
        double delay = nodesStruct[i].time.queue / real_arrivals;
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

// Print a separation line
void print_line() {
    printf("\n----------------------------------------------------------------------------------------------\n");
}

// Print simulation configuration 
void print_configuration(network_configuration *networkConfiguration) {
    for (int slot = 0; slot < 3; slot++) {
        printf("\nSlot %d\n", slot);
        for (int node = 0; node < NUM_NODES; node++) {
            printf("> %s: %d\n", string_node_type(node), networkConfiguration->slot_config[slot][node]);
        }
    }
}

// Print a progress bar
void print_progress_bar(double part, double total, double old_part) {
    double percentage = part / total * 100;
    double old_percentage = old_part / total *100;

    if ((int)old_percentage == (int)percentage) {
        return;
    }

    printf("\rSimulation progress: |");

    for (int i = 0; i <= percentage / 2; i++) {
        printf("█");
        fflush(stdout);
    }

    for (int j = percentage / 2; j < 50 - 1; j++) {
        printf(" ");
    }

    printf("|");
    printf(" %02.0f%%", percentage + 1);
}

// Print the loss probability
void print_loss_prob(struct node *nodesStruct) {
    double loss_percentage = (float)nodesStruct[NODO_TRE].total_losses / (float)nodesStruct[NODO_TRE].total_arrivals;
    printf("Loss probability: %f\n", loss_percentage);
}

// Clear the screen
void clear_screen() {
    printf("\033[H\033[2J");
}

// Open a csv file in standard mode
FILE *open_csv(char *filename) {
    FILE *file;
    file = fopen(filename, "w+");
    return file;
}

// Open a csv file in append mode
FILE *open_csv_append_mode(char *filename) {
    FILE *file;
    file = fopen(filename, "a");
    return file;
}

// Append a line in a csv file
void append_on_csv_1(FILE *file, double ts) {
    fprintf(file, "%2.6f\n", ts);
}

// Append a line in a csv file
void append_on_csv_2(FILE *file, double ts, double p) {
    fprintf(file, "%2.6f; %2.6f\n", ts, p);
}

// Append a line in a csv file
void append_on_csv_3(FILE *file, int rep, double ts, double p) {
    fprintf(file, "%i; %2.6f; %2.6f\n", rep, ts, p);
}

// Add a completion in the list
int add_to_completions_list(completions_list_struct *c_list, struct completion c) {
    int i;
    int n = c_list->num_completions;

    for (i = n - 1; (i >= 0 && (c_list->list[i].value > c.value)); i--) {
        c_list->list[i + 1] = c_list->list[i];
    }

    c_list->list[i + 1] = c;
    c_list->num_completions++;

    return (n + 1);
}

// Remove a completion from the list
int remove_from_completions_list(completions_list_struct *c_list, struct completion c) {
    int n = c_list->num_completions;
    
    int pos = search_element_completions_list(c_list, 0, n-1, c);
    if (pos == -1) {
        printf("Element not found");
        return n;
    }

    for (int i = pos; i < n; i++) {
        c_list->list[i] = c_list->list[i+1];
    }

    c_list->list[n-1].value = INFINITY;
    c_list->num_completions--;

    return n-1;
}

// Search an element in the completions list
int search_element_completions_list(completions_list_struct *c_list, int low, int high, struct completion c) {
    if (high < low) {
        return -1;
    }

    int mid = (low + high) / 2;
    if (c.value == c_list->list[mid].value) {
        return mid;
    }

    if (c.value == c_list->list[mid].value) {
        return search_element_completions_list(c_list, (mid+1), high, c);
    }

    return search_element_completions_list(c_list, low, (mid-1), c);
}
