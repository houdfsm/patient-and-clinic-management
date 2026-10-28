#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

// structures (une seule pour tout le projet)

typedef struct patient{
    int ID ;
    char nom[50];
    char prenom[50];
    char datenaissance[11];
    char telephone[20];
    struct patient *next;
} patient, *Listepatients;

typedef struct rdv{
    union { int numero; int num_rdv; };
    union { int ID; int patientID; int num_patient; };
    char date[15];
    char heure[10];
    union { char raison[100]; char motif[100]; };
    union { struct rdv *suivant; struct rdv *next; struct rdv *suiv; };
} rdv;

typedef rdv appointment;
typedef rdv rendez_vous;
typedef rendez_vous *ListeRDV;

appointment *headappointment = NULL;
patient *recherpatient(patient *current, int ID);

void rechercherParNom(patient *head);

// utilise par Qst 5 (relie a recherpatient)
patient *trouverPatient(patient *head, int ID){
    return recherpatient(head, ID);
}


// ================= PARTIE 1 : HOUSSEM (main.c) =================

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



void afficherPresentation(void) {

    printf("\n");
    printf("========================================\n");
    printf("       USTHB - COMPUTER SCIENCE\n");
    printf("          2ND YEAR - ASD\n");
    printf("========================================\n\n");

    printf(" PATIENT AND APPOINTMENT MANAGEMENT\n");
    printf(" Project 5 | Groupe 28\n\n");

    printf(" GROUP MEMBERS\n");
    printf("------------------------------\n");
    printf(" Djafercherif  | Oumeima\n");
    printf(" Oueddane      | Loubna\n");
    printf(" Mohammed i    | Ilyes\n");
    printf(" Douifi        | Houssem eddine\n");
    printf("------------------------------\n\n");

    printf(" Press ENTER to continue...");
    getchar();
}





// ================= PARTIE 2 : AMI 1 (Qst 5 et 6) =================

// horaire de travail de cabinet 
#define heure_debut 8
#define heure_fin 18
// outils de saisie

void lire(char*dest,int taille){
if(fgets(dest,taille,stdin)==NULL){
dest[0]='\0';
return;
}
dest[strcspn(dest,"\n")]='\0';
}
int lireEntier(void){
char buf[50];
lire(buf,sizeof(buf));
return(int)strtol(buf,NULL,10);
}
// Comparer deux chaines de caracteres en ignorant les majuscules/minuscules

int memeTexte(const char*a,const char*b){
while(*a&&*b){
if(tolower((unsigned char)*a)!=tolower((unsigned char)*b))return 0;
a++;b++;
}
return*a==*b;
}
// check si la date et lheure sont valides 

int dateValide(const char*dateStr,char*dest){
if(strlen(dateStr)!=10)return 0;
if(dateStr[2]!='/'||dateStr[5]!='/')return 0;
int jour=atoi(dateStr);
int mois=atoi(dateStr+3);
int annee=atoi(dateStr+6);
if(jour<1||jour>31||mois<1||mois>12||annee<0)return 0;
strcpy(dest,dateStr);
return 1;
}
int heureValide(const char*heureStr,char*dest){
if(strlen(heureStr)!=5)return 0;
if(heureStr[2]!=':')return 0;
int heures=atoi(heureStr);
int minutes=atoi(heureStr+3);
if(heures<0||heures>23||minutes<0||minutes>59)return 0;
strcpy(dest,heureStr);
return 1;
}
// comparer les dates et les heures pour savoir si le rdv est dans les horaires de travail 
int dans_horaire(const char*date,const char*heure){
int heures=atoi(heure);
int minutes=atoi(heure+3);
if(heures<heure_debut||heures>heure_fin||(heures==heure_fin&&minutes>0))
return 0;
return 1;
}
// converetir une date et une heure en nomnbre pour les comparer
int cleDate(const char*d){
int j,m,a;
sscanf(d,"%d/%d/%d",&j,&m,&a);
return a*10000+m*100+j;
}
// converetr en minutes so we can les comparer 
int cleHeure(const char*h){
int hh,mm;
sscanf(h,"%d:%d",&hh,&mm);
return hh*60+mm;
}
void saisirDate(const char*message,char*dest){
char saisie[50];
while(1){
printf("%s",message);
lire(saisie,sizeof(saisie));
if(dateValide(saisie,dest))return;
printf("Date invalide (format JJ/MM/AAAA).\n");
}
}
void saisirHeure(const char*message,char*dest){
char saisie[50];
while(1){
printf("%s",message);
lire(saisie,sizeof(saisie));
if(heureValide(saisie,dest))return;
printf("Heure invalide (HH:MM, de 08:00 a 18:00, par pas de 30 min).\n");
}
}

