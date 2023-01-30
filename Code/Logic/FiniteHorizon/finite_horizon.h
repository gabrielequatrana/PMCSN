// Export methods to run a finite horizon simulation
void finite_horizon_simulation(int reps);
void finite_horizon_run(int rep);
void compute_statistics_finite_horizon(network_struct *networkStruct, struct node nodesStruct[], double current_clock, double rt_arr[][3], int rep);
void print_csv_rt_finite_horizon();
void print_csv_utilization_finite_horizon(network_struct *networkStruct, double current_clock, int slot);
void print_result_finite_horizon();
void set_time_slot(int rep);