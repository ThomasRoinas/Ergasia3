#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

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

void parent_orders(product catalog[], int c_socket, int *sum_parag, int *sum_succparag, int *sum_failparag, double *sum_price)
{
    int i;

    while(1)
    {
        char buff[100];
        int arithmos_prod;
        int bread;

        bread = read(c_socket, &arithmos_prod, sizeof(arithmos_prod));

        if(bread <= 0)
        {
            printf("Client disconnected\n");
            close(c_socket);

            return;
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
            write(c_socket, buff, sizeof(buff));
        }
        else
        {
            (*sum_failparag) = (*sum_failparag) + 1;
            write(c_socket, "Products unavailable, request failed", sizeof("Products unavailable, request failed"));
        }

        sleep(1);
    }
}

void child_orders(int c_socket)
{
    int i;
    int arithmos_prod;

    srand(time(NULL));

    for(i=0; i<10; i++)
    {
        arithmos_prod = rand() % 20;

        write(c_socket, &arithmos_prod, sizeof(arithmos_prod));

        char buff[100];
        int bread;

        bread = read(c_socket, buff, sizeof(buff));

        printf("Client %d: %s\n", c_socket, buff);

        sleep(1);
    }

    close(c_socket);

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
    int c_socket;
    int p_socket;

    int sum_parag = 0;
    int sum_succparag = 0;
    int sum_failparag = 0;
    int sum_price = 0;

    struct sockaddr_un server;

    if((p_socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, "server_socket");

    if(bind(p_socket, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        perror("bind");
        exit(1);
    }
    
    if(listen(p_socket, 5) < 0)
    {
        perror("listen");
        exit(1);
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
            if((c_socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
            {
                perror("socket");
                exit(1);
            }

            if(connect(c_socket, (struct sockaddr *) &server, sizeof(server)) < 0)
            {
                perror("connect");
                exit(1);
            }

            child_orders( i+1);  
        }

        else
        {
            while(1)
            {
                c_socket = accept(p_socket, NULL, NULL);

                if(c_socket < 0)
                {
                    perror("accept");
                    exit(1);
                }

                parent_orders(catalog, c_socket, &sum_parag, &sum_succparag, &sum_failparag, &sum_price);
            }
        }                                
    }

}