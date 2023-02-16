#include "../config.h"

#include "utils.h"

// Compute the minimum between two double
double min(double x, double y) {
    return (x < y) ? x : y;
}

// Compute routing from REQUEST_ACCEPTANCE
int routing() {

    // Generate a random number with Uniform distribution between 0 and 100
    double random = Uniform(0, 100);

    // Exit the network
    if (random < P_EXIT_FROM_ACCEPTANCE) {
        return EXIT;
    }

    // Route to SUBSCRIBER_REQUEST_PROCESSING
    else if (random < P_EXIT_FROM_ACCEPTANCE + P_REQUEST_SUBSCRIBER) {
        return SUBSCRIBER_REQUEST_PROCESSING;
    }

    // Route to NON_SUBSCRIBER_REQUEST_PROCESSING
    else {
        return NON_SUBSCRIBER_REQUEST_PROCESSING;
    }
}

// Find the destination node of the job based on the current node
int find_destination_node(enum node_type type) {
    switch (type) {
        case REQUEST_ACCEPTANCE:
            return routing();                   // Compute the routing
        case SUBSCRIBER_REQUEST_PROCESSING:
        case NON_SUBSCRIBER_REQUEST_PROCESSING:
            return RESPONSE_VALIDATION;         // Route to RESPONSE_VALIDATION
        default:
            return 0;
    }
}

// Return the string name of the node type specified as parameter
char *string_node_type(enum node_type type) {
    char *strings[] = {"REQUEST_ACCEPTANCE", "SUBSCRIBER_REQUEST_PROCESSING", "NON_SUBSCRIBER_REQUEST_PROCESSING", "RESPONSE_VALIDATION"};
    return strings[type];
}

// Print a separation line
void print_line() {
    printf("\n----------------------------------------------------------------------------------------------\n");
}

// Print simulation configuration 
void print_configuration() {
    for (int slot = 0; slot < 3; slot++) {
        printf("\nSlot %d\n", slot);
        for (int node = 0; node < NUM_NODES; node++) {
            printf("> %s: %d\n", string_node_type(node), config.slot_config[slot][node]);
        }
    }
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
void append_on_csv_1(FILE *file, double val) {
    fprintf(file, "%2.6f\n", val);
}

// Append a line in a csv file
void append_on_csv_2(FILE *file, int rep, double val) {
    fprintf(file, "%i; %2.6f\n", rep, val);
}

// Append a line in a csv file
void append_on_csv_3(FILE *file, double val, double clock_val) {
    fprintf(file, "%2.6f; %2.6f\n", val, clock_val);
}

// Add a completion to the list
int add_to_completions_list(struct completion c) {

    // Get number of completions in the list
    int n = completions_list.num_completions;
    int i;

    // Add the element to the completions list
    for (i = n - 1; (i >= 0 && (completions_list.list[i].value > c.value)); i--) {
        completions_list.list[i + 1] = completions_list.list[i];
    }
    completions_list.list[i + 1] = c;
    completions_list.num_completions++;

    return (n + 1);
}

// Remove a completion from the list
int remove_from_completions_list(struct completion c) {

    // Get number of completions
    int n = completions_list.num_completions;

    // Find index of the completion in the list
    int pos = search_element_completions_list(c);
    //int pos = search_test(0, n-1, c);
    if (pos == -1) {
        //printf("Element not found");
        return n;
    }

    // Update completions list
    for (int i = pos; i < n; i++) {
        completions_list.list[i] = completions_list.list[i+1];
    }
    completions_list.list[n-1].value = INFINITY;
    completions_list.num_completions--;

    return n-1;
}

// Search the index of an element in the completions list
int search_element_completions_list(struct completion c) {

    // Get length of the list
    int len = completions_list.num_completions - 1;

    // If num_completions is 0 return -1
    if (len == -1) {
        return -1;
    }

    // If num_completions is 1 return 0
    if (len == 0) {
        return 0;
    }

    // Else find the element index in the list
    for (int i = 0; i < len; i++) {
        if (completions_list.list[i].value == c.value) {
            return i;
        }
    }

    return -1;
}
