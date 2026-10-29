#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	// crear los semaforos
	const char s1[]= "/SEM1";
	const char s2[]= "/SEM2";

	if (sem_unlink(s1) < 0 || sem_unlink(s2) < 0){
		perror("Error en sem_unlink()\n");
	}
	
	sem_t *sem1 = sem_open(s1, O_CREAT, 0666, 1);
	sem_t *sem2 = sem_open(s2, O_CREAT, 0666, 0);

	if (sem1 == SEM_FAILED || sem2 == SEM_FAILED){
		perror("Error en sem_open()\n");
		return 1;
	}

	//Entrar al área de memoria compartida por p3
	const int tamano = sizeof(int);
	const char nombre_recurso[] = "/SHR";

	int fd = shm_open(nombre_recurso, O_RDWR, 0666);
	int* ptr = (int*)mmap(0, tamano, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (argc != 3 ){
		printf("Se necesitan los 3 argumentos\n");
		return 1;
	}
	
	int N = atoi(argv[1]);
	int times = atoi(argv[2]);
	int valor_leido = 0;

	for (int i = 0; i < times; i++){
		sem_wait(sem1);
		if (i == 0 ){
			*ptr = N;
			printf("Enviando:%d\n", N);
		} else {
			valor_leido = *ptr;   
			printf("Recibido: %d\n", valor_leido);
			
			int enviar = valor_leido + 1;
			N = N + valor_leido;
			*ptr = enviar;
			printf("Enviando: %d\n", enviar);
		}
		sem_post(sem2);
	}
	sem_wait(sem1);
	*ptr = -1;
	sem_post(sem2);
	printf("Enviando: -1\n");
	printf("Termina\n");

	munmap(ptr, tamano);
    close(fd);
    sem_close(sem1);
    sem_close(sem2);
	shm_unlink(nombre_recurso);
	return 0;
}
