#include "./Logic/var.h"

network_configuration get_config(const int *values_1, const int *values_2, const int *values_3);

// Initialize the configuration of the network
void init_config() {

    int a[] = {8, 20, 2, 9};
    int b[] = {18, 42, 5, 22};
    int c[] = {4, 14, 1, 6};

    printf("SIUM");

    config = get_config(a, b, c);
    printf("PRTZ %i", config_ptr->slot_config[1][1]);
}

network_configuration get_config(const int values_1[NUM_NODES], const int *values_2, const int *values_3) {
    network_configuration *config_test = malloc(sizeof(network_configuration));
    for (int i = 0; i < NUM_NODES; i++) {
        config_test->slot_config[0][i] = values_1[i];
        config_test->slot_config[1][i] = values_2[i];
        config_test->slot_config[2][i] = values_3[i];
    }

    return *config_test;
}