// Code for server-client.c
//Both server and client are in the same file
//The server is the parent process and the client is the child process
//The server is responsible for handling the orders and the client is responsible for making the orders

//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

//Struct for the product
typedef struct {
    char description[100];
    double price;
    int item_count;
    int aithmata;
    int temaxia_sell;
} product;

//Function for initializing the catalog
void init_catalog(product catalog[]) {
    for(int i=0; i<20; i++) {
        sprintf(catalog[i].description, "Product(%d)", i+1);
        catalog[i].price = i*3;
        catalog[i].item_count = 2;
        catalog[i].aithmata = 0;
        catalog[i].temaxia_sell = 0;
    }
}

//Function for handling the parent orders
void parent_orders(product catalog[], int p_socket, int *sum_parag, int *sum_succparag, int *sum_failparag, double *sum_price) {
    int counter = 0;
    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, "server_socket");

    //Creating the socket
    if(p_socket < 0) {
        perror("socket");
        exit(1);
    }
    //Unlinking the socket if it already exists
    unlink("server_socket");

    //Bind the socket to the server
    if(bind(p_socket, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("bind");
        close(p_socket);
        exit(1);
    }
    //Listening for incoming connections
    if(listen(p_socket, 5) < 0) {
        perror("listen");
        close(p_socket);
        exit(1);
    }
    //Accepting incoming connections and handling the orders (50 orders)
    while(counter < 50) {
        char buff[100];
        int arithmos_prod;
        int b_read;
        int c_socket;
        struct sockaddr_un client_addr;
        socklen_t client_addr_size = sizeof(client_addr);

        //Accepting the connection from the client
        c_socket = accept(p_socket, (struct sockaddr *) &client_addr, &client_addr_size);
        
        //Checking if the connection was successful
        if(c_socket < 0) {
            perror("accept");
            continue;
        }
        //Reading the product number from the client
        b_read = read(c_socket, &arithmos_prod, sizeof(arithmos_prod));
        
        //Checking if the read was successful
        if(b_read <= 0) {
            printf("Client disconnected\n");
            counter = counter - 1;
            (*sum_parag) = (*sum_parag) - 1;
            close(c_socket);
            continue;
        }

        (*sum_parag) = (*sum_parag) + 1;
        catalog[arithmos_prod].aithmata++;

        //Checking if the product is in stock
        if(catalog[arithmos_prod].item_count > 0) {    
            (*sum_succparag) = (*sum_succparag) + 1;  
            (*sum_price) = (*sum_price) + catalog[arithmos_prod].price;
            catalog[arithmos_prod].item_count--;
            catalog[arithmos_prod].temaxia_sell++;
            //Creating the message for the client about the order
            sprintf(buff, "Purchase complete, your total is %.2lf", catalog[arithmos_prod].price);   
        } else {
            strcpy(buff, "Purchase failed, product is out of stock");
            (*sum_failparag) = (*sum_failparag) + 1;
        }

        //Sending the message to the client updating them on the order 
        write(c_socket, buff, sizeof(buff));
        close(c_socket);  //Closing the connection
        counter = counter + 1;   //Incrementing the counter
        sleep(1);    //Sleeping for 1 second before accepting the next connection
    }
}

//Function for executing the child orders
void child_orders(int client_arithmos) {
    int i;
    int arithmos_prod;
    srand(time(NULL));   //Seeding the random number generator

    //Creating 10 orders
    for(i=0; i<10; i++) {
        int p_socket = socket(AF_UNIX, SOCK_STREAM, 0);  //Creating the socket

        //Checking if the socket was created successfully
        if(p_socket < 0) {
            perror("socket");
            exit(1);
        }
        //Creating the server address
        struct sockaddr_un server;
        server.sun_family = AF_UNIX;
        strcpy(server.sun_path, "server_socket");

        //Checking if the connection was successful
        if(connect(p_socket, (struct sockaddr *) &server, sizeof(server)) < 0) {
            perror("connect");
            close(p_socket);
            continue;
        }
        //Generating a random product number and sending it to the server
        arithmos_prod = rand() % 20;
        write(p_socket, &arithmos_prod, sizeof(arithmos_prod));

        char buff[100];  //Buffer for reading the message from the server
        int b_read;
        b_read = read(p_socket, buff, sizeof(buff)); //Reading the message from the server

        //Checking if the read was successful
        if(b_read > 0) {
            printf("Client %d: %s\n", client_arithmos, buff);
        }

        close(p_socket);  //Closing the connection
        sleep(1);  //Sleeping for 1 second before making the next order
    }
    //Exiting the child process
    exit(0);
}

//Function for displaying the report
void report(product catalog[]) {
    int i;

    //Displaying the report for each product
    for(i=0; i<20; i++) { 
        printf("\nPerigrafi Proiontos %d: %s\n", i+1, catalog[i].description);
        printf("Aithmata gia agora: %d\n", catalog[i].aithmata);
        printf("Temaxia Agorastikan: %d\n", catalog[i].temaxia_sell);
    }
}

//Function for displaying the statistics
void statistics(int sum_parag, int sum_succparag, int sum_failparag, double sum_price) {
    printf("\nSunolikos arithmos paraggeliwn: %d\n", sum_parag);
    printf("Epituxhmenes Paraggelies: %d\n", sum_succparag);
    printf("Apotuxhmenes Paraggelies: %d\n", sum_failparag);
    printf("Sunoliko kostos: %.2lf\n", sum_price);
}

//Main function
int main() {
    product catalog[20];   
    init_catalog(catalog);   //Initializing the catalog

    int i;
    int p_socket = socket(AF_UNIX, SOCK_STREAM, 0);   //Creating the socket
    //Set the sum variables to 0
    int sum_parag = 0;
    int sum_succparag = 0;
    int sum_failparag = 0;
    double sum_price = 0;

    int parpid = fork();  //Forking the parent process

    //Checking if the fork was successful
    if(parpid < 0) {
        perror("Error in fork\n");
        return -1;
    } else if(parpid > 0) {
        //Parent process
        //Calling the parent_orders function
        parent_orders(catalog, p_socket, &sum_parag, &sum_succparag, &sum_failparag, &sum_price);
        anafora(catalog);
        statistics(sum_parag, sum_succparag, sum_failparag, sum_price);
        exit(0); //Exiting the parent process
    }

    printf("Program starting in 3 seconds...\n\n");  //Printing a message before starting the child processes

    //Creating 5 child processes
    for(i=0; i<5; i++) {
        pid_t pid = fork();

        if(pid < 0) {   //Checking if the fork was successful
            perror("Error in fork\n");
            return -1;
        //Child process
        } else if(pid == 0) {
            //Sleeping for 3 seconds before starting the child process to avoid conflicts with the parent process 
            sleep(3);
            
            child_orders(i+1); 
            //Exiting the child process
            exit(0);
        }
    }

    //Waiting for the child processes to finish
    for(i=0; i<6; i++) {
        wait(NULL);
    }
    //Unlinking the socket
    unlink("server_socket");

    //Exiting the program
    return 0;  
}
