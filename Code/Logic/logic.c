#include "./../config.h"
#include "./../Utils/utils.h"

#include "./FiniteHorizon/finite_horizon.h"

#include "logic.h"
#include "var.h"

// Reset the parameters of the network nodes between batches
void reset_nodes_parameters() {
    clock.current_batch = clock.current;
    for (int i = 0; i < NUM_NODES; i++) {
        nodes[i].total_arrivals = 0;
        nodes[i].total_completions = 0;
        nodes[i].total_losses = 0;
        nodes[i].total_dropped = 0;
        nodes[i].time.node = 0;
        nodes[i].time.queue = 0;
        nodes[i].time.server = 0;
    }
}

// Initialize the parameters of the network nodes
void init_nodes() {

    // Repeat for every node
    for (int i = 0; i < NUM_NODES; i++) {

        // Initialize server parameters
        nodes[i].type = i;
        nodes[i].node_jobs = 0;
        nodes[i].queue_jobs = 0;
        nodes[i].total_arrivals = 0;
        nodes[i].total_completions = 0;
        nodes[i].total_losses = 0;
        nodes[i].total_dropped = 0;
        nodes[i].time.node = 0;
        nodes[i].time.queue = 0;
        nodes[i].time.server = 0;

        // Repeat for every node server
        for (int j = 0; j < MAX_SERVERS; j++) {

            // Initialize a node server
            server s;
            s.id = j;
            s.status = IDLE;
            s.online = DEACTIVATED;
            s.used = NOT_USED;
            s.stream = streamID++;
            s.last_time_online = 0.0;
            s.online_time = 0.0;
            s.node = &nodes[i];
            s.service.service_time = 0.0;
            s.service.job_served = 0;
            s.stop_after_completion = false;

            network.server_list[i][j] = s;

            struct completion c = {&network.server_list[i][j], INFINITY};
            add_to_completions_list(&completions_list, c);
        }
    }
}

// Generate an exponential arrival time
double generate_arrival_time(double current) {

    // Select the stream for the rng
    SelectStream(254);

    // Compute arrival time
    double arrival = current;
    arrival += Exponential(1 / arrival_rate);

    // Return arrival time
    return arrival;
}

// Initialize the network 
void init_network() {
    
    // Initialize some parameters
    network.config = &config;
    streamID = 0;
    clock.current = START;
    for (int i = 0; i < 3; i++) {
        slot_switched[i] = false;
    }

    // Initialize nodes
    init_nodes();

    // Set the current time slot in finite horizon simulation
    if (strcmp(simulation_mode, "FINITE") == 0) {
        set_time_slot(0);
    }

    // Initialize some parameters
    job_completed = 0;
    job_dropped = 0;
    job_lost = 0;
    clock.arrival = generate_arrival_time(clock.current);
    completions_list.num_completions = 0;
}

// Clear execution environment
void clear_environment() {

    // Reset network and completions list
    network = empty_network;
    completions_list = empty_completions_list;
    
    // Reset node times
    for (int i = 0; i < NUM_NODES; i++) {
        nodes[i].time.queue = 0;
        nodes[i].time.server = 0;
        nodes[i].time.node = 0;   
    }
}

// Generate a random exponential service time
double generate_service_time(enum node_type type, int stream) {

    // Select the stream for the rng
    SelectStream(stream);

    // Generate the service time based on the node
    switch (type) {
        case NODO_UNO:
            return Exponential(SERV_TIPOUNO);
        case NODO_DUE:
            return Exponential(SERV_TIPODUE);
        case NODO_TRE:
            return Exponential(SERV_TIPOTRE);
        case NODO_QUATTRO:
            return Exponential(SERV_TIPOQUATTRO);
        default:
            return 0;
    }
}

// Activate servers of a node based on its configuration
void activate_servers(int node) {
    int timeSlot = network.time_slot;
    int start = network.online_servers[node];

    // Repeat for each server to activate
    for (int i = start; i < config.slot_config[timeSlot][node]; i++) {

        // Server to activate
        server *s = &network.server_list[node][i];
        s->online = ACTIVATED;
        s->last_time_online = clock.current;
        s->used = USED;

        // If there are jobs in the queue, the server can process one
        if (nodes[node].queue_jobs > 0) {

            // Update the first job in the queue
            if (nodes[node].head_queue->next != NULL) {
                struct job *temp = nodes[node].head_queue->next;
                nodes[node].head_queue = temp;
            }
            else {
                nodes[node].head_queue = NULL;
            }

            // Process the job
            double service_time = generate_service_time(node, s->stream);
            struct completion c = {s, INFINITY};
            s->status = BUSY;
            c.value = clock.current + service_time;
            nodes[node].queue_jobs--;
            s->node->time.server += service_time;
            s->service.service_time += service_time;

            add_to_completions_list(&completions_list, c);
        }

        // Update network parameters
        network.online_servers[node] = config.slot_config[timeSlot][node];
    }
}

