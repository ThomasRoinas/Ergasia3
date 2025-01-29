#include <stdio.h>          //Πηγαίος Κώδικας
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

//Δημιουργία συναρτήσεων για την εκτέλεση των διεργασιών

//Συνάρτηση για την διαχείρηση των υποβληθέντων παραγγελιών από τους πελάτες
void parent_orders(product catalog[], int p_socket, int *sum_parag, int *sum_succparag, int *sum_failparag, double *sum_price)
{                                                   //Χρήση δεικτών για την μεταφορά των τιμών των μεταβλητών για χρήση τους στην main συνάρτηση
    int counter = 0;   //Δήλωση μεταβλητής ακεραίου για τον μετρητή των παραγγελιών

    struct sockaddr_un server;    //Δήλωση δομής struct sockaddr_un για τον server
    server.sun_family = AF_UNIX;    //Αρχικοποίηση του πεδίου sun_family της δομής struct sockaddr_un για την χρήση του πρωτοκόλλου AF_UNIX
    strcpy(server.sun_path, "server_socket");    //Αντιγραφή του server_socket στο πεδίο sun_path της δομής struct sockaddr_un

    if(p_socket < 0)   //Δημιουργία socket (parent socket) για την επικοινωνία
    {
        perror("socket");
        exit(1);
    }

    unlink("server_socket");   //Διαγραφή του server_socket
 
    if(bind(p_socket, (struct sockaddr *) &server, sizeof(server)) < 0)    //Συσχέτιση του socket με την τοπική διεύθυνση του server για επικοινωνία
    {
        perror("bind");
        close(p_socket);   //Κλείσιμο του socket (parent socket) για την αποφυγή διαρροής μνήμης
        exit(1);
    }

    if(listen(p_socket, 5) < 0)    //Αναμονή για αίτημα σύνδεσης από τον πελάτη με μέγιστο αριθμό συνδέσεων 5
    {
        perror("listen");
        close(p_socket);   //Κλείσιμο του socket (parent socket) για την αποφυγή διαρροής μνήμης
        exit(1);
    }

    while(counter < 50)   //Επανάληψη έως ότου ο μετρητής των παραγγελιών φτάσει το 50, δηλαδή να έχουν εξυπηρετηθεί 50 πααγγελίες, για την αποφυγή επανάληψης της διαδικασίας όταν π.χ προκύψει σφάλμα σε κάποιο socket
    {
        char buff[100];     //Δήλωση πίνακα χαρακτήρων 100 θέσεων για την αποθήκευση του μηνύματος για το αποτέλεσμα της παραγγελίας που θα σταλεί στον πελάτη
        int arithmos_prod;    //Δήλωση μεταβλητής ακεραίου για την αποθήκευση του αριθμού του προϊόντος που επέλεξε ο πελάτης

        int b_read;                 //Δήλωση μεταβλητής για την αποθήκευση του αριθμού των bytes που διαβάστηκαν από το socket
        int c_socket;               //Δήλωση μεταβλητής για το socket του πελάτη (client socket)
        struct sockaddr_un client_addr;   //Δήλωση δομής struct sockaddr_un για τον πελάτη
        socklen_t client_addr_size = sizeof(client_addr);    //Δήλωση μεταβλητής τύπου socklen_t για το μέγεθος της δομής struct sockaddr_un του πελάτη

        c_socket = accept(p_socket, (struct sockaddr *) &client_addr, &client_addr_size);    //Εξυπηρέτηση του αιτήματος σύνδεσης από τον πελάτη

        if(c_socket < 0)   //Έλεγχος για την επιτυχία της εξυπηρέτησης του αιτήματος σύνδεσης από τον πελάτη
        {
            perror("accept");
            continue;        //Συνέχιση της επανάληψης σε περίπτωση που δεν εξυπηρετηθεί το αίτημα σύνδεσης
        }

        b_read = read(c_socket, &arithmos_prod, sizeof(arithmos_prod));    //Ανάγνωση του αριθμού του προϊόντος που επέλεξε ο πελάτης μέσω του socket του πελάτη

        if(b_read <= 0)    //Έλεγχος για την επιτυχία της ανάγνωσης του αριθμού του προϊόντος που επέλεξε ο πελάτης
        {
            printf("Client disconnected\n");
            counter = counter - 1;            //Μείωση κατά 1 του μετρητή των παραγγελιών σε περίπτωση που προκύψει κάποιο σφάλμα στην επικοινωνία με τον πελάτη, για την σωστή επανάληψη της διαδικασίας για τον αριθμό των παραγγελιών (50)
            (*sum_parag) = (*sum_parag) - 1;    //Μείωση κατά 1 του συνολικού αριθμού των παραγγελιών σε περίπτωση που προκύψει κάποιο σφάλμα στην επικοινωνία με τον πελάτη
            close(c_socket);    //Κλείσιμο του socket του πελάτη σε περίπτωση που δεν είναι δυνατή η ανάγνωση του αριθμού του προϊόντος που επέλεξε ο πελάτης

            continue;     //Συνέχιση της επανάληψης σε περίπτωση που δεν είναι δυνατή η ανάγνωση του αριθμού του προϊόντος που επέλεξε ο πελάτης
        }

        (*sum_parag) = (*sum_parag) + 1;    //Αύξηση κατά 1 του συνολικού αριθμού των παραγγελιών
        catalog[arithmos_prod].aithmata++;    //Αύξηση κατά 1 των αιτημάτων αγοράς του προϊόντος που επέλεξε ο πελάτης

        if(catalog[arithmos_prod].item_count > 0)   //Έλεγχος για αν το προϊόν είναι διαθέσιμο
        {
            (*sum_succparag) = (*sum_succparag) + 1;        //Αύξηση κατά 1 των επιτυχημένων παραγγελιών
            (*sum_price) = (*sum_price) + catalog[arithmos_prod].price;      //Αύξηση του συνολικού κόστους των παραγγελιών προσθέτοντας την τιμή του προϊόντος
            catalog[arithmos_prod].item_count--;                             //Μείωση της διαθεσιμότητας του προϊόντος κατά 1
            catalog[arithmos_prod].temaxia_sell++;                           //Αύξηση κατά 1 των τεμαχίων που πωλήθηκαν                                

            sprintf(buff, "Purchase complete, your total is %.2lf", catalog[arithmos_prod].price);      //Αποθήκευση του μηνύματος για το αποτέλεσμα της παραγγελίας στον πίνακα buff, χρησιμοποιώντας την sprintf για την εισαγωγή της τιμής του προϊόντος με ακρίβεια 2 δεκαδικών ψηφίων
        }
        else     //Περίπτωση που το προϊόν δεν είναι διαθέσιμο
        {
            strcpy(buff, "Purchase failed, product is out of stock");    //Αποθήκευση του μηνύματος για το αποτέλεσμα της παραγγελίας στον πίνακα buff
            (*sum_failparag) = (*sum_failparag) + 1;    //Αύξηση κατά 1 των αποτυχημένων παραγγελιών
        }

        write(c_socket, buff, sizeof(buff));    //Αποστολή του μηνύματος για το αποτέλεσμα της παραγγελίας στον πελάτη μέσω του socket του πελάτη, με τη χρήση του συγγραφέα (write)

        close(c_socket);    //Κλείσιμο του socket του πελάτη (client socket) για την αποφυγή διαρροής μνήμης

        counter = counter + 1;    //Αύξηση κατά 1 του μετρητή των παραγγελιών

        sleep(1);     //Χρόνος διεκπεραίωσης της παραγγελίας 1 δευτερόλεπτο
    }
}