// 5. Faire un rendez-vous
// rechercher un rdv par son num unique dans la liste chainee de rdv
rdv*trouverrdv(rdv*head,int num){
rdv*current=head;
while(current!=NULL){
if(current->numero==num)return current;
current=current->suivant;
}
return NULL;
}
// verfying si un rdv existe deja pour un patient a une date et heure donnee
int rdvexiste(rdv*head,int id,const char*date,const char*heure){
rdv*current=head;
while(current!=NULL){
if(current->ID==id&&strcmp(current->date,date)==0&&strcmp(current->heure,heure)==0)return 1;
current=current->suivant;
}
return 0;
}
// inserer un rdv dans la liste dune maniere triee par date et heure
void insererrdv(rdv**head,rdv*nouveau){
int kd=cleDate(nouveau->date),kh=cleHeure(nouveau->heure);
if(*head==NULL||kd<cleDate((*head)->date)||(kd==cleDate((*head)->date)&&kh<cleHeure((*head)->heure))){
nouveau->suivant=*head;
*head=nouveau;
return;
}
rdv*current=*head;
while(current->suivant!=NULL&&(cleDate(current->suivant->date)<kd||(cleDate(current->suivant->date)==kd&&cleHeure(current->suivant->heure)<=kh))){
current=current->suivant;
}
nouveau->suivant=current->suivant;
current->suivant=nouveau;
}
// Détache un rendez-vous de la liste sans libérer sa mémoire (utile pour le déplacer) 
void detacherRdv(rdv**head,rdv*cible){
rdv*current=*head,*prev=NULL;
while(current!=NULL&&current!=cible){
prev=current;
current=current->suivant;
}
if(current==NULL)return;
if(prev==NULL)*head=current->suivant;
else prev->suivant=current->suivant;
current->suivant=NULL;
}
// planning des rdv 
void ajoutrdv(rdv**head,patient*patients){
rdv*nouveau=malloc(sizeof(rdv));
if(!nouveau){
printf("erreur d'allocation de memoire\n");
return;
}
printf("ID du patient : ");
nouveau->ID=lireEntier();
if(!trouverPatient(patients,nouveau->ID)){
printf("Patient introuvable.\n");
free(nouveau);
return;
}
printf("Numero du rdv : ");
nouveau->numero=lireEntier();
if(trouverrdv(*head,nouveau->numero)){
printf("Ce numero de rdv existe deja.\n");
free(nouveau);
return;
}
saisirDate("Date du rdv (JJ/MM/AAAA) : ",nouveau->date);
saisirHeure("Heure du rdv (HH:MM) : ",nouveau->heure);
if(!dans_horaire(nouveau->date,nouveau->heure)){
printf("Hors des horaires de travail (08:00 a 18:00).\n");
free(nouveau);
return;
}
if(rdvexiste(*head,nouveau->ID,nouveau->date,nouveau->heure)){
printf("Un rdv existe deja pour ce patient a cette date et heure.\n");
free(nouveau);
return;
}
printf("Raison du rdv : ");
lire(nouveau->raison,sizeof(nouveau->raison));
nouveau->suivant=NULL;
insererrdv(head,nouveau);
printf("Rendez-vous ajoute avec succes.\n");
}
// 6. Annuler (supprimer) un rendez-vous
void annulerrdv(rdv**headrdv){
printf("Numero du rendez-vous a annuler : ");
int num=lireEntier();
rdv*cible=trouverrdv(*headrdv,num);
if(cible==NULL){
printf("Rendez-vous introuvable.\n");
return;
}
detacherRdv(headrdv,cible);
free(cible);
printf("Rendez-vous annule avec succes.\n");
}





// ================= PARTIE 3 : AMI 2 (Qst 7, 8 et 9) =================

// ----------------------
// Qst 7
// ----------------------

void modifierRDV(ListeRDV l_rdv) {
    int id;
    printf("Entrez le numéro du rendez-vous à modifier : ");
    scanf("%d", &id);

    // find the appointment node 
    rendez_vous* p = l_rdv;
    while (p != NULL && p->num_rdv != id) {
        p = p->suiv;
    }
    if (p == NULL) {
        printf("Erreur : Rendez-vous introuvable.\n");
        return;
    }

    // what to change
    int choix;
    printf("\nQue voulez-vous modifier pour le RDV #%d ?\n", id);
    printf("1. Modifier la date et l'heure\n");
    printf("2. Modifier le motif\n");
    printf("Votre choix : ");
    scanf("%d", &choix);

    if (choix == 1) {
        char nouvelle_date[15];
        char nouvelle_heure[10];
        printf("Nouvelle date (YYYY-MM-DD) : ");
        scanf("%s", nouvelle_date);
        printf("Nouvelle heure (HH:MM) : ");
        scanf("%s", nouvelle_heure);

        // Check slot availability (if date or hour changed)
        if (strcmp(p->date, nouvelle_date) != 0 || strcmp(p->heure, nouvelle_heure) != 0) {
            rendez_vous* temp = l_rdv;
            while (temp != NULL) {
                if (temp->num_rdv != id && 
                    strcmp(temp->date, nouvelle_date) == 0 && 
                    strcmp(temp->heure, nouvelle_heure) == 0) {
                    printf("Erreur : Ce créneau horaire est déjà pris !\n");
                    return;
                }
                temp = temp->suiv;
            }
        }

        // Apply date & time changes
        strcpy(p->date, nouvelle_date);
        strcpy(p->heure, nouvelle_heure);
        printf("Date et heure modifiées avec succès !\n");

    } else if (choix == 2) {
        char nouveau_motif[100];
        printf("Nouveau motif : ");
        scanf(" %[^\n]", nouveau_motif); // Reads string with spaces

        // Apply motif change
        strcpy(p->motif, nouveau_motif);
        printf("Motif modifié avec succès !\n");

    } else {
        printf("Choix invalide.\n");
    }
}

