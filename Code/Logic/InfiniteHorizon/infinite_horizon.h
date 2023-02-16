// Export methods to run an infinite horizon simulation
void infinite_horizon_simulation(int timeSlot);
void infinite_horizon_batch(int timeSlot, int k, int b);
void compute_statistics_infinite_horizon(int batch);
void compute_utilization_infinite_horizon(int batch);
void print_csv_rt_infinite_horizon(int slot);
void print_csv_p_loss_infinite_horizon(int slot);
void print_results_infinite_horizon(int slot);
double find_lambda(int timeSlot);