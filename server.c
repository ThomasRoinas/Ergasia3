#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

typedef struct                //Δημιοθργία δομής struct για τα προϊόντα
{
    char description[100];    //Περιγραφή προϊόντος
    double price;             //Τιμή προϊόντος
    int item_count;           //Διαθεσιμότητα προϊόντος
    int aithmata;             //Αιτήματα αγοράς από τους πελάτες
    int temaxia_sell;         //Τεμάχια του προϊόντος που πωλήθηκαν 
} product;

void init_catalog(product catalog[])    //Συνάρτηση για την Αρχικοποίηση του πίνακα catalog
{
    for(int i=0; i<20; i++)          //Επανάληψη 20 φορές για τα 20 προϊόντα
    {
        sprintf(catalog[i].description, "Product(%d)", i+1);   //Αρχικοποίηση της περιγραφής του προϊόντος με χρήση sprintf ώστε να περάσει η τιμή  του i+1 στο "Product(%d)"
        catalog[i].price = i*3;                  //Αρχικοποίηση της τιμής του προϊόντος με την τιμή του i*3 ώστε να έχουμε διαφορετικές τιμές για κάθε προϊόν
        catalog[i].item_count = 2;               //Αρχικοποίηση της διαθεσιμότητας του προϊόντος με 2
        catalog[i].aithmata = 0;                 //Αρχικοποίηση των αιτημάτων αγοράς του προϊόντος με 0
        catalog[i].temaxia_sell = 0;             //Αρχικοποίηση των τεμαχίων που πωλήθηκαν με 0
    }
}

void parent_orders(product catalog[], int p_socket, int *sum_parag, int *sum_succparag, int *sum_failparag, double *sum_price)
{
    int counter = 0;

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, "server_socket");

    if(p_socket < 0)
    {
        perror("socket");
        exit(1);
    }

    //unlink("server_socket");

    if(bind(p_socket, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        perror("bind");
        close(p_socket);
        exit(1);
    }

    if(listen(p_socket, 5) < 0)
    {
        perror("listen");
        close(p_socket);
        exit(1);
    }

    while(counter < 50)
    {
        char buff[100];
        int arithmos_prod;
        int b_read;
        int c_socket;
        struct sockaddr_un client_addr;
        socklen_t client_addr_size = sizeof(client_addr);

        c_socket = accept(p_socket, (struct sockaddr *) &client_addr, &client_addr_size);

        if(c_socket < 0)
        {
            perror("accept");
            continue;
        }

        b_read = read(c_socket, &arithmos_prod, sizeof(arithmos_prod));

        if(b_read <= 0)
        {
            printf("Client disconnected\n");
            close(c_socket);

            continue;
        }

        (*sum_parag) = (*sum_parag) + 1;
        catalog[arithmos_prod].aithmata++;

        if(catalog[arithmos_prod].item_count > 0)
        {
            (*sum_succparag) = (*sum_succparag) + 1;
            (*sum_price) = (*sum_price) + catalog[arithmos_prod].price;
            catalog[arithmos_prod].item_count--;
            catalog[arithmos_prod].temaxia_sell++;

            sprintf(buff, "Purchase complete, your total is %.2lf", catalog[arithmos_prod].price);
        }
        else
        {
            strcpy(buff, "Purchase failed, product is out of stock");
            (*sum_failparag) = (*sum_failparag) + 1;
        }

        write(c_socket, buff, sizeof(buff));

        close(c_socket);

        counter = counter + 1;

        sleep(1);
    }
}

void child_orders(int client_arithmos)
{
    int i;

    srand(time(NULL));

    for(i=0; i<10; i++)
    {
        int p_socket = socket(AF_UNIX, SOCK_STREAM, 0);

        if(p_socket < 0)
        {
            perror("socket");
            exit(1);
        }

        struct sockaddr_un server;
        server.sun_family = AF_UNIX;
        strcpy(server.sun_path, "server_socket");

        if(connect(p_socket, (struct sockaddr *) &server, sizeof(server)) < 0)
        {
            perror("connect");
            close(p_socket);
            continue;
        }

        int arithmos_prod = rand() % 20;

        write(p_socket, &arithmos_prod, sizeof(arithmos_prod));

        char buff[1000];
        int b_read;

        b_read = read(p_socket, buff, sizeof(buff));

        if(b_read > 0)
        {
            printf("Client %d: %s\n", client_arithmos, buff);
        }

        close(p_socket);

        sleep(1);
    }

    exit(0);
}

void anafora(product catalog[])
{
    int i;

    for(i=0; i<20; i++)
    {
        printf("\nPerigrafi Proiontos %d: %s\n", i+1, catalog[i].description);  //Εμφάνιση της περιγραφής του προϊόντος
        printf("Aithmata gia agora: %d\n", catalog[i].aithmata);                //Εμφάνιση των συνολικών αιτημάτων αγοράς του προϊόντος
        printf("Temaxia Agorastikan: %d\n", catalog[i].temaxia_sell);           //Εμφάνιση του συνολικού αριθμού τεμαχίων που πωλήθηκαν
    }
}

void statistics(int sum_parag, int sum_succparag, int sum_failparag, double sum_price)
{
    printf("\nSunolikos arithmos paraggeliwn: %d\n", sum_parag);   //Εμφάνιση του συνολικού αριθμού των παραγγελιών που υποβλήθηκαν
    printf("Epituxhmenes Paraggelies: %d\n", sum_succparag);       //Εμφάνιση του συνολικού αριθμού των επιτυχημένων παραγγελιών που υποβλήθηκαν
    printf("Apotuxhmenes Paraggelies: %d\n", sum_failparag);       //Εμφάνιση του συνολικού αριθμού των αποτυχημένων παραγγελιών που υποβλήθηκαν
    printf("Sunoliko kostos: %.2lf\n", sum_price);                 //Εμφάνιση του συνολικού κόστους των παραγγελιών που υποβλήθηκαν με ακρίβεια 2 δεκαδικά ψηφίά
}

int main()
{
    product catalog[20];
    init_catalog(catalog);

    int i;

    int p_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    int sum_parag = 0;
    int sum_succparag = 0;
    int sum_failparag = 0;
    double sum_price = 0;

    int parpid = fork();

    if(parpid < 0)
    {
        perror("Error in fork\n");
        return -1;
    }

    else if(parpid > 0)
    {
        parent_orders(catalog, p_socket, &sum_parag, &sum_succparag, &sum_failparag, &sum_price);

        anafora(catalog);
        statistics(sum_parag, sum_succparag, sum_failparag, sum_price);

        exit(0);
    }
    
    for(i=0; i<5; i++)       
    {
        pid_t pid = fork();     

        if(pid < 0)         
        {
            perror("Error in fork\n");   
            return -1;
        }

        else if(pid == 0)    
        {                   
            child_orders(i+1);  
            exit(0);
        }
    }      
    
    for(i=0; i<5; i++)
    {
        wait(NULL);
    }

    return 0;
}