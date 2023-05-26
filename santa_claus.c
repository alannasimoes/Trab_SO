#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutexRenas;
pthread_mutex_t mutexPapaiNoel;
pthread_mutex_t mutexElfos;
pthread_mutex_t mutex;

pthread_cond_t vcPapaiNoel;
pthread_cond_t vcRenas;
pthread_cond_t vcElfos;

//Condições de parada
int renas = 0;
int elfos = 0;

//Total de threads
int numRenas = 9; //Default 9 renas 
int numElfos = 0;

void *papaiNoel();
void *rena();
void *elfo();

int main() { 
    printf("Quantos elfos estão trabalhando? ");
    scanf("%d", &numElfos);

    pthread_t threadsRenas[numRenas];
    pthread_t threadsElfos[numElfos];
    pthread_t threadPapaiNoel;

    pthread_mutex_init(&mutexRenas, NULL);
    pthread_mutex_init(&mutexPapaiNoel, NULL);
    pthread_mutex_init(&mutexElfos, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_init(&vcPapaiNoel, NULL);
    pthread_cond_init(&vcRenas, NULL);
    pthread_cond_init(&vcElfos, NULL);

    pthread_create(&threadPapaiNoel, NULL, papaiNoel, NULL);

    for (int i = 0; i < numElfos; i++){ 
        pthread_create(&threadsElfos[i], NULL, elfo, NULL);
    }

    for (int i = 0; i < numRenas; i++){     //Refatorar a criação de renas para ser aleatório
        pthread_create(&threadsRenas[i], NULL, rena, NULL); //Inicializando as renas
    }

    for (int i = 0; i < numRenas; i++){
        pthread_join(threadsRenas[i], NULL);
    }

    for (int i = 0; i < numElfos; i++){
        pthread_join(threadsElfos[i], NULL);
    }

    pthread_join(threadPapaiNoel, NULL); 

    pthread_mutex_destroy(&mutexRenas);
    pthread_mutex_destroy(&mutexPapaiNoel);
    pthread_mutex_destroy(&mutexElfos);
    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&vcPapaiNoel);
    pthread_cond_destroy(&vcRenas);
    pthread_cond_destroy(&vcElfos);

    printf("Papai Noel saiu.\n");
    return 0;
}

void *elfo(){
    pthread_mutex_lock(&mutexElfos);
    pthread_mutex_lock(&mutex);
    
    elfos++;
    printf("Um elfo pediu ajuda\n");

    if(elfos == 3){
        printf("Três elfos precisam de ajuda!\n");
        pthread_cond_signal(&vcPapaiNoel); //Envia o sinal pro Papai Noel acordar
        pthread_mutex_unlock(&mutex);
    }
    else{
        pthread_mutex_unlock(&mutex);
        pthread_cond_wait(&vcElfos, &mutexElfos);
    }

    pthread_mutex_unlock(&mutexElfos);

    if(elfos==0){
        pthread_mutex_unlock(&mutexElfos);
    }
    pthread_cond_signal(&vcElfos);

    pthread_exit(NULL);
}

void *rena(){
    pthread_mutex_lock(&mutex);
    renas++;   
    printf("Uma rena chegou ao polo norte. Número de renas aguardado: %d.\n", renas);

    if (renas == 9){
        printf("Todas as renas chegaram ao Polo Norte.\n");
        pthread_cond_signal(&vcPapaiNoel); //Envia o sinal pro Papai Noel acordar
    }
    pthread_mutex_unlock(&mutex);

    pthread_cond_wait(&vcRenas, &mutexRenas);
    printf("Rena amarrada\n");
    pthread_mutex_unlock(&mutexRenas);
    pthread_cond_signal(&vcRenas);

    pthread_exit(NULL);

}

void *papaiNoel(){
    while(renas != 9){
        pthread_mutex_lock(&mutexPapaiNoel);
        pthread_cond_wait(&vcPapaiNoel, &mutexPapaiNoel);
        pthread_mutex_lock(&mutex);

        if(elfos == 3){
            printf("Elfos ajudados!\n");
            elfos -= 3;
            pthread_cond_signal(&vcElfos);
        }
        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&mutexPapaiNoel);
    }
    printf("Papai Noel acordou e está saindo.\n");
    pthread_cond_signal(&vcRenas);
    pthread_cond_signal(&vcElfos);

    pthread_exit(NULL);
}