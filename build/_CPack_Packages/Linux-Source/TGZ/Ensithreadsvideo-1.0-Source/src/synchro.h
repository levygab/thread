#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <stdbool.h>
#include <pthread.h>
#include "ensitheora.h"

extern bool fini;
extern pthread_t theora2sdlthread;
extern pthread_mutex_t mutex_for_hashmap;


/* Les extern des variables pour la synchro ici */


/* Fonctions de synchro à implanter */

void envoiTailleFenetre(th_ycbcr_buffer buffer);
void attendreTailleFenetre();

void attendreFenetreTexture();
void signalerFenetreEtTexturePrete();

void debutConsommerTexture();
void finConsommerTexture();

void debutDeposerTexture();
void finDeposerTexture();

void init();
void destroy();
#endif
