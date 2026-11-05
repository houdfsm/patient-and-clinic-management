#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// STRUCTURES DE DONNEES

typedef struct patient {
    int ID;
    char nom[50];
    char prenom[50];
    char datenaissance[11];
    char telephone[20];
    struct patient *next;
} patient, *Listepatients;

typedef struct rdv {
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

// CONSTANTES DES HORAIRES DU CABINET


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

void supprimerpatient(patient **head) {
    if (*head == NULL) {
        printf("\n  [!] Aucun patient a supprimer.\n");
        return;
    }
    printf("\n--- SUPPRIMER UN PATIENT ---\n");
    printf("Entrer l'ID du patient a supprimer : ");
    int ID = lireEntier();

    patient *current = *head;
    patient *previous = NULL;

    while (current != NULL && current->ID != ID) {
        previous = current;
        current = current->next;
    }
    if (current == NULL) {
        printf("  [!] Erreur : Patient avec l'ID %d introuvable !\n", ID);
        return;
    }

    char nomTemp[100];
    sprintf(nomTemp, "%s %s", current->nom, current->prenom);

    annulerRdvPatient(&headappointment, ID);

    if (previous == NULL) {
        *head = current->next;
    } else {
        previous->next = current->next;
    }
    free(current);
    printf("\n  [-] Patient #%d (%s) et ses rendez-vous supprimes avec succes !\n", ID, nomTemp);
}

// GESTION DES RENDEZ-VOUS

void insererrdv(rdv **head, rdv *nouveau) {
    int kd = cleDate(nouveau->date), kh = cleHeure(nouveau->heure);
    if (*head == NULL || kd < cleDate((*head)->date) || (kd == cleDate((*head)->date) && kh < cleHeure((*head)->heure))) {
        nouveau->suivant = *head;
        *head = nouveau;
        return;
    }
    rdv *current = *head;
    while (current->suivant != NULL && (cleDate(current->suivant->date) < kd || (cleDate(current->suivant->date) == kd && cleHeure(current->suivant->heure) <= kh))) {
        current = current->suivant;
    }
    nouveau->suivant = current->suivant;
    current->suivant = nouveau;
}

void detacherRdv(rdv **head, rdv *cible) {
    rdv *current = *head, *prev = NULL;
    while (current != NULL && current != cible) {
        prev = current;
        current = current->suivant;
    }
    if (current == NULL) return;
    if (prev == NULL) *head = current->suivant;
    else prev->suivant = current->suivant;
    current->suivant = NULL;
}

void ajoutrdv(rdv **head, patient *patients) {
    if (patients == NULL) {
        printf("\n  [!] Aucun patient enregistre. Veuillez d'abord ajouter un patient.\n");
        return;
    }
    printf("\n--- PROGRAMMER UN RENDEZ-VOUS ---\n");
    printf("ID du patient : ");
    int patientID = lireEntier();
    patient *p = trouverPatient(patients, patientID);
    if (!p) {
        printf("  [!] Erreur : Patient avec l'ID %d introuvable.\n", patientID);
        return;
    }
    printf("  --> Patient selectionne : %s %s\n", p->nom, p->prenom);

    printf("Numero unique du RDV : ");
    int numRDV = lireEntier();
    if (numRDV <= 0) {
        printf("  [!] Erreur : Le numero de rendez-vous doit etre positif.\n");
        return;
    }
    if (trouverrdv(*head, numRDV)) {
        printf("  [!] Erreur : Le numero de rendez-vous #%d existe deja.\n", numRDV);
        return;
    }

    char date[15];
    char heure[10];
    saisirDateRDV("Date du rendez-vous (JJ/MM/AAAA) : ", date);
    saisirHeureRDV("Heure du rendez-vous (HH:MM) : ", date, heure);

    if (creneauOccupe(*head, date, heure)) {
        printf("  [!] Erreur : Ce creneau (%s a %s) est deja reserve par un autre rendez-vous !\n", date, heure);
        return;
    }

    rdv *nouveau = malloc(sizeof(rdv));
    if (!nouveau) {
        printf("  [!] Erreur d'allocation memoire.\n");
        return;
    }
    nouveau->ID = patientID;
    nouveau->numero = numRDV;
    strcpy(nouveau->date, date);
    strcpy(nouveau->heure, heure);

    printf("Motif de la consultation : ");
    lire(nouveau->raison, sizeof(nouveau->raison));
    if (strlen(nouveau->raison) == 0) {
        strcpy(nouveau->raison, "Consultation generale");
    }
    nouveau->suivant = NULL;
    insererrdv(head, nouveau);

    printf("\n  [+] Rendez-vous #%d programme avec succes pour %s %s le %s a %s !\n",
           nouveau->numero, p->nom, p->prenom, nouveau->date, nouveau->heure);
}

// OPTION : Ajout rapide d'un rendez-vous pour aujourd'hui

void ajoutRDVAujourdhui(rdv **head, patient *patients) {
    if (patients == NULL) {
        printf("\n  [!] Aucun patient enregistre. Veuillez d'abord ajouter un patient.\n");
        return;
    }
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char dateAuj[32];
    snprintf(dateAuj, sizeof(dateAuj), "%02d/%02d/%04d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);

    printf("\n--- AJOUT RAPIDE DE RENDEZ-VOUS (AUJOURD'HUI : %s) ---\n", dateAuj);

    printf("ID du patient : ");
    int patientID = lireEntier();
    patient *p = trouverPatient(patients, patientID);
    if (!p) {
        printf("  [!] Erreur : Patient avec l'ID %d introuvable.\n", patientID);
        return;
    }
    printf("  --> Patient selectionne : %s %s\n", p->nom, p->prenom);

    printf("Numero unique du RDV : ");
    int numRDV = lireEntier();
    if (numRDV <= 0) {
        printf("  [!] Erreur : Le numero de rendez-vous doit etre positif.\n");
        return;
    }
    if (trouverrdv(*head, numRDV)) {
        printf("  [!] Erreur : Le numero de rendez-vous #%d existe deja.\n", numRDV);
        return;
    }

    char heure[10];
    saisirHeureRDV("Heure du rendez-vous aujourd'hui (HH:MM) : ", dateAuj, heure);

    if (creneauOccupe(*head, dateAuj, heure)) {
        printf("  [!] Erreur : Le creneau de %s aujourd'hui est deja reserve !\n", heure);
        return;
    }

    rdv *nouveau = malloc(sizeof(rdv));
    if (!nouveau) {
        printf("  [!] Erreur d'allocation memoire.\n");
        return;
    }
    nouveau->ID = patientID;
    nouveau->numero = numRDV;
    strcpy(nouveau->date, dateAuj);
    strcpy(nouveau->heure, heure);

    printf("Motif de la consultation : ");
    lire(nouveau->raison, sizeof(nouveau->raison));
    if (strlen(nouveau->raison) == 0) {
        strcpy(nouveau->raison, "Consultation rapide");
    }
    nouveau->suivant = NULL;
    insererrdv(head, nouveau);

    printf("\n  [+] Rendez-vous rapide #%d programme avec succes pour %s %s aujourd'hui a %s !\n",
           nouveau->numero, p->nom, p->prenom, nouveau->heure);
}

// OPTION : Proposition intelligente du prochain creneau libre (Smart Booking)

void proposerProchainCreneau(rdv **headRDV, patient *headPatient) {
    if (headPatient == NULL) {
        printf("\n  [!] Aucun patient enregistre. Veuillez d'abord ajouter un patient.\n");
        return;
    }
    printf("\n--- PROPOSITION DU PROCHAIN CRENEAU DISPONIBLE ---\n");
    char date[15];
    saisirDateRDV("Entrez la date souhaitee (JJ/MM/AAAA) : ", date);

    char creneauTrouve[6] = "";
    int h, m;
    for (h = heure_debut; h <= heure_fin; h++) {
        for (m = 0; m < 60; m += 30) {
            if (h == heure_fin && m > 0) break;
            char creneau[6];
            snprintf(creneau, sizeof(creneau), "%02d:%02d", h, m);
            if (!dateDansLePasse(date, creneau) && !creneauOccupe(*headRDV, date, creneau)) {
                strcpy(creneauTrouve, creneau);
                break;
            }
        }
        if (strlen(creneauTrouve) > 0) break;
    }

    if (strlen(creneauTrouve) == 0) {
        printf("\n  [i] Aucun creneau disponible pour le %s (journee complete ou creneaux passes).\n", date);
        return;
    }

    printf("\n  [+] Premier creneau libre trouve : Le %s a %s\n", date, creneauTrouve);
    printf("  Voulez-vous reserver ce creneau ? (1 = Oui / 0 = Non) : ");
    int reserver = lireEntier();
    if (reserver != 1) {
        printf("  Reservation ignoree.\n");
        return;
    }

    printf("ID du patient : ");
    int patientID = lireEntier();
    patient *p = trouverPatient(headPatient, patientID);
    if (!p) {
        printf("  [!] Erreur : Patient avec l'ID %d introuvable.\n", patientID);
        return;
    }

    printf("Numero unique du RDV : ");
    int numRDV = lireEntier();
    if (numRDV <= 0 || trouverrdv(*headRDV, numRDV)) {
        printf("  [!] Erreur : Numero de rendez-vous invalide ou deja existant.\n");
        return;
    }

    rdv *nouveau = malloc(sizeof(rdv));
    if (!nouveau) {
        printf("  [!] Erreur d'allocation memoire.\n");
        return;
    }
    nouveau->ID = patientID;
    nouveau->numero = numRDV;
    strcpy(nouveau->date, date);
    strcpy(nouveau->heure, creneauTrouve);

    printf("Motif de la consultation : ");
    lire(nouveau->raison, sizeof(nouveau->raison));
    if (strlen(nouveau->raison) == 0) strcpy(nouveau->raison, "Consultation recommandee");
    nouveau->suivant = NULL;

    insererrdv(headRDV, nouveau);
    printf("\n  [+] Rendez-vous #%d reserve avec succes pour %s %s le %s a %s !\n",
           numRDV, p->nom, p->prenom, date, creneauTrouve);
}


// OPTION : Menu pour programmer un rendez-vous (Libre, Aujourd'hui, Prochain creneau)

void menuProgrammerRDV(rdv **headRDV, patient *headPatient) {
    if (headPatient == NULL) {
        printf("\n  [!] Aucun patient enregistre. Veuillez d'abord ajouter un patient.\n");
        return;
    }
    printf("\n--- PROGRAMMER UN RENDEZ-VOUS ---\n");
    printf("1. Choisir la date et l'heure (Date libre)\n");
    printf("2. Ajout rapide (Aujourd'hui)\n");
    printf("3. Trouver le prochain creneau libre (Automatique)\n");
    printf("Votre choix : ");
    int ch = lireEntier();

    if (ch == 1) {
        ajoutrdv(headRDV, headPatient);
    } else if (ch == 2) {
        ajoutRDVAujourdhui(headRDV, headPatient);
    } else if (ch == 3) {
        proposerProchainCreneau(headRDV, headPatient);
    } else {
        printf("  [!] Choix invalide.\n");
    }
}

void annulerrdv(rdv **headrdv) {
    if (*headrdv == NULL) {
        printf("\n  [!] Aucun rendez-vous a annuler dans la liste.\n");
        return;
    }
    printf("\n--- ANNULER UN RENDEZ-VOUS ---\n");
    printf("Numero du rendez-vous a annuler : ");
    int num = lireEntier();
    rdv *cible = trouverrdv(*headrdv, num);
    if (cible == NULL) {
        printf("  [!] Erreur : Rendez-vous #%d introuvable.\n", num);
        return;
    }
    detacherRdv(headrdv, cible);
    free(cible);
    printf("\n  [-] Rendez-vous #%d annule avec succes !\n", num);
}

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
if(jour<1||jour>31||mois<1||mois>12||annee<1900)return 0;
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
    if (l_rdv == NULL) {
        printf("\n  [!] Aucun rendez-vous enregistre.\n");
        return;
    }
    printf("\n--- MODIFIER UN RENDEZ-VOUS ---\n");
    printf("Entrez le numero du rendez-vous a modifier : ");
    int id = lireEntier();

    rendez_vous *p = l_rdv;
    while (p != NULL && p->num_rdv != id) {
        p = p->suiv;
    }
    if (p == NULL) {
        printf("  [!] Erreur : Rendez-vous #%d introuvable.\n", id);
        return;
    }

    printf("RDV actuel : Date = %s | Heure = %s | Motif = %s\n", p->date, p->heure, p->motif);
    printf("\nQue voulez-vous modifier pour le RDV #%d ?\n", id);
    printf("1. Modifier la date et l'heure\n");
    printf("2. Modifier le motif\n");
    printf("Votre choix : ");
    int choix = lireEntier();

    if (choix == 1) {
        char nouvelle_date[15];
        char nouvelle_heure[10];
        saisirDateRDV("Nouvelle date (JJ/MM/AAAA) : ", nouvelle_date);
        saisirHeureRDV("Nouvelle heure (HH:MM) : ", nouvelle_date, nouvelle_heure);

        if (strcmp(p->date, nouvelle_date) != 0 || strcmp(p->heure, nouvelle_heure) != 0) {
            rendez_vous *temp = l_rdv;
            while (temp != NULL) {
                if (temp->num_rdv != id &&
                    strcmp(temp->date, nouvelle_date) == 0 &&
                    strcmp(temp->heure, nouvelle_heure) == 0) {
                    printf("  [!] Erreur : Ce creneau (%s a %s) est deja reserve !\n", nouvelle_date, nouvelle_heure);
                    return;
                }
                temp = temp->suiv;
            }
        }

        strcpy(p->date, nouvelle_date);
        strcpy(p->heure, nouvelle_heure);
        printf("\n  [+] Date et heure du RDV #%d modifiees avec succes !\n", id);

    } else if (choix == 2) {
        printf("Nouveau motif : ");
        lire(p->motif, sizeof(p->motif));
        printf("\n  [+] Motif du RDV #%d modifie avec succes !\n", id);
    } else {
        printf("  [!] Choix invalide.\n");
    }
}

void afficherRDVJour(ListeRDV l_rdv) {
    if (l_rdv == NULL) {
        printf("\n  [i] Aucun rendez-vous enregistre dans la clinique.\n");
        return;
    }
    char date_cherche[15];
    printf("\n--- AFFICHER LES RENDEZ-VOUS D'UN JOUR ---\n");
    printf("Entrez la date (JJ/MM/AAAA) : ");
    lire(date_cherche, sizeof(date_cherche));
    while (!dateValide(date_cherche, NULL)) {
        printf("  [!] Format de date invalide (JJ/MM/AAAA). Reessayez : ");
        lire(date_cherche, sizeof(date_cherche));
    }

    int count = 0;
    rendez_vous *p = l_rdv;
    while (p != NULL) {
        if (strcmp(p->date, date_cherche) == 0) count++;
        p = p->suiv;
    }
    if (count == 0) {
        printf("  [i] Aucun rendez-vous prevu pour le %s.\n", date_cherche);
        return;
    }

    rendez_vous **tab = malloc(count * sizeof(rendez_vous *));
    int i = 0;
    p = l_rdv;
    while (p != NULL) {
        if (strcmp(p->date, date_cherche) == 0) tab[i++] = p;
        p = p->suiv;
    }

    for (int j = 0; j < count - 1; j++) {
        for (int k = 0; k < count - j - 1; k++) {
            if (strcmp(tab[k]->heure, tab[k + 1]->heure) > 0) {
                rendez_vous *temp = tab[k];
                tab[k] = tab[k + 1];
                tab[k + 1] = temp;
            }
        }
    }

    printf("\n--- Rendez-vous pour le %s (Classes par heure) ---\n", date_cherche);
    printf("+---------+------------+-------+--------------------------------+\n");
    printf("| %-7s | %-10s | %-5s | %-30s |\n", "RDV #", "Patient ID", "Heure", "Motif");
    printf("+---------+------------+-------+--------------------------------+\n");
    for (i = 0; i < count; i++) {
        printf("| %-7d | %-10d | %-5s | %-30s |\n",
               tab[i]->num_rdv, tab[i]->num_patient, tab[i]->heure, tab[i]->motif);
    }
    printf("+---------+------------+-------+--------------------------------+\n");
    free(tab);
}

void afficherRDVPatient(ListeRDV l_rdv) {
    if (l_rdv == NULL) {
        printf("\n  [i] Aucun rendez-vous enregistre dans la clinique.\n");
        return;
    }
    printf("\n--- AFFICHER LES RENDEZ-VOUS D'UN PATIENT ---\n");
    printf("Entrez l'ID du patient : ");
    int id_patient = lireEntier();

    int count = 0;
    rendez_vous *p = l_rdv;
    while (p != NULL) {
        if (p->num_patient == id_patient) count++;
        p = p->suiv;
    }
    if (count == 0) {
        printf("  [i] Ce patient n'a aucun rendez-vous enregistre.\n");
        return;
    }

    rendez_vous **tab = malloc(count * sizeof(rendez_vous *));
    int i = 0;
    p = l_rdv;
    while (p != NULL) {
        if (p->num_patient == id_patient) tab[i++] = p;
        p = p->suiv;
    }

    for (int j = 0; j < count - 1; j++) {
        for (int k = 0; k < count - j - 1; k++) {
            int cmp_date = strcmp(tab[k]->date, tab[k + 1]->date);
            if (cmp_date > 0 || (cmp_date == 0 && strcmp(tab[k]->heure, tab[k + 1]->heure) > 0)) {
                rendez_vous *temp = tab[k];
                tab[k] = tab[k + 1];
                tab[k + 1] = temp;
            }
        }
    }

    printf("\n--- Historique chronologique des rendez-vous du patient #%d ---\n", id_patient);
    printf("+---------+------------+-------+--------------------------------+\n");
    printf("| %-7s | %-10s | %-5s | %-30s |\n", "RDV #", "Date", "Heure", "Motif");
    printf("+---------+------------+-------+--------------------------------+\n");
    for (i = 0; i < count; i++) {
        printf("| %-7d | %-10s | %-5s | %-30s |\n",
               tab[i]->num_rdv, tab[i]->date, tab[i]->heure, tab[i]->motif);
    }
    printf("+---------+------------+-------+--------------------------------+\n");
    free(tab);
}

// OPTION : Generation d'un ticket de rendez-vous (Ecran et Fichier texte)

void genererTicketRDV(rdv *headRDV, patient *headPatient) {
    if (headRDV == NULL) {
        printf("\n  [i] Aucun rendez-vous enregistre dans la clinique.\n");
        return;
    }
    printf("\n--- GENERATION DE TICKET DE RENDEZ-VOUS ---\n");
    printf("Entrez le numero du rendez-vous : ");
    int num = lireEntier();

    rdv *r = trouverrdv(headRDV, num);
    if (!r) {
        printf("  [!] Erreur : Rendez-vous #%d introuvable.\n", num);
        return;
    }

    patient *p = trouverPatient(headPatient, r->ID);
    char nomPatient[128] = "Inconnu", telPatient[20] = "Inconnu";
    int agePatient = 0;
    if (p) {
        snprintf(nomPatient, sizeof(nomPatient), "%s %s", p->nom, p->prenom);
        strcpy(telPatient, p->telephone);
        agePatient = calculerAge(p->datenaissance);
    }

    printf("\n************************************************************\n");
    printf("*               CLINIQUE MEDICALE USTHB                    *\n");
    printf("*            TICKET DE RENDEZ-VOUS MEDICAL                 *\n");
    printf("************************************************************\n");
    printf("  Numero de RDV    : #%d\n", r->numero);
    printf("  Patient ID       : %d\n", r->ID);
    printf("  Nom & Prenom     : %s\n", nomPatient);
    printf("  Age              : %d ans\n", agePatient);
    printf("  Telephone        : %s\n", telPatient);
    printf("  --------------------------------------------------------\n");
    printf("  Date prevue      : %s\n", r->date);
    printf("  Heure            : %s\n", r->heure);
    printf("  Motif            : %s\n", r->raison);
    printf("  --------------------------------------------------------\n");
    printf("  Consignes :\n");
    printf("  - Priere de vous presenter 15 minutes a l'avance.\n");
    printf("  - En cas d'empechement, merci d'annuler au moins 24h avant.\n");
    printf("************************************************************\n");

    char nomFichier[64];
    snprintf(nomFichier, sizeof(nomFichier), "ticket_rdv_%d.txt", r->numero);
    FILE *f = fopen(nomFichier, "w");
    if (f) {
        fprintf(f, "************************************************************\n");
        fprintf(f, "*               CLINIQUE MEDICALE USTHB                    *\n");
        fprintf(f, "*            TICKET DE RENDEZ-VOUS MEDICAL                 *\n");
        fprintf(f, "************************************************************\n");
        fprintf(f, "  Numero de RDV    : #%d\n", r->numero);
        fprintf(f, "  Patient ID       : %d\n", r->ID);
        fprintf(f, "  Nom & Prenom     : %s\n", nomPatient);
        fprintf(f, "  Age              : %d ans\n", agePatient);
        fprintf(f, "  Telephone        : %s\n", telPatient);
        fprintf(f, "  --------------------------------------------------------\n");
        fprintf(f, "  Date prevue      : %s\n", r->date);
        fprintf(f, "  Heure            : %s\n", r->heure);
        fprintf(f, "  Motif            : %s\n", r->raison);
        fprintf(f, "  --------------------------------------------------------\n");
        fprintf(f, "  Consignes :\n");
        fprintf(f, "  - Priere de vous presenter 15 minutes a l'avance.\n");
        fprintf(f, "  - En cas d'empechement, merci d'annuler au moins 24h avant.\n");
        fprintf(f, "************************************************************\n");
        fclose(f);
        printf("\n  [+] Le ticket a ete sauvegarde dans le fichier : %s\n", nomFichier);
    }
}

// OPTION : Filtrage avance des patients par tranche d'age

void filtrerPatientsParAge(patient *head) {
    if (head == NULL) {
        printf("\n  [i] Aucun patient enregistre.\n");
        return;
    }
    printf("\n--- FILTRER LES PATIENTS PAR TRANCHE D'AGE ---\n");
    printf("1. Enfants & Adolescents (< 18 ans)\n");
    printf("2. Adultes (18 a 59 ans)\n");
    printf("3. Seniors (60 ans et plus)\n");
    printf("4. Tranche d'age personnalisee\n");
    printf("Votre choix : ");
    int choix = lireEntier();

    int ageMin = 0, ageMax = 150;
    if (choix == 1) { ageMin = 0; ageMax = 17; }
    else if (choix == 2) { ageMin = 18; ageMax = 59; }
    else if (choix == 3) { ageMin = 60; ageMax = 150; }
    else if (choix == 4) {
        printf("Age minimum : "); ageMin = lireEntier();
        printf("Age maximum : "); ageMax = lireEntier();
    } else {
        printf("  [!] Choix invalide.\n");
        return;
    }

    printf("\n+-----+----------------------+----------------------+-----+------------+------------+\n");
    printf("| %-3s | %-20s | %-20s | %-3s | %-10s | %-10s |\n", "ID", "Nom", "Prenom", "Age", "Date Naiss", "Telephone");
    printf("+-----+----------------------+----------------------+-----+------------+------------+\n");
    int count = 0;
    patient *courant = head;
    while (courant != NULL) {
        int age = calculerAge(courant->datenaissance);
        if (age >= ageMin && age <= ageMax) {
            printf("| %-3d | %-20s | %-20s | %-3d | %-10s | %-10s |\n",
                   courant->ID, courant->nom, courant->prenom, age, courant->datenaissance, courant->telephone);
            count++;
        }
        courant = courant->next;
    }
    printf("+-----+----------------------+----------------------+-----+------------+------------+\n");
    printf("Total trouve : %d patient(s) dans cette tranche.\n", count);
}

// OPTION : Filtrage des rendez-vous par mot-cle du motif

void filtrerRDVParMotif(rdv *headRDV, patient *headPatient) {
    if (headRDV == NULL) {
        printf("\n  [i] Aucun rendez-vous enregistre.\n");
        return;
    }
    printf("\n--- FILTRER LES RENDEZ-VOUS PAR MOTIF ---\n");
    char motCle[50];
    printf("Entrez un mot-cle du motif a rechercher (ex: Urgence, Controle, Analyse) : ");
    lire(motCle, sizeof(motCle));

    printf("\n+---------+------------+----------------------+------------+-------+--------------------------------+\n");
    printf("| %-7s | %-10s | %-20s | %-10s | %-5s | %-30s |\n", "RDV #", "ID Patient", "Nom Patient", "Date", "Heure", "Motif");
    printf("+---------+------------+----------------------+------------+-------+--------------------------------+\n");
    int count = 0;
    rdv *courant = headRDV;
    while (courant != NULL) {
        char motifMin[100], motCleMin[50];
        int i;
        for (i = 0; courant->raison[i]; i++) motifMin[i] = (char)tolower((unsigned char)courant->raison[i]);
        motifMin[i] = '\0';
        for (i = 0; motCle[i]; i++) motCleMin[i] = (char)tolower((unsigned char)motCle[i]);
        motCleMin[i] = '\0';

        if (strstr(motifMin, motCleMin) != NULL) {
            patient *p = trouverPatient(headPatient, courant->ID);
            char nomPatient[128] = "Inconnu";
            if (p) snprintf(nomPatient, sizeof(nomPatient), "%s %s", p->nom, p->prenom);

            printf("| %-7d | %-10d | %-20s | %-10s | %-5s | %-30s |\n",
                   courant->numero, courant->ID, nomPatient, courant->date, courant->heure, courant->raison);
            count++;
        }
        courant = courant->suivant;
    }
    printf("+---------+------------+----------------------+------------+-------+--------------------------------+\n");
    printf("Total trouve : %d rendez-vous correspondant au motif \"%s\".\n", count, motCle);
}

// PARTIE 10 : Rechercher les creneaux disponibles

void rechercherCreneauxDisponibles(rdv *head, const char *date) {
    int h, m, occupe, nb_libres = 0;
    rdv *courant;
    char creneau[6];

    printf("\n========================================\n");
    printf("  Creneaux disponibles pour le %s\n", date);
    printf("========================================\n");

    for (h = heure_debut; h <= heure_fin; h++) {
        for (m = 0; m < 60; m += 30) {
            if (h == heure_fin && m > 0) break;
            snprintf(creneau, sizeof(creneau), "%02d:%02d", h, m);
            occupe = 0;
            courant = head;
            while (courant != NULL) {
                if (strcmp(courant->date, date) == 0 && strcmp(courant->heure, creneau) == 0) {
                    occupe = 1;
                    break;
                }
                courant = courant->suivant;
            }
            if (occupe == 0) {
                printf("  [LIBRE]   %s\n", creneau);
                nb_libres++;
            } else {
                printf("  [OCCUPE]  %s\n", creneau);
            }
        }
    }

    printf("----------------------------------------\n");
    if (nb_libres == 0)
        printf("  Aucun creneau disponible pour cette date.\n");
    else
        printf("  %d creneau(x) disponible(s).\n", nb_libres);
    printf("========================================\n");
}

// PARTIE 11 : Calculer des statistiques

int compterPatients(patient *head) {
    int compteur = 0;
    patient *courant = head;
    while (courant != NULL) {
        compteur++;
        courant = courant->next;
    }
    return compteur;
}

int compterRDV(rdv *head) {
    int compteur = 0;
    rdv *courant = head;
    while (courant != NULL) {
        compteur++;
        courant = courant->suivant;
    }
    return compteur;
}

int compterRDVParDate(rdv *head, const char *date) {
    int compteur = 0;
    rdv *courant = head;
    while (courant != NULL) {
        if (strcmp(courant->date, date) == 0)
            compteur++;
        courant = courant->suivant;
    }
    return compteur;
}

// OPTION : Statistiques avancees de la clinique (Demographie, Taux, Frequence)

void afficherStatistiquesAvancees(patient *listePatients, rdv *listeRDV) {
    printf("\n========================================================================\n");
    printf("                  STATISTIQUES AVANCEES DE LA CLINIQUE                  \n");
    printf("========================================================================\n");

    int nbPatients = compterPatients(listePatients);
    int nbRDV = compterRDV(listeRDV);

    printf("  1. TOTAL GENERAL :\n");
    printf("     - Nombre total de patients inscrits   : %d\n", nbPatients);
    printf("     - Nombre total de rendez-vous actifs  : %d\n", nbRDV);

    if (nbPatients > 0) {
        int sommeAge = 0;
        int minAge = 999, maxAge = -1;
        patient *pMin = NULL, *pMax = NULL;
        patient *courant = listePatients;
        while (courant != NULL) {
            int age = calculerAge(courant->datenaissance);
            sommeAge += age;
            if (age < minAge) { minAge = age; pMin = courant; }
            if (age > maxAge) { maxAge = age; pMax = courant; }
            courant = courant->next;
        }
        double ageMoyen = (double)sommeAge / nbPatients;
        printf("\n  2. DEMOGRAPHIE DES PATIENTS :\n");
        printf("     - Age moyen des patients              : %.1f ans\n", ageMoyen);
        if (pMin) printf("     - Patient le plus jeune               : %s %s (%d ans)\n", pMin->nom, pMin->prenom, minAge);
        if (pMax) printf("     - Patient le plus age                 : %s %s (%d ans)\n", pMax->nom, pMax->prenom, maxAge);
    }

    if (nbPatients > 0 && nbRDV > 0) {
        patient *pFreq = NULL;
        int maxRdvCount = 0;
        patient *cp = listePatients;
        while (cp != NULL) {
            int c = 0;
            rdv *crdv = listeRDV;
            while (crdv != NULL) {
                if (crdv->ID == cp->ID) c++;
                crdv = crdv->suivant;
            }
            if (c > maxRdvCount) {
                maxRdvCount = c;
                pFreq = cp;
            }
            cp = cp->next;
        }
        printf("\n  3. FREQUENCE DES CONSULTATIONS :\n");
        if (pFreq && maxRdvCount > 0) {
            printf("     - Patient le plus fidele              : %s %s (%d rendez-vous)\n",
                   pFreq->nom, pFreq->prenom, maxRdvCount);
        } else {
            printf("     - Aucun rendez-vous associe aux patients actuels.\n");
        }
    }

    printf("\n  4. TAUX D'OCCUPATION DU CABINET :\n");
    char date[15];
    printf("     Entrez une date a analyser (JJ/MM/AAAA) : ");
    saisirDateRDV("", date);

    int nbOccupes = compterRDVParDate(listeRDV, date);
    int totalCreneaux = ((heure_fin - heure_debut) * 2) + 1;
    double taux = ((double)nbOccupes / totalCreneaux) * 100.0;

    printf("     - Creneaux occupes le %s : %d / %d\n", date, nbOccupes, totalCreneaux);
    printf("     - Taux d'occupation effectif         : %.1f %%\n", taux);
    printf("========================================================================\n");
}

// OPTION : Menu des statistiques et des filtres de la clinique

void menuStatistiquesEtFiltres(patient *headPatient, rdv *headRDV) {
    printf("\n--- STATISTIQUES & ANALYSES ---\n");
    printf("1. Statistiques globales & Demographie (Age moyen, Taux d'occupation, etc.)\n");
    printf("2. Filtrer les patients par tranche d'age\n");
    printf("3. Filtrer les rendez-vous par mot-cle du motif\n");
    printf("Votre choix : ");
    int ch = lireEntier();

    if (ch == 1) {
        afficherStatistiquesAvancees(headPatient, headRDV);
    } else if (ch == 2) {
        filtrerPatientsParAge(headPatient);
    } else if (ch == 3) {
        filtrerRDVParMotif(headRDV, headPatient);
    } else {
        printf("  [!] Choix invalide.\n");
    }
}

// PARTIE 12 : Quitter le programme (liberer la memoire)

void libererListePatients(patient **head) {
    patient *courant = *head;
    patient *suiv;
    while (courant != NULL) {
        suiv = courant->next;
        free(courant);
        courant = suiv;
    }
    *head = NULL;
}

void libererListeRDV(rdv **head) {
    rdv *courant = *head;
    rdv *suiv;
    while (courant != NULL) {
        suiv = courant->suivant;
        free(courant);
        courant = suiv;
    }
    *head = NULL;
}

void quitterProgramme(patient **listePatients, rdv **listeRDV) {
    printf("\n========================================\n");
    printf("  Liberation de la memoire dynamique...\n");
    printf("========================================\n");
    libererListePatients(listePatients);
    libererListeRDV(listeRDV);
    printf("  Memoire liberee avec succes.\n");
    printf("  Au revoir et a bientot !\n");
    printf("========================================\n");
}

// PRESENTATION DU PROJET

void afficherPresentation(void) {
    printf("\n");
    printf("========================================================================\n");
    printf("                    USTHB - FACULTE D'INFORMATIQUE                      \n");
    printf("                 2EME ANNEE LICENCE - ASD3 (2026)                       \n");
    printf("========================================================================\n\n");
    printf("        GESTION DES PATIENTS ET DES RENDEZ-VOUS MEDICAUX                \n");
    printf("        Projet 5 | Groupe 28 | Listes Simplement Chainees               \n\n");
    printf(" MEMBRES DU GROUPE :\n");
    printf(" ----------------------------------------------------------------------\n");
    printf("  Djafercherif  | Oumeima\n");
    printf("  Oueddane      | Loubna\n");
    printf("  Mohammedi     | Ilyes\n");
    printf("  Douifi        | Houssem eddine\n");
    printf(" ----------------------------------------------------------------------\n\n");
    printf(" Appuyez sur Entree pour lancer l'application...");
    char buf[10];
    lire(buf, sizeof(buf));
}

// OPTION : Menu principal interactif et ergonomique

void afficherMenu(void) {
    printf("\n");
    printf("========================================================================\n");
    printf("                CLINIQUE MEDICALE - MENU PRINCIPAL                      \n");
    printf("========================================================================\n");
    printf("  [ GESTION DES PATIENTS ]\n");
    printf("    1. Ajouter un nouveau patient\n");
    printf("    2. Afficher la liste des patients (avec option de tri A-Z)\n");
    printf("    3. Rechercher un patient (ID ou Nom - auto-detecte)\n");
    printf("    4. Modifier les informations d'un patient\n");
    printf("    5. Supprimer le dossier d'un patient\n\n");
    printf("  [ GESTION DES RENDEZ-VOUS ]\n");
    printf("    6. Programmer un rendez-vous (Libre / Aujourd'hui / Creneau suggere)\n");
    printf("    7. Annuler un rendez-vous\n");
    printf("    8. Modifier un rendez-vous existant\n");
    printf("    9. Afficher les rendez-vous d'une journee (classes par heure)\n");
    printf("   10. Afficher l'historique des RDV d'un patient\n");
    printf("   11. Rechercher les creneaux disponibles pour une date\n");
    printf("   12. Generer un ticket de rendez-vous (Ecran & Fichier)\n\n");
    printf("  [ STATISTIQUES & ANALYSES ]\n");
    printf("   13. Statistiques et filtres (Demographie, Taux, Filtres age/motif)\n");
    printf("   14. Quitter le programme (liberer la memoire)\n");
    printf("========================================================================\n");
    printf("  Entrez votre choix [1-14] : ");
}

// PROGRAMME PRINCIPAL

int main(void) {
    patient *headpatient = NULL;
    int choix;

    afficherPresentation();

    do {
        afficherMenu();
        choix = lireEntier();

        switch (choix) {
            case 1:
                ajoutpatient(&headpatient);
                break;
            case 2:
                display(headpatient);
                break;
            case 3:
                rechercherPatient(headpatient);
                break;
            case 4:
                modifierpatient(headpatient);
                break;
            case 5:
                supprimerpatient(&headpatient);
                break;
            case 6:
                menuProgrammerRDV(&headappointment, headpatient);
                break;
            case 7:
                annulerrdv(&headappointment);
                break;
            case 8:
                modifierRDV(headappointment);
                break;
            case 9:
                afficherRDVJour(headappointment);
                break;
            case 10:
                afficherRDVPatient(headappointment);
                break;
            case 11: {
                char date[15];
                printf("\n--- RECHERCHER LES CRENEAUX DISPONIBLES ---\n");
                saisirDateRDV("Entrez la date (JJ/MM/AAAA) : ", date);
                rechercherCreneauxDisponibles(headappointment, date);
                break;
            }
            case 12:
                genererTicketRDV(headappointment, headpatient);
                break;
            case 13:
                menuStatistiquesEtFiltres(headpatient, headappointment);
                break;
            case 14:
                quitterProgramme(&headpatient, &headappointment);
                break;
            default:
                printf("\n  [!] Choix invalide ! Veuillez saisir un nombre entre 1 et 14.\n");
                break;
        }

        if (choix != 14) {
            printf("\nAppuyez sur Entree pour revenir au menu principal...");
            char pause[10];
            lire(pause, sizeof(pause));
        }

    } while (choix != 14);

    return 0;
}
