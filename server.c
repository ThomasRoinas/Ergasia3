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

void parent_orders(product catalog[], int c_socket)
{
    
}