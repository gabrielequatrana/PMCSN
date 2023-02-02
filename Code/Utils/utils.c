#include "../config.h"

#include "utils.h"

// Compute the minimum between two double
double min(double x, double y) {
    return (x < y) ? x : y;
}

// Define the routing from NODE_UNO to NODE_DUE and NODE_TRE
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

    for (int j = (int)percentage / 2; j < 50 - 1; j++) {
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
    
    int pos = search_element_completions_list(c_list, n-1, c);
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

// Search the index of an element in the completions list
int search_element_completions_list(completions_list_struct *c_list, int len, struct completion c) {
    for (int i = 0; i < len; i++) {
        if (c_list->list[i].value == c.value) {
            return i;
        }
    }

    return -1;
}

void print_statistics(network_struct *networkStruct, struct node nodesStruct[], double currentClock) {
    char type[20];
    double system_total_wait = 0;
    for (int i = 0; i < NUM_NODES; i++) {
        strcpy(type, string_node_type(nodesStruct[i].type));

        int m = networkStruct->online_servers[i];
        int arr = nodesStruct[i].total_arrivals;
        int r_arr = arr - nodesStruct[i].total_losses;
        int jq = nodesStruct[i].queue_jobs;
        double inter = currentClock / nodesStruct[i].total_arrivals;

        double wait = nodesStruct[i].time.node / arr;
        double delay = nodesStruct[i].time.queue / r_arr;
        double service = nodesStruct[i].time.server / r_arr;

        system_total_wait += wait;

        printf("\n\n======== Result for block %s ========\n", type);
        printf("Number of Servers ................... = %d\n", m);
        printf("Arrivals ............................ = %d\n", arr);
        printf("Completions.......................... = %d\n", nodesStruct[i].total_completions);
        printf("Job in Queue at the end ............. = %d\n", jq);
        printf("Average interarrivals................ = %6.6f\n", inter);

        printf("Average wait ........................ = %6.6f\n", wait);
        if (i == NODO_QUATTRO) {
            printf("Average wait (2)..................... = %6.6f\n", nodesStruct[i].time.node / nodesStruct[i].total_arrivals);
            printf("Number bypassed ..................... = %d\n", nodesStruct[i].total_losses);
        }
        printf("Average delay ....................... = %6.6f\n", delay);
        printf("Average service time ................ = %6.6f\n", service);

        printf("Average # in the queue .............. = %6.6f\n", nodesStruct[i].time.queue / currentClock);
        printf("Average # in the node ............... = %6.6f\n", nodesStruct[i].time.node / currentClock);

        printf("\n    server     utilization     avg service\n");
        double p = 0;
        int n = 0;
        for (int j = 0; j < networkStruct->online_servers[i]; j++) {
            server s = networkStruct->server_list[i][j];
            printf("%8d %15.5f %15.2f\n", s.id, (s.service.service_time / currentClock), (s.service.service_time / s.service.job_served));
            p += s.service.service_time / currentClock;
            n++;
        }
    }
}