//Συνάρτηση για την υποβολή αιτημάτων αγοράς προϊόντων από τους πελάτες
void child_orders(int client_arithmos)
{
    int i;
    int arithmos_prod;     //Δήλωση μεταβλητής ακεραίου για την αποθήκευση του αριθμού του προϊόντος που επέλεξε ο πελάτης
    
    srand(time(NULL));

    for(i=0; i<10; i++)    //Επανάληψη 10 φορές για την υποβολή 10 παραγγελιών από τον πελάτη
    {
        int p_socket = socket(AF_UNIX, SOCK_STREAM, 0);    //Δημιουργία socket (parent socket) για την επικοινωνία με τον server

        if(p_socket < 0)   //Έλεγχος για την επιτυχία της δημιουργίας του socket (parent socket)
        {
            perror("socket");
            exit(1);
        }

        struct sockaddr_un server;   //Δήλωση δομής struct sockaddr_un για τον server
        server.sun_family = AF_UNIX;   //Αρχικοποίηση του πεδίου sun_family της δομής struct sockaddr_un για την χρήση του πρωτοκόλλου AF_UNIX
        strcpy(server.sun_path, "server_socket");   //Αντιγραφή του server_socket στο πεδίο sun_path της δομής struct sockaddr_un

        if(connect(p_socket, (struct sockaddr *) &server, sizeof(server)) < 0)   //Υποβολή αιτήματος σύνδεσης από τον πελάτη στον server
        {
            perror("connect");
            close(p_socket);   //Κλείσιμο του socket για την αποφυγή διαρροής μνήμης
            continue;          //Συνέχιση της επανάληψης σε περίπτωση που δεν είναι δυνατή η σύνδεση με τον server
        }
        
        arithmos_prod = rand() % 20;    //Παραγωγή τυχαίου αριθμού από το 0 έως το 19 για την επιλογή του προϊόντος που θα αγοράσει ο πελάτης

        write(p_socket, &arithmos_prod, sizeof(arithmos_prod));    //Αποστολή του αριθμού του προϊόντος που επέλεξε ο πελάτης στον server μέσω του parent socket

        char buff[100];    //Δήλωση πίνακα χαρακτήρων 100 θέσεων για την αποθήκευση του μηνύματος για το αποτέλεσμα της παραγγελίας που στλενεται από το κατάστημα στον πελάτη
        int b_read;        //Δήλωση μεταβλητής για τον αριθμό των bytes που διαβάστηκαν από το socket

        b_read = read(p_socket, buff, sizeof(buff));   //Ανάγνωση του μηνύματος για το αποτέλεσμα της παραγγελίας από τον server μέσω του socket

        if(b_read > 0)   //Έλεγχος για την επιτυχία της ανάγνωσης του μηνύματος για το αποτέλεσμα της παραγγελίας
        {
            printf("Client %d: %s\n", client_arithmos, buff);    //Εμφάνιση μηνύματος για το αποτέλεσμα της παραγγελίας του πελάτη
        }

        close(p_socket);   //Κλείσιμο του parent socket για την αποφυγή διαρροής μνήμης

        sleep(1);      //Αναμονή 1 δευτερόλεπτο ανάμεσα στις παραγγελίες των πελατών
    }

    exit(0);     //Εξοδος της θυγατρικής διεργασίας
}

