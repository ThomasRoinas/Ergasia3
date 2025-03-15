//Header file for server-client.c
// Declare the functions and structs used in server-client.c

void init_catalog(product catalog[])   
void parent_orders(product catalog[], int p_socket, int *sum_parag, int *sum_succparag, int *sum_failparag, double *sum_price)
void child_orders(int client_arithmos)
void anafora(product catalog[])
void statistics(int sum_parag, int sum_succparag, int sum_failparag, double sum_price)