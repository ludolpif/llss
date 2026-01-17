#include "app.h"

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>

void thread_inotify_main_func();
static void handle_events(int fd, int *wd, int argc, char* argv[]); 
void push_inotify_event_to_sdl_queue(const struct inotify_event *ievent);

int main(int argc, char* argv[]) {
    char buf;
    int fd, i, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];

    if (argc < 2) {
        printf("Utilisation : %s CHEMIN [CHEMIN ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Appuyer sur la touche Entrée pour quitter.\n");

    /* Créer le descripteur de fichier pour accéder à l’interface de programmation inotify. */
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }

    /* Allouer la mémoire pour les descripteurs de surveillance. */
    wd = calloc(argc, sizeof(int));
    if (wd == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Marquer les répertoires pour les événements :
       - un fichier a été ouvert ;
       - un fichier a été fermé
       */
    for (i = 1; i < argc; i++) {
        wd[i] = inotify_add_watch(fd, argv[i], IN_OPEN | IN_CLOSE);
        if (wd[i] == -1) {
            fprintf(stderr, "Impossible de surveiller « %s » : %s\n",
                    argv[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    /* Préparer pour la scrutation (polling). */

    nfds = 2;

    fds[0].fd = STDIN_FILENO;       /* Entrée de console */
    fds[0].events = POLLIN;
    fds[1].fd = fd;                 /* Entrée d'inotify */
    fds[1].events = POLLIN;

    /* Attendre les événements ou une entrée du terminal. */
    printf("En écoute d’événements.\n");
    while (1) {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (poll_num > 0) {

            if (fds[0].revents & POLLIN) {
                /* Entrée de console disponible.
                   Vider l’entrée standard et quitter. */
                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN) {
                /* Des événements inotify sont disponibles. */
                handle_events(fd, wd, argc, argv);
            }
        }
    }

    printf("Arrêt de l’écoute d’événements.\n");
    /* Fermer le descripteur de fichier inotify. */
    close(fd);

    free(wd);
    exit(EXIT_SUCCESS);
}

/* Lire tous les événements inotify disponibles à partir du descripteur de fichier « fd ».
   wd est le tableau des descripteurs de surveillance pour les répertoires en argv.
   argc est la taille de wd et argv.
   argv est la liste des répertoires surveillés.
   L’entrée 0 de wd et argv n’est pas utilisée. */

// Code from man inotify
static void handle_events(int fd, int *wd, int argc, char* argv[]) {
    char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;

    /* Boucler tant que les événements peuvent être lus à partir du
       descripteur de fichier inotify */
    for (;;) {
        /* Lire certains événements. */
        len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* Si le read() non bloquant n’a pas trouvé d’événement à
           lire, il renvoie -1 avec errno défini à EAGAIN. Dans ce
           cas, on sort de la boucle. */
        if (len <= 0)
            break;

        /* Boucler sur tous les événements du tampon. */
        for (char *ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (const struct inotify_event *) ptr;
            push_inotify_event_to_sdl_queue(event);
        }
    }
}
