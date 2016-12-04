#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include "stream_common.h"
#include "oggstream.h"


int main(int argc, char *argv[]) {
    int res;

    if (argc != 2) {
	fprintf(stderr, "Usage: %s FILE", argv[0]);
	exit(EXIT_FAILURE);
    }
    assert(argc == 2);


    // Initialisation de la SDL
    res = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS);
    atexit(SDL_Quit);
    assert(res == 0);

    // start the two stream readers

    if (pthread_create(&tStreamR, NULL, theoraStreamReader, argv[1])!=0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_create(&vStreamR, NULL, vorbisStreamReader , argv[1])!=0) {
        perror("pthread_create");
        exit(1);
    }

    // wait audio thread

    pthread_join(vStreamR, NULL);

    // 1 seconde de garde pour le son,
    sleep(1);

    // tuer les deux threads videos si ils sont bloqués
    pthread_detach(theora2sdlthread);
    pthread_detach(tStreamR);

    // attendre les 2 threads videos
    pthread_join(theora2sdlthread,NULL);
    pthread_join(tStreamR, NULL);

    exit(EXIT_SUCCESS);
}