// Deactivate servers of a node
void deactivate_servers(int node) {
    int timeSlot = network.time_slot;
    int start_server = network.online_servers[node];

    // Repeat for each server to deactivate
    for (int i = start_server - 1; i >= config.slot_config[timeSlot][node]; i--) {

        // Server to deactivate
        server *s = &network.server_list[node][i];

        // If the server is BUSY wait for completion to deactivate it
        if (s->status == BUSY) {
            s->stop_after_completion = true;
        }

        // Deactivate server
        else {
            s->online = DEACTIVATED;
            s->online_time += (clock.current - s->last_time_online);
            s->last_time_online = clock.current;
        }

        // Update network parameters
        network.online_servers[node] = config.slot_config[timeSlot][node];
    }
}

// Update network by activating/deactivating the servers
void update_network() {
    
    // Repeat for every node
    int current_servers, new_slot_servers;
    for (int node = 0; node < NUM_NODES; node++) {
        current_servers = network.online_servers[node];
        new_slot_servers = config.slot_config[network.time_slot][node];

        // Deactivate servers
        if (current_servers > new_slot_servers) {
            deactivate_servers(node);
        }

        // Activate servers
        else if (current_servers < new_slot_servers) {
            activate_servers(node);
        }
    }
}

// Update all servers online time in the end of a simulation
void update_server_online_time() {
    for (int i = 0; i < NUM_NODES; i++) {
        for (int j = 0; j < MAX_SERVERS; j++) {
            server *s = &network.server_list[i][j];
            if (s->online == ACTIVATED) {
                s->online_time += (clock.current - s->last_time_online);
                s->last_time_online = clock.current;
            }
        }
    }
}

// Find the first free server to process a job
server *find_free_server(struct node node) {
    int node_type = node.type;
    int active_servers = network.online_servers[node_type];
    for (int i = 0; i < active_servers; i++) {
        if (network.server_list[node_type][i].status == IDLE) {
            return &network.server_list[node_type][i];
        }
    }

    // Returns NULL when no free servers are found
    return NULL;
}

// Add a job to a node queue
void add_job_to_queue(struct node *node) {

    // Allocate a job
    struct job *job = (struct job *)malloc(sizeof(struct job));
    if (job == NULL ) {
        handle_error("malloc");
    }

    // Initialize the job
    job->next = NULL;

    // If the node has a last job in the queue, append the job
    if (node->tail_job) {
        node->tail_job->next = job;
    }

    // Else the job is the head of the queue
    else {
        node->head_server = job;
    }

    // Set the new tail as the job added to the queue
    node->tail_job = job;

    // If the queue is empty, the new job is the head
    if (node->head_queue == NULL) {
        node->head_queue = job;
    }
}

// Remove a job from a node queue
void remove_job_from_queue(struct node *node) {

    // The job to remove is the head of the queue
    struct job *job = node->head_server;

    // If it is the only job in the queue, the queue becomes empty
    if (!job->next) {
        node->tail_job = NULL;
    }

    // The next job becomes the head
    node->head_server = job->next;

    // If there is a job in the queue, the head of the queue became the next job
    if (node->head_queue != NULL && node->head_queue->next != NULL) {
        struct job *temp = node->head_queue->next;
        node->head_queue = temp;
    }

    // Else there are no jobs in the queue
    else {
        node->head_queue = NULL;
    }

    // Deallocate the job
    free(job);
}

// Compute costs of the network servers and the total cost of all servers
double compute_cost(network_struct *net) {
    double cm_costs[] = {COST_TIPOUNO, COST_TIPODUE, COST_TIPOTRE, COST_TIPOQUATTRO};
    double costs[4] = {0, 0, 0, 0};
    double total_cost = 0;

    // Compute cost for each server of the network
    for (int i = 0; i < NUM_NODES; i++) {
        for (int j = 0; j < MAX_SERVERS; j++) {
            server s = net->server_list[i][j];
            costs[i] += s.online_time * (cm_costs[i] / SECOND_IN_A_MONTH);
        }
        total_cost += costs[i];
    }

    // Returns total cost of the servers
    return total_cost;
}

