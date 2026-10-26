#include<stdio.h>
#include<stdlib.h>
#include<string.h>
  typedef struct patient{
         int ID ;
         char nom[50];
         char prenom[50];
         char datenaissance[11];
            char telephone[20];
          struct patient *next;
    }patient;
    typedef struct appointment{
    int numero;
    int patientID;
    char date[11];
    char heure[6];
    char motif[100];
    struct appointment *next;
} appointment;

appointment *headappointment = NULL; 
patient *recherpatient(patient *current, int ID);
void rechercherParNom(patient *head);

    void ajoutpatient(patient **head){
        patient *newpatient = malloc(sizeof(patient));
        if ( newpatient == NULL){
            printf("error");
            return ;
        }

        printf("Entre l'ID du patient :");
        scanf("%d", &newpatient->ID);

       patient *result = recherpatient(*head, newpatient->ID);
        if (result != NULL) {
    printf("Erreur : cet ID existe deja !\n");
    free(newpatient);
    return;
}

        printf("Entrer le nom du patient :");
        scanf("%49s", &newpatient->nom);

        printf("Entrer le prenom du patient :");
        scanf("%49s", &newpatient->prenom);

        printf("Entrer la date de naissance du patient :");
        scanf("%10s", &newpatient->datenaissance);

        printf("Entrer le numero de telephone du patient :");
        scanf("%19s", &newpatient->telephone);
    

        newpatient->next = *head ;
        *head = newpatient;
    }
    patient *recherpatient(patient *current, int ID) {
    while (current != NULL) {
        if (current->ID == ID) {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

void rechercherParNom(patient *head) {
    char nomRecherche[50];
    int compteur = 0;
    int idRecherche;

    patient *current = head;

    printf("Entrer le nom du patient a rechercher : ");
    scanf("%49s", nomRecherche);

   
    while (current != NULL) {
        if (strcmp(current->nom, nomRecherche) == 0) {
            printf("ID: %d | Nom: %s | Prenom: %s\n",
                   current->ID, current->nom, current->prenom);
            compteur++;
        }

        current = current->next;
    }

    if (compteur == 0) {
        printf("Aucun patient trouve.\n");
        return;
    }

    if (compteur == 1) {
        printf("Un seul patient trouve.\n");
        return;
    }

    /* Several patients have the same name */
    printf("Plusieurs patients ont ce nom.\n");
    printf("Entrer l'ID du patient recherche : ");
    scanf("%d", &idRecherche);

    patient *result = recherpatient(head, idRecherche);

    if (result == NULL) {
        printf("Aucun patient avec cet ID.\n");
        return;
    }

    if (strcmp(result->nom, nomRecherche) != 0) {
        printf("Cet ID ne correspond pas au nom recherche.\n");
        return;
    }

    printf("\nPatient selectionne :\n");
    printf("ID: %d\n", result->ID);
    printf("Nom: %s\n", result->nom);
    printf("Prenom: %s\n", result->prenom);
    printf("Date de naissance: %s\n", result->datenaissance);
    printf("Telephone: %s\n", result->telephone);
}

    void display(patient *head){
      
    patient *current = head;

    while (current != NULL) {
        // We'll display the patient's information here.
    printf("ID: %d\n", current->ID);

    printf("Nom: %s\n", current->nom);

    printf("Prenom: %s\n", current->prenom);

    printf("Date de naissance: %s\n", current->datenaissance);

    printf("Telephone: %s\n", current->telephone);

        current = current->next;
    }
}

void modifierpatient(patient *head) {
    int id;

    printf("Entrer l'ID du patient a modifier : ");
    scanf("%d", &id);

    patient *result = recherpatient(head, id);

    if (result == NULL) {
        printf("Patient introuvable !\n");
        return;
    }

    printf("Entrer le nouveau nom : ");
    scanf("%49s", result->nom);

    printf("Entrer le nouveau prenom : ");
    scanf("%49s", result->prenom);

    printf("Entrer la nouvelle date de naissance : ");
    scanf("%10s", result->datenaissance);

    printf("Entrer le nouveau numero de telephone : ");
    scanf("%19s", result->telephone);

    printf("Patient modifie avec succes !\n");
}

void annulerRdvPatient(appointment **head, int patientID) {
    appointment *current = *head;
    appointment *previous = NULL;

    while (current != NULL) {
        if (current->patientID == patientID) {
            appointment *temp = current;

            if (previous == NULL) {
                *head = current->next;
            } else {
                previous->next = current->next;
            }

            current = current->next;
            free(temp);
        } else {
            previous = current;
            current = current->next;
        }
    }
}

void supprimerpatient(patient **head){
int ID;
printf("Entre l'ID du patient a supprimer :");
scanf("%d", &ID);

patient *current = *head ;
patient *previous = NULL ;

while(current != NULL && current->ID !=ID){
    previous = current ;
    current = current->next;
}
if (current == NULL){
    printf("Patient introuvable !\n");
    return ;
} 
annulerRdvPatient(&headappointment, ID);

 if(previous == NULL ){
*head = current->next;
} 
else{
    previous->next = current->next ;
}
free(current);
printf("Patient supprimer avec succes\n");

}




int main(){
    patient *headpatient = NULL;
       
    int choix;
printf("\n=== GESTION DES PATIENTS ===\n");
printf("1. Ajouter un patient\n");
printf("2. Afficher les patients\n");
printf("3. Rechercher un patient par ID\n");
printf("4. Rechercher un patient par son nom\n");
printf("5. Modifier les informations d'un patient\n");
printf("6. Supprimer un patient\n");
printf("7. Quitter\n");

printf("Votre choix : ");
do{
scanf("%d", &choix);

  if (choix == 1) {
    ajoutpatient(&headpatient);
}
else if (choix == 2) {
    display(headpatient);
}
else if (choix == 3) {
    int id;

    printf("Entrer l'ID du patient a rechercher : ");
    scanf("%d", &id);

    patient *result = recherpatient(headpatient, id);

    if (result != NULL) {
        printf("Patient trouve !\n");
        printf("ID : %d\n", result->ID);
        printf("Nom : %s\n", result->nom);
        printf("Prenom : %s\n", result->prenom);
        printf("Date de naissance : %s\n", result->datenaissance);
        printf("Telephone : %s\n", result->telephone);
    }
    else {
        printf("Patient introuvable !\n");
    }
}
else if (choix == 4) {
    rechercherParNom(headpatient);
}
else if (choix == 5) {
    modifierpatient(headpatient);
}
else if (choix == 6) {
   supprimerpatient(&headpatient);
}
else if (choix == 7) {
    printf("Au revoir!\n");
}
else {
    printf("Choix invalide!\n");
} }while (choix !=7);


    return 0;
}