#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
// structure defs
typedef struct patient{
int ID;
char nom[50];
char prenom[50];
char datenaissance[11];
char telephone[20];
struct patient*next;
}patient;

typedef struct rdv{
int numero; // must be unique et c le numero du rdv dun seul patient
int ID; // id de patient
char date[11]; // date du rdv
char heure[6]; // heure du rdv
char raison[100]; // raison du rdv 
struct rdv*suivant; //pointer a le rdv de le patient suivant
}rdv;
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
