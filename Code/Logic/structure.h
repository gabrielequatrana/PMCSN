#include "./../config.h"

#ifndef GRANDPARENT_H
#define GRANDPARENT_H
// Enumerate node types
enum node_type {
    NODO_UNO,
    NODO_DUE,
    NODO_TRE,
    NODO_QUATTRO,
    EXIT
};

// Structure to implement the clock
struct clock_struct {
    double current;         // Current simulation time
    double next;            // Time of the next event (arrival/completion)
    double arrival;         // Time of next arrival
    double current_batch;   // Current batch in infinite horizon simulation
};

// Structure to keep track of service time of a server
struct service_time {
    double service_time;    // Sum of service time
    long job_served;        // Number of job served
};

// Structure to implement a job with a linked list
struct job {
    double arrival;     // Job arrival index
    struct job* next;   // Next job to process
};

// Structure to implement a server
typedef struct {
    int id;                         // Server ID
    int online;                     // Server activated/deactivated
    int status;                     // Server busy/idle
    int used;                       // Server used/not used
    int stream;                     // Server stream used for rngs

    struct node *node;              // Server node
    struct service_time service;    // Server service time

    double online_time;             // Server online time
    double last_time_online;        // Server last time online
} server;

// Structure to implement the network configuration
typedef struct {
    int slot_config[NUM_SLOTS][NUM_NODES];  // Configurations of every node for each time slot
} network_configuration;

// Structure to implement the queues network
typedef struct {
    int time_slot;                              // Current time slot of the network
    int online_servers[NUM_NODES];              // Activated server for each node of the network

    server server_list[NUM_NODES][MAX_SERVERS]; // Server list for each node of the network
    network_configuration *config;              // Network configuration
} network_struct;

// Structure to implement node times
struct time {
    double node;    // Node response time
    double queue;   // Queue time
    double server;  // Service time
};

// Structure to implement a completion on a server
struct completion {
    server *server; // Server that completed a job
    double value;   // TODO
};

// Structure to implement a completions list
typedef struct {
    struct completion list[NUM_NODES * MAX_SERVERS];    // List of completions
    int num_completions;                                // Number of completions
} completions_list_struct;

// Structure to implement a node
struct node {
    enum node_type type;        // Node type

    double active_time;         // Node active time
    double service_rate;        // Node service rate

    struct job *tail_job;       // TODO
    struct job *head_server;    // TODO
    struct job *head_queue;     // TODO

    int queue_jobs;             // Number of job in the queue
    int node_jobs;              // Number of job in the node

    int batch_node;             // TODO
    int batch_queue;            // TODO
    int batch_arrivals;         // TODO
    
    int total_arrivals;         // Number of job arrived
    int total_completions;      // Number of job completed
    int total_losses;           // Number of job lost
    int total_dropped;          // Number of job dropped

    struct time time;           // Node times
};

#endif