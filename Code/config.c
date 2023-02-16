#include "./Logic/var.h"

// Set the values of the configuration
network_configuration set_config(const int *values_1, const int *values_2, const int *values_3) {

    // Allocate the configuration struct
    network_configuration *config_struct = malloc(sizeof(network_configuration));

    // Set the values
    for (int i = 0; i < NUM_NODES; i++) {
        config_struct->slot_config[0][i] = values_1[i];
        config_struct->slot_config[1][i] = values_2[i];
        config_struct->slot_config[2][i] = values_3[i];
    }

    return *config_struct;
}

// Initialize the configuration of the network
void init_config() {

    // Standard Configurations
    int null[] = {0, 0, 0, 0};
    int full[] = {50, 50, 50, 50};

    // Time Slot 1
    int unstable_node_0[] = {1, 4, 17, 5};
    int unstable_node_2[] = {3, 4, 15, 5};
    int stable_node_2[] = {3, 4, 17, 5};

    int almost_best_1[] = {3, 5, 18, 6};
    int best_1[] = {2, 3, 16, 4};

    // Time Slot 2
    int almost_best_2[] = {8, 9, 47, 15};
    int best_2[] = {5, 7, 38, 9};

    // Time Slot 3
    int almost_best_3[] = {7, 7, 35, 9};
    int best_3[] = {4, 5, 29, 7};

    config = set_config(unstable_node_0, best_2, best_3);
}