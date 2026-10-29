#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>

int main(){
	const char s1[] = "/SEM1";
	const char s2[] = "/SEM2";

	sem_t *sem1 = sem_open(s1, O_CREAT, 0666, 1);
	sem_t *sem2 = sem_open(s2, O_CREAT, 0666, 0);

	const int tamano = sizeof(int);
	const char nombre_recurso[] = "/SHR";

	int fd = shm_open(nombre_recurso, O_RDWR, 0666);
	int* ptr = (int*)mmap(0, tamano, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);
	
	int valor_leido = 0;
	int acumulado = 0;
	while(1){
		sem_wait(sem2);
		valor_leido = *((int*)ptr);
		if (valor_leido == -1){
			printf("Recibido: %d\n",valor_leido);
			printf("Termina\n");
			break;
		}
		printf("Recibido: %d\n", valor_leido);
		int enviar = valor_leido + 1;
		
		*ptr = enviar;
		printf("Enviado: %d\n",enviar);
		sem_post(sem1);
	}
	return 0;
}
