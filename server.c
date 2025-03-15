#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

typedef struct {
    char description[100];
    double price;
    int item_count;
    int aithmata;
    int temaxia_sell;
} product;

void init_catalog(product catalog[]) {
    for(int i=0; i<20; i++) {
        sprintf(catalog[i].description, "Product(%d)", i+1);
        catalog[i].price = i*3;
        catalog[i].item_count = 2;
        catalog[i].aithmata = 0;
        catalog[i].temaxia_sell = 0;
    }
}

void parent_orders(product catalog[], int p_socket, int *sum_parag, int *sum_succparag, int *sum_failparag, double *sum_price) {
    int counter = 0;
    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, "server_socket");

    if(p_socket < 0) {
        perror("socket");
        exit(1);
    }

    unlink("server_socket");

    if(bind(p_socket, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("bind");
        close(p_socket);
        exit(1);
    }

    if(listen(p_socket, 5) < 0) {
        perror("listen");
        close(p_socket);
        exit(1);
    }

    while(counter < 50) {
        char buff[100];
        int arithmos_prod;
        int b_read;
        int c_socket;
        struct sockaddr_un client_addr;
        socklen_t client_addr_size = sizeof(client_addr);

        c_socket = accept(p_socket, (struct sockaddr *) &client_addr, &client_addr_size);

        if(c_socket < 0) {
            perror("accept");
            continue;
        }

        b_read = read(c_socket, &arithmos_prod, sizeof(arithmos_prod));

        if(b_read <= 0) {
            printf("Client disconnected\n");
            counter = counter - 1;
            (*sum_parag) = (*sum_parag) - 1;
            close(c_socket);
            continue;
        }

        (*sum_parag) = (*sum_parag) + 1;
        catalog[arithmos_prod].aithmata++;

        if(catalog[arithmos_prod].item_count > 0) {
            (*sum_succparag) = (*sum_succparag) + 1;
            (*sum_price) = (*sum_price) + catalog[arithmos_prod].price;
            catalog[arithmos_prod].item_count--;
            catalog[arithmos_prod].temaxia_sell++;
            sprintf(buff, "Purchase complete, your total is %.2lf", catalog[arithmos_prod].price);
        } else {
            strcpy(buff, "Purchase failed, product is out of stock");
            (*sum_failparag) = (*sum_failparag) + 1;
        }

        write(c_socket, buff, sizeof(buff));
        close(c_socket);
        counter = counter + 1;
        sleep(1);
    }
}

void child_orders(int client_arithmos) {
    int i;
    int arithmos_prod;
    srand(time(NULL));

    for(i=0; i<10; i++) {
        int p_socket = socket(AF_UNIX, SOCK_STREAM, 0);

        if(p_socket < 0) {
            perror("socket");
            exit(1);
        }

        struct sockaddr_un server;
        server.sun_family = AF_UNIX;
        strcpy(server.sun_path, "server_socket");

        if(connect(p_socket, (struct sockaddr *) &server, sizeof(server)) < 0) {
            perror("connect");
            close(p_socket);
            continue;
        }

        arithmos_prod = rand() % 20;
        write(p_socket, &arithmos_prod, sizeof(arithmos_prod));

        char buff[100];
        int b_read;
        b_read = read(p_socket, buff, sizeof(buff));

        if(b_read > 0) {
            printf("Client %d: %s\n", client_arithmos, buff);
        }

        close(p_socket);
        sleep(1);
    }

    exit(0);
}

void anafora(product catalog[]) {
    int i;

    for(i=0; i<20; i++) {
        printf("\nPerigrafi Proiontos %d: %s\n", i+1, catalog[i].description);
        printf("Aithmata gia agora: %d\n", catalog[i].aithmata);
        printf("Temaxia Agorastikan: %d\n", catalog[i].temaxia_sell);
    }
}

void statistics(int sum_parag, int sum_succparag, int sum_failparag, double sum_price) {
    printf("\nSunolikos arithmos paraggeliwn: %d\n", sum_parag);
    printf("Epituxhmenes Paraggelies: %d\n", sum_succparag);
    printf("Apotuxhmenes Paraggelies: %d\n", sum_failparag);
    printf("Sunoliko kostos: %.2lf\n", sum_price);
}

int main() {
    product catalog[20];
    init_catalog(catalog);

    int i;
    int p_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    int sum_parag = 0;
    int sum_succparag = 0;
    int sum_failparag = 0;
    double sum_price = 0;

    int parpid = fork();

    if(parpid < 0) {
        perror("Error in fork\n");
        return -1;
    } else if(parpid > 0) {
        parent_orders(catalog, p_socket, &sum_parag, &sum_succparag, &sum_failparag, &sum_price);
        anafora(catalog);
        statistics(sum_parag, sum_succparag, sum_failparag, sum_price);
        exit(0);
    }

    printf("Program starting in 3 seconds...\n\n");

    for(i=0; i<5; i++) {
        pid_t pid = fork();

        if(pid < 0) {
            perror("Error in fork\n");
            return -1;
        } else if(pid == 0) {
            sleep(3);
            child_orders(i+1);
            exit(0);
        }
    }

    for(i=0; i<6; i++) {
        wait(NULL);
    }

    unlink("server_socket");

    return 0;
}