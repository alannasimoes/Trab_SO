#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutexRenas;
pthread_mutex_t mutexPapaiNoel;
pthread_cond_t condicionalPapaiNoel;
pthread_cond_t condicionalRenas;

//Condições de parada
int renas = 0;

//Total de threads
int numRenas = 0; //Default 9 renas 

void *papaiNoel();
void *rena();

int main() { 
    printf("Quantas renas estão fora do polo norte?\n");
    scanf("%d", &numRenas);

    pthread_t threadsRenas[numRenas];
    pthread_t threadPapaiNoel;

    pthread_mutex_init(&mutexRenas, NULL);
    pthread_mutex_init(&mutexPapaiNoel, NULL);
    pthread_cond_init(&condicionalPapaiNoel, NULL);
    pthread_cond_init(&condicionalRenas, NULL);

    pthread_create(&threadPapaiNoel, NULL, papaiNoel, NULL);

    for (int i = 0; i < numRenas; i++) {     //Refatorar a criação de renas para ser aleatório
        pthread_create(&threadsRenas[i], NULL, rena, NULL); //Inicializando as renas
    }

    for (int i = 0; i < numRenas; i++) {
        pthread_join(threadsRenas[i], NULL);
    }

    pthread_join(threadPapaiNoel, NULL); 

    pthread_mutex_destroy(&mutexRenas);
    pthread_mutex_destroy(&mutexPapaiNoel);
    pthread_cond_destroy(&condicionalPapaiNoel);
    pthread_cond_destroy(&condicionalRenas);

    printf("Fim do programa.\n");
    return 0;
}


void *rena(){
    pthread_mutex_lock(&mutexRenas);
    renas++;   

    printf("Uma rena chegou ao polo norte. Número de renas aguardado: %d.\n", renas);

    if (renas == numRenas) {
        printf("Todas as renas chegaram ao Polo Norte.\n");
        pthread_cond_signal(&condicionalRenas); //Envia o sinal pro Papai Noel acordar
    }

    pthread_mutex_unlock(&mutexRenas);

    pthread_exit(NULL);

}

void *papaiNoel(){
    int iteracoes = 0;
    while(iteracoes<100){
        pthread_mutex_lock(&mutexPapaiNoel);

        while (renas > 0) {
            pthread_cond_wait(&condicionalRenas, &mutexPapaiNoel);
            printf("Papai Noel acordou e está liberando as renas.\n");
        }

        if(renas == numRenas){
            pthread_mutex_lock(&mutexRenas);
            renas -= numRenas;
            pthread_mutex_unlock(&mutexRenas);
        }

        pthread_mutex_unlock(&mutexPapaiNoel);
        iteracoes++;
    }
}
