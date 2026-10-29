/* Autor: jfmunozf */

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>


void handler_senal (int signum) {
    printf("\nDespertando....\n");
}

int main(int argc, char *argv[]) {

    const int tamano = sizeof(int);
    const char nombre_recurso[] = "/SHR";

    printf("Armando senal SIGINT...\n");
    if (signal (SIGINT, handler_senal) == SIG_ERR) {
        perror("Error en signal()\n");
        return 1;
    }

    shm_unlink(nombre_recurso);

    int fd = shm_open(nombre_recurso, O_CREAT | O_TRUNC | O_RDWR, 0666);

    if (fd < 0) {
        perror("Error en shm_open()\n");
        return 2;
    }

    if (ftruncate(fd, tamano) < 0) {
        perror("Error en ftruncate()\n");
        close(fd);
        return 3;
    }

    void * ptr = mmap(0, tamano, 
                      PROT_READ | PROT_WRITE, 
                      MAP_SHARED,
                      fd,
                      0);

    if (ptr == MAP_FAILED) {
        perror("Error en mmap()\n");
        close(fd);
        return 3;
    }

    printf("Recurso instalado\nEste proceso a dormir...\nUse [CTRL+C] para despertar\n");
    pause();

    printf("Liberando recursos...\nTerminado\n");

    munmap(ptr, tamano);
    close(fd);
    shm_unlink(nombre_recurso);

    return 0;
}
