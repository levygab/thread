#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <pthread.h>

#include "stream_common.h"
#include "oggstream.h"
#include "synchro.h"

int main(int argc, char *argv[])
{
  int res;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s FILE", argv[0]);
    exit(EXIT_FAILURE);
  }
  assert(argc == 2);

  // Initialisation de la SDL
  res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
  atexit(SDL_Quit);
  assert(res == 0);

  // start the two stream readers

  /*Q1 : Dans la fonction int main(int argc, char *argv[]) du fichier ensivideo.c,
    ajouter le lancement de deux threads qui exécutent, chacun, une des fonctions
    theoraStreamReader et vorbisStreamReader. Chacun reçoit en argument le nom
    du fichier à lire (argv[1]).*/

  // Initialiser les mutex et les variables de condition
  init();

  pthread_t thread_for_theora;
  pthread_t thread_for_vorbis;

  /*To create a new thread : int pthread_create(pthread_t *restrict thread,const pthread_attr_t *restrict attr,
    void *(*start_routine)(void *),void *restrict arg);*/
  /*void *theoraStreamReader(void *arg); defined in oggstream.h*/

  res = pthread_create(&thread_for_theora, NULL, theoraStreamReader, argv[1]);
  assert(res == 0); /*On success, pthread_create() returns 0*/
  res = pthread_create(&thread_for_vorbis, NULL, vorbisStreamReader, argv[1]);
  assert(res == 0);

  // wait audio thread
  /*Il faudra ensuite, dans la fonction main, attendre la terminaison du thread vorbis (audio)*/
  /*int pthread_join (pthread_t th, void ** thread_return);: fonction qui met en attente le pg main jusqu'à ce que theora_for_vorbis se termine*/

  if (pthread_join(thread_for_vorbis, NULL) == 0)
  {
    printf("La terminaison du thread vorbis est bien en attente\n");
  }

  // 1 seconde de garde pour le son,
  sleep(1);

  // tuer les deux threads videos si ils sont bloqués
  /*utiliser pthread_cancel: int pthread_cancel(pthread_t thread); */
  if (pthread_cancel(thread_for_theora) == 0)
  {
    printf("Le thread vidéo theora est bien tué \n");
  }

  if (pthread_cancel(theora2sdlthread) == 0)
  {
    printf("Le deuxième thread vidéo est bien tué \n");
  }

  // attendre les 2 threads videos
  if (pthread_join(thread_for_theora, NULL) == 0)
  {
    printf("La terminaison du thread theora est bien en attente\n");
  }
  if (pthread_join(theora2sdlthread, NULL) == 0)
  {
    printf("La terminaison du deuxième thread vidéo est bien en attente\n");
  }

  // Detruire les mutex et les variables de condition
  destroy();

  exit(EXIT_SUCCESS);
}
