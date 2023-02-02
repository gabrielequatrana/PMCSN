#include "./../config.h"

// Export general logic methods used by finite/infinite horizon simulations
void clear_environment();
void init_network();

void process_arrival();
void process_completion(struct completion c);

void update_server_online_time();

double compute_cost(network_struct *net);

void reset_nodes_parameters();

void update_network();