// -------------------
// Qst 8 
// -------------------

void afficherRDVJour(ListeRDV l_rdv) {
    char date_cherche[15];
    printf("Entrez la date à rechercher (YYYY-MM-DD) : ");
    scanf("%s", date_cherche);

    // count matching appointments
    int count = 0;
    rendez_vous* p = l_rdv;
    while (p != NULL) {
        if (strcmp(p->date, date_cherche) == 0) {
            count++;
        }
        p = p->suiv;
    }
    if (count == 0) {
        printf("Aucun rendez-vous prévu pour cette date.\n");
        return;
    }

    // allocate a temporary array of pointers
    rendez_vous** tab = malloc(count * sizeof(rendez_vous*));
    int i = 0;
    p = l_rdv;
    while (p != NULL) {
        if (strcmp(p->date, date_cherche) == 0) {
            tab[i++] = p;
        }
        p = p->suiv;
    }

    // sort the array by hour 
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(tab[j]->heure, tab[j+1]->heure) > 0) {
                rendez_vous* temp = tab[j];
                tab[j] = tab[j+1];
                tab[j+1] = temp;
            }
        }
    }

    // display sorted appointments
    printf("\n--- Rendez-vous pour le %s (Classés par heure) ---\n", date_cherche);
    for (int i = 0; i < count; i++) {
        printf("RDV #%d | Patient #%d | Heure: %s | Motif: %s\n", 
               tab[i]->num_rdv, tab[i]->num_patient, tab[i]->heure, tab[i]->motif);
    }
    free(tab);
}

// ------------------------
// Qst 9 
// ------------------------

void afficherRDVPatient(ListeRDV l_rdv) {
    int id_patient;
    printf("Entrez le numéro du patient : ");
    scanf("%d", &id_patient);

    // count matching appointments for this patient
    int count = 0;
    rendez_vous* p = l_rdv;
    while (p != NULL) {
        if (p->num_patient == id_patient) {
            count++;
        }
        p = p->suiv;
    }
    if (count == 0) {
        printf("Ce patient n'a aucun rendez-vous enregistré.\n");
        return;
    }

    // allocate a temporary array of pointers
    rendez_vous** tab = malloc(count * sizeof(rendez_vous*));
    int i = 0;
    p = l_rdv;
    while (p != NULL) {
        if (p->num_patient == id_patient) {
            tab[i++] = p;
        }
        p = p->suiv;
    }

    // sort chronologically 
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int cmp_date = strcmp(tab[j]->date, tab[j+1]->date);
            if (cmp_date > 0 || (cmp_date == 0 && strcmp(tab[j]->heure, tab[j+1]->heure) > 0)) {
                rendez_vous* temp = tab[j];
                tab[j] = tab[j+1];
                tab[j+1] = temp;
            }
        }
    }

    // display sorted history
    printf("\n--- Historique chronologique des rendez-vous du patient #%d ---\n", id_patient);
    for (int i = 0; i < count; i++) {
        printf("RDV #%d | Date: %s | Heure: %s | Motif: %s\n", 
               tab[i]->num_rdv, tab[i]->date, tab[i]->heure, tab[i]->motif);
    }
    free(tab);
}





// ================= MAIN (Houssem) =================

int main(){
    patient *headpatient = NULL;
       
    int choix;
    afficherPresentation();
printf("\n=== GESTION DES PATIENTS ===\n");
printf("1. Ajouter un patient\n");
printf("2. Afficher les patients\n");
printf("3. Rechercher un patient par ID\n");
printf("4. Rechercher un patient par son nom\n");
printf("5. Modifier les informations d'un patient\n");
printf("6. Supprimer un patient\n");
printf("7. Ajouter un rendez-vous\n");
printf("8. Annuler un rendez-vous\n");
printf("9. Modifier un rendez-vous\n");
printf("10. Afficher les rendez-vous d'un jour\n");
printf("11. Afficher les rendez-vous d'un patient\n");
printf("12. Quitter\n");

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
    int c; while ((c = getchar()) != '\n' && c != EOF);
    ajoutrdv(&headappointment, headpatient);
}
else if (choix == 8) {
    int c; while ((c = getchar()) != '\n' && c != EOF);
    annulerrdv(&headappointment);
}
else if (choix == 9) {
    modifierRDV(headappointment);
}
else if (choix == 10) {
    afficherRDVJour(headappointment);
}
else if (choix == 11) {
    afficherRDVPatient(headappointment);
}
else if (choix == 12) {
    printf("Au revoir!\n");
}
else {
    printf("Choix invalide!\n");
} }while (choix !=12);


    return 0;
}
