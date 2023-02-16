// Export methods to run a finite horizon simulation
void finite_horizon_simulation(int reps);
void finite_horizon_run(int rep);
double compute_response_time(int timeSlot);
void compute_statistics_finite_horizon(int rep);
void compute_continuous_statistics_finite_horizon();
void print_csv_rt_finite_horizon();
void print_csv_p_loss_finite_horizon();
void print_result_finite_horizon();
void set_time_slot(int rep);