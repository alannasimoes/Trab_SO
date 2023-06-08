#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutexRenas;
pthread_mutex_t mutexPapaiNoel;
pthread_mutex_t mutexElfos;
pthread_mutex_t mutex;

pthread_cond_t vcPapaiNoel;
pthread_cond_t vcRenas;
pthread_cond_t vcElfos;

//Total de threads
int numRenas = 9;
int numElfos = 0;

//Contagem de renas e elfos
int renas = 0;
int elfos = 0;

void *papaiNoel();
void *rena();
void *elfo();
void manipularThreads();

int main() { 
    //Usuário define a quantidade de elfos
    printf("Quantos elfos estão trabalhando? ");
    scanf("%d", &numElfos);

    //Inicializa os mutex
    pthread_mutex_init(&mutexRenas, NULL);
    pthread_mutex_init(&mutexPapaiNoel, NULL);
    pthread_mutex_init(&mutexElfos, NULL);
    pthread_mutex_init(&mutex, NULL);

    //Inicializa as variáveis de condição
    pthread_cond_init(&vcPapaiNoel, NULL);
    pthread_cond_init(&vcRenas, NULL);
    pthread_cond_init(&vcElfos, NULL); 

    //Cria as threads e espera elas terminarem de executar
    manipularThreads();

    //Destroi os mutex
    pthread_mutex_destroy(&mutexRenas);
    pthread_mutex_destroy(&mutexPapaiNoel);
    pthread_mutex_destroy(&mutexElfos);
    pthread_mutex_destroy(&mutex);

    //Detroi as variáveis de condição
    pthread_cond_destroy(&vcPapaiNoel);
    pthread_cond_destroy(&vcRenas);
    pthread_cond_destroy(&vcElfos);

    //Finaliza o programa
    printf("Papai Noel saiu!\n");
    return 0;
}

void *elfo(){
    pthread_mutex_lock(&mutexElfos); //Bloqueia o lock dos elfos
    pthread_mutex_lock(&mutex); //Bloqueia o mutex geral

    elfos++; //Incrementa a contagem de elfos

    //Quando há 3 elfos esperando ajuda, o terceiro elfo
    //envia o sinal para o Papai Noel acordar
    if(elfos == 3){
        printf("Três elfos precisam de ajuda!\n");
        pthread_cond_signal(&vcPapaiNoel);
    }

    //Se ainda não há 3 elfos esperando ajuda, o mutex 
    //é liberado para que outros elfos entrem
    else{
        pthread_mutex_unlock(&mutexElfos);
    }

    pthread_cond_wait(&vcElfos, &mutex); //Espera a ajuda do Papai Noel
    
    //Se todas as renas tiverem chegado, os locks são liberados
    //para que o programa consiga encerrar 
    //(as threads encerram antes dos elfos serem ajudados)
    if (renas == 9){
        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&mutexElfos);
        pthread_cond_signal(&vcElfos);
        pthread_exit(NULL);
    }
    
    printf("Elfo ajudado!\n");
    
    pthread_cond_signal(&vcElfos); //Envia sinal para que o próximo elfo receba ajuda

    elfos--; //Decrementa a quantidade de elfos, pois já recebeu ajuda

    //Quando não há mais elfos para serem ajudados, o mutex dos elfos é liberado
    //e os elfos aguardando podem executar (evitando mais de 3 elfos nessa região)
    if(elfos==0){
        pthread_mutex_unlock(&mutexElfos);
    }

    //Desbloqueia o mutex geral e encerra a thread
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void *rena(){
    pthread_mutex_lock(&mutex); //Bloqueia o mutex geral
    renas++; //Incrementa a quantidade de renas
    printf("Uma rena chegou ao polo norte. Renas aguardando: %i\n", renas);

    //Quando todas as 9 renas chegam, a última
    //envia o sinal pro Papai Noel acordar
    if (renas == 9){
        printf("Todas as renas chegaram ao Polo Norte\n");
        pthread_cond_signal(&vcPapaiNoel);
    }
    pthread_mutex_unlock(&mutex); //Desbloqueia o mutex geral

    pthread_mutex_lock(&mutexRenas); //Bloqueia o mutex das renas
    pthread_cond_wait(&vcRenas, &mutexRenas); //Aguarda o sinal do Papai Noel
    printf("Rena amarrada!\n");
    pthread_mutex_unlock(&mutexRenas); //Desbloqueia o mutex das renas
    pthread_cond_signal(&vcRenas); //Envia um sinal para que a próxima rena possa ser amarrada

    pthread_exit(NULL); //Encerra a thread

}

void *papaiNoel(){
    while(1){
        pthread_mutex_lock(&mutexPapaiNoel); //Bloqueia o mutex do Papai Noel
        pthread_cond_wait(&vcPapaiNoel, &mutexPapaiNoel); //Aguarda o sinal para que seja acordado
        pthread_mutex_lock(&mutex); //Bloqueia o mutex geral

        //Primeiro verifica a quantidade de renas, pois caso
        //todas já tenham chegado, amarrá-las é prioridade
        if(renas == 9){
            printf("Papai Noel acordou e está saindo!\n");
            pthread_cond_signal(&vcRenas); //Envia sinal para que as renas sejam amarradas
            pthread_mutex_unlock(&mutex); //Desbloqueia o mutex para permitir que as renas sejam amarradas
            pthread_cond_signal(&vcElfos); //Envia sinal para os elfos, para permitir finalizar o programa

            pthread_exit(NULL); //Thread finaliza a execução
        }

        //Quando há 3 elfos esperando, o Papai Noel acorda para ajudá-los
        else if(elfos == 3){
            printf("Papai Noel acordou para ajudar os elfos!\n");
            pthread_cond_signal(&vcElfos); //Envia sinal para que 3 elfos sejam ajudados
        }

        pthread_mutex_unlock(&mutex); //Desbloqueia o mutex
        pthread_mutex_unlock(&mutexPapaiNoel); //Desbloqueia o mutex do Papai Noel
    }
}

void manipularThreads(){
    pthread_t threadsRenas[numRenas];
    pthread_t threadsElfos[numElfos];
    pthread_t threadPapaiNoel;

    //Cria a thread do Papai Noel
    pthread_create(&threadPapaiNoel, NULL, papaiNoel, NULL); 

    //Cria as threads dos elfos
    for (int i = 0; i < numElfos; i++){ 
        pthread_create(&threadsElfos[i], NULL, elfo, NULL);
    }

    //Cria as threads das renas
    for (int i = 0; i < numRenas; i++){ 
        pthread_create(&threadsRenas[i], NULL, rena, NULL); 
    }

    //Aguarda a thread do Papai Noel finalizar
    pthread_join(threadPapaiNoel, NULL);

    //Aguarda as threads das renas finalizarem
    for (int i = 0; i < numRenas; i++){
        pthread_join(threadsRenas[i], NULL);
    }

    //Aguarda as threads dos elfos finalizarem
    for (int i = 0; i < numElfos; i++){
        pthread_join(threadsElfos[i], NULL);
    }
}