//Συνάρτηση για την εξαγωγή συγκεντρωτικής αναφοράς για κάθε προϊόν
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

//Συνάρτηση για την τύπωση συγκεντρωτικού μηνύματος με τα στατιστικά των παραγγελιών
void statistics(int sum_parag, int sum_succparag, int sum_failparag, double sum_price)
{
    printf("\nSunolikos arithmos paraggeliwn: %d\n", sum_parag);   //Εμφάνιση του συνολικού αριθμού των παραγγελιών που υποβλήθηκαν
    printf("Epituxhmenes Paraggelies: %d\n", sum_succparag);       //Εμφάνιση του συνολικού αριθμού των επιτυχημένων παραγγελιών που υποβλήθηκαν
    printf("Apotuxhmenes Paraggelies: %d\n", sum_failparag);       //Εμφάνιση του συνολικού αριθμού των αποτυχημένων παραγγελιών που υποβλήθηκαν
    printf("Sunoliko kostos: %.2lf\n", sum_price);                 //Εμφάνιση του συνολικού κόστους των παραγγελιών που υποβλήθηκαν με ακρίβεια 2 δεκαδικά ψηφίά
}

//Συνάρτηση για την εκτέλεση των διεργασιών 
int main()
{
    product catalog[20];      //Δήλωση του πίνακα catalog με 20 θέσεις τύπου δομής struct product για τα προϊόντα 
    init_catalog(catalog);    //Αρχικοποίηση του πίνακα catalog με την χρήση της συνάρτησης init_catalog

    int i;

    int p_socket = socket(AF_UNIX, SOCK_STREAM, 0);   //Δημιουργία socket (parent socket) για την επικοινωνία με τον server

    int sum_parag = 0;       //Δήλωση ακεραίου για τον συνολικό αριθμό των παραγγελιών
    int sum_succparag = 0;   //Δήλωση ακεραίου για τον συνολικό αριθμό των επιτυχημένων παραγγελιών
    int sum_failparag = 0;   //Δήλωση ακεραίου για τον συνολικό αριθμό των αποτυχημένων παραγγελιών
    double sum_price = 0;    //Δήλωση πραγματικού αριθμού για το συνολικό κόστος των παραγγελιών

    int parpid = fork();   //Δημιουργία νέας διεργασίας με τη χρήση της κλήσης συστήματος fork
                           //Στη πατρική διεργασία θα γίνει κλήση της συνάρτησης parent_orders ώστε πρώτα να υλοποιηθεί η δημιουργία του socket και μετά να γίνει η σύνδεση των πελατών με την συνάρτηση child_orders
    if(parpid < 0)   //Έλεγχος για την επιτυχία της δημιουργίας της νέας διεργασίας
    {
        perror("Error in fork\n");
        return -1;
    }

    else if(parpid > 0)   //Περίπτωση όπου βρισκόμαστε στην πατρική διεργασία
    {
        parent_orders(catalog, p_socket, &sum_parag, &sum_succparag, &sum_failparag, &sum_price);      //Κλήση της συνάρτησης parent_orders για την διαχείρηση των υποβληθέντων παραγγελιών από τους πελάτες
                                                                                                       //Χρήση δεικτών για την μεταφορά των τιμών των μεταβλητών για χρήση τους στην main συνάρτηση
                                                                                                       
        anafora(catalog);      //Κλήση της συνάρτησης anafora για την εξαγωγή συγκεντρωτικής αναφοράς για κάθε προϊόν
        statistics(sum_parag, sum_succparag, sum_failparag, sum_price);     //Κλήση της συνάρτησης statistics για την τύπωση συγκεντρωτικού μηνύματος με τα στατιστικά των παραγγελιών

        exit(0);  //Εξοδος της πατρικής διεργασίας
    }
    
    for(i=0; i<5; i++)        //Επανάληψη 5 φορές για τη δημιουργία 5 θυγατρικών διεργασιών, δηλαδή 5 πελατών
    {
        pid_t pid = fork();     //Δημιουργία νέας διεργασίας με τη χρήση της κλήσης συστήματος fork

        if(pid < 0)           //Ελέγχος για την επιτυχία της δημιουργίας μιας νέας θυγατρικής διεργασίας
        {
            perror("Error in fork\n");     //Εμφάνιση μηνύματος λάθους σε περίπτωση που δεν δημιουργηθεί μια νέα θυγατρική διεργασία
            return -1;
        }

        //Θυγατρική διεργασία
        else if(pid == 0)      //Περίπτωση όπου βρισκόμαστε στην θυγατρική διεργασία
        {       
            printf("Program starting in...\n");
            printf("3...\n");
            sleep(1);
            printf("2...\n");
            sleep(1);
            printf("1...\n");
            sleep(1);

            child_orders(i+1);     //Κλήση της συνάρτησης child_orders για την υποβολή των αιτημάτων αγοράς προϊόντων από τον πελάτη
                                   //i+1 για την μεταφορά του αριθμού του πελάτη στην συνάρτηση child_orders
            exit(0);
        }
    }      
    
    for(i=0; i<6; i++)    //Επανάληψη 6 φορές για κάθε θυγατρική διεργασία, πελάτη
    {
        wait(NULL);    //Αναμονή για τον τερματισμό θυγατρικής διεργασίας
    }

    unlink("server_socket");   //Διαγραφή του server_socket

    return 0;
}