// Process an arrival
void process_arrival() {

    // Add the arrival in the first node
    nodes[NODO_UNO].total_arrivals++;
    nodes[NODO_UNO].node_jobs++;

    // Find a free server in the node one to process the job
    server *s = find_free_server(nodes[NODO_UNO]);

    // If there is a free server, the queue is empty
    if (s != NULL) {

        // Process the job
        double service_time = generate_service_time(NODO_UNO, s->stream);
        struct completion completion = {s, INFINITY};
        completion.value = clock.current + service_time;
        s->status = BUSY;
        s->node->time.server += service_time;
        s->service.service_time += service_time;
        s->service.job_served++;

        add_to_completions_list(&completions_list, completion);
        add_job_to_queue(&nodes[NODO_UNO]);
    }

    // Else add the job to the queue
    else {
        add_job_to_queue(&nodes[NODO_UNO]);
        nodes[NODO_UNO].queue_jobs++;
    }

    // Generate the next arrival
    clock.arrival = generate_arrival_time(clock.current);
}

// Process a completion
void process_completion(struct completion c) {

    // Add the completion to the node
    int node_type = c.server->node->type;
    nodes[node_type].total_completions++;
    nodes[node_type].node_jobs--;

    int destination;
    server *free_server;

    // Remove the job from the queue and the completion from the list
    remove_job_from_queue(&nodes[node_type]);
    remove_from_completions_list(&completions_list, c);

    // Generate the next completion if there are jobs in queue
    if (nodes[node_type].queue_jobs > 0 && !c.server->stop_after_completion) {
        nodes[node_type].queue_jobs--;
        double service = generate_service_time(node_type, c.server->stream);
        c.value = clock.current + service;
        c.server->node->time.server += service;
        c.server->service.service_time += service;
        c.server->service.job_served++;

        add_to_completions_list(&completions_list, c);
    }

    // Else the server is IDLE
    else {
        c.server->status = IDLE;
    }

    // Deactivate the server after the completion if it is flagged
    if (c.server->stop_after_completion) {
        c.server->online = DEACTIVATED;
        c.server->online_time += (clock.current - c.server->last_time_online);
        c.server->last_time_online = clock.current;
        c.server->stop_after_completion = false;
    }

    // If the last node processed a job, increment the total job completed
    if (node_type == NODO_QUATTRO) {
        job_completed++;
        return;
    }

    // Find destination node
    destination = find_destination_node(c.server->node->type);

    // If the destination is the EXIT, increment job dropped
    if (destination == EXIT) {
        nodes[node_type].total_dropped++;
        job_dropped++;
        return;
    }

    // The destination is a node with an infinite queue
    if (destination != NODO_QUATTRO) {
        nodes[destination].total_arrivals++;
        nodes[destination].node_jobs++;
        add_job_to_queue(&nodes[destination]);

        free_server = find_free_server(nodes[destination]);
        if (free_server != NULL) {
            struct completion completion = {free_server, INFINITY};
            double service = generate_service_time(destination, free_server->stream);
            completion.value = clock.current + service;
            add_to_completions_list(&completions_list, completion);
            free_server->status = BUSY;
            free_server->node->time.server += service;
            free_server->service.service_time += service;
            free_server->service.job_served++;

            return;
        }
        else {
            nodes[destination].queue_jobs++;
            return;
        }
    }

    // The destination is the node 4 with finite queue
    // Update arrivals to the destination
    nodes[destination].total_arrivals++;

    // Find a free server on the destination node
    free_server = find_free_server(nodes[destination]);

    // If there is a free server, the queue is empty
    if (free_server != NULL) {
        nodes[destination].node_jobs++;
        add_job_to_queue(&nodes[destination]);

        struct completion completion = {free_server, INFINITY};
        double service = generate_service_time(destination, free_server->stream);
        completion.value = clock.current + service;
        add_to_completions_list(&completions_list, completion);

        free_server->status = BUSY;
        free_server->node->time.server += service;
        free_server->service.service_time += service;
        free_server->service.job_served++;
        return;
    }

    // Else the job is lost
    else {
        job_completed++;
        job_lost++;
        nodes[NODO_QUATTRO].total_losses++;
        return;
    }
}