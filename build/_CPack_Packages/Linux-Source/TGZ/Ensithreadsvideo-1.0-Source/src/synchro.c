#include <pthread.h>
#include "synchro.h"
#include "ensitheora.h"

extern bool fini;
pthread_t theora2SDLthread;
pthread_mutex_t mutex_for_hashmap;

/* les variables pour la synchro, ici */

pthread_mutex_t mutex_window_size;
pthread_cond_t cond_size;
bool is_sent_size;

pthread_mutex_t mutex_window_texture;
pthread_cond_t cond_texture;
bool is_sent_texture;

pthread_mutex_t mutex_window_prod;
pthread_cond_t cond_empty_prod;
pthread_cond_t cond_full_prod;
unsigned int texture_number;

/* l'implantation des fonctions de synchro ici */

//Q5 : coder cette synchronisation (coté décodeur) qui transmet taille de l'image à afficher
//utiliser buffer[0].width ainsi que buffer[0].height pour accéder aux infos taille fenêtre à partir variable buffer
void envoiTailleFenetre(th_ycbcr_buffer buffer)
{
    pthread_mutex_lock(&mutex_window_size);

    windowsx = buffer[0].width; //windowsx et windowsy déf dans ensitheora.c
    windowsy = buffer[0].height;

    is_sent_size = true;
    //puis on libère le consommateur :
    pthread_cond_signal(&cond_size); //prototype : int pthread_cond_signal(pthread_cond_t *cond);
                                     //shall unblock at least one of the threads that are blocked on the specified condition variable cond
    pthread_mutex_unlock(&mutex_window_size);
}

//Côté afficheur :
void attendreTailleFenetre()
{
    pthread_mutex_lock(&mutex_window_size);
    while (!is_sent_size)
    {
        pthread_cond_wait(&cond_size, &mutex_window_size); //these functions atomically release mutex and cause the calling thread to block on the condition variable cond
    }
    is_sent_size = false;
    pthread_mutex_unlock(&mutex_window_size);
}

//Côté afficheur
void signalerFenetreEtTexturePrete()
{
    pthread_mutex_lock(&mutex_window_texture);

    is_sent_texture = true;
    pthread_cond_signal(&cond_texture);
    pthread_mutex_unlock(&mutex_window_texture);
}

//Côté décodeur
void attendreFenetreTexture()
{
    pthread_mutex_lock(&mutex_window_texture);
    while (!is_sent_texture)
    {
        pthread_cond_wait(&cond_texture, &mutex_window_texture);
    }
    is_sent_texture = false;
    pthread_mutex_unlock(&mutex_window_texture);
}

// Coté consommateur
void debutConsommerTexture() // On attend jusqu'à ce qu'il y ait une texture à consommer
{
  pthread_mutex_lock(&mutex_window_prod);
  while(texture_number == 0){
    pthread_cond_wait(&cond_empty_prod, &mutex_window_prod);
  }
  pthread_mutex_unlock(&mutex_window_prod);
}

void finConsommerTexture() // On consomme une texture
{
  pthread_mutex_lock(&mutex_window_prod);
  texture_number--;
  pthread_cond_signal(&cond_full_prod);
  signalerFenetreEtTexturePrete();
  pthread_mutex_unlock(&mutex_window_prod);
}

// Coté producteur
void debutDeposerTexture() // On attend jusqu'à ce qu'il y ait de la place
{
  pthread_mutex_lock(&mutex_window_prod);
  while(texture_number == NBTEX){
    pthread_cond_wait(&cond_full_prod, &mutex_window_prod);
  }
  pthread_mutex_unlock(&mutex_window_prod);
}

void finDeposerTexture() // On ajoute une texture
{
  pthread_mutex_lock(&mutex_window_prod);
  texture_number++;
  pthread_cond_signal(&cond_empty_prod);
  pthread_mutex_unlock(&mutex_window_prod);
}

void init(){
  pthread_mutex_init(&mutex_for_hashmap, NULL);

  pthread_mutex_init(&mutex_window_size, NULL);
  pthread_cond_init(&cond_size, NULL);
  is_sent_size = false;

  pthread_mutex_init(&mutex_window_texture, NULL);
  pthread_cond_init(&cond_texture, NULL);
  is_sent_texture = false;

  pthread_mutex_init(&mutex_window_prod, NULL);
  pthread_cond_init(&cond_full_prod, NULL);
  pthread_cond_init(&cond_empty_prod, NULL);
  texture_number = 0;
}

void destroy(){
  pthread_mutex_destroy(&mutex_for_hashmap);

  pthread_mutex_destroy(&mutex_window_size);
  pthread_cond_destroy(&cond_size);

  pthread_mutex_destroy(&mutex_window_texture);
  pthread_cond_destroy(&cond_texture);

  pthread_mutex_destroy(&mutex_window_prod);
  pthread_cond_destroy(&cond_full_prod);
  pthread_cond_destroy(&cond_empty_prod);
}
