#include "./../config.h"
#include "./../Logic/var.h"

// Method to handle errors
#define handle_error(msg)   \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while(0)

// Seconds in a month
#define SECOND_IN_A_MONTH 2052000 //(60 * 60 * 19 * 30)

// Global logic methods
int find_destination_node(enum node_type type);
char *string_node_type(enum node_type type);
double min(double x, double y);

// Graphic output methods
void print_line();
void print_configuration();

// CSV methods
FILE *open_csv(char *filename);
FILE *open_csv_append_mode(char *filename);
void append_on_csv_1(FILE *file, double val);
void append_on_csv_2(FILE *file, int rep, double val);
void append_on_csv_3(FILE *file, double val, double clock_val);

// Completions list methods
int add_to_completions_list(struct completion c);
int remove_from_completions_list(struct completion c);
int search_element_completions_list(struct completion c);