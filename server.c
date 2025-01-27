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