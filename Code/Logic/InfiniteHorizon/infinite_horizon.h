// Export methods to run an infinite horizon simulation
void infinite_horizon_simulation(int timeSlot);
void infinite_horizon_batch(int timeSlot, int k);
void compute_statistics_infinite_horizon(network_struct *networkStruct, struct node nodesStruct[], double current_clock, double rt_arr[], int pos, double dl_arr[][NUM_NODES]);
void compute_utilization_infinite_horizon(int k);
void print_csv_rt_infinite_horizon(int slot);
void print_results_infinite_horizon(int slot);