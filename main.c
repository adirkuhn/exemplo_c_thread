#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "funcs.h"

#define TAM 3

pthread_t produtor1, produtor2, consumidor1, consumidor2;

pthread_mutex_t ex_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tem_vaga_buffer = PTHREAD_COND_INITIALIZER;
pthread_cond_t tem_dados_buffer = PTHREAD_COND_INITIALIZER;

int id[] = {0, 1, 2, 3};
double buffer[TAM];
int prox_inserir = 0;
int prox_retirar = 0;
int num_dados_buffer = 0;

int main(void)
{

    pthread_create(&produtor1, NULL, (void *) produtor, &id[0]);
    pthread_create(&produtor2, NULL, (void *) produtor, &id[1]);
    pthread_create(&consumidor1, NULL, (void *) consumidor, &id[2]);
    pthread_create(&consumidor2, NULL, (void *) consumidor, &id[3]);

    pthread_join(produtor1, NULL);
    pthread_join(produtor2, NULL);
    pthread_join(consumidor1, NULL);
    pthread_join(consumidor2, NULL);

}

void produtor(int *id) {
    double x = 0;
    while(1) {
        x += 1 + *id;
        inserir(x);
    }
}

void consumidor(int *id) {
    double x = 0;
    while(1) {
        x = retirar();
        printf("Proc: %d - Valor retirado do buffer: %lf\n", *id, x);
    }
}

void inserir(double x) {
    pthread_mutex_lock(&ex_mut);

    sleep(1);

    while(num_dados_buffer == TAM)
        pthread_cond_wait(&tem_vaga_buffer, &ex_mut);

    buffer[prox_inserir] = x;
    printf("Inserindo %lf no buffer[%d]\n", x, prox_inserir);
    prox_inserir = (prox_inserir +1) % TAM;
    ++num_dados_buffer;

    pthread_cond_signal(&tem_dados_buffer);

    pthread_mutex_unlock(&ex_mut);
}

double retirar(void) {
    double x;

    pthread_mutex_lock(&ex_mut);

    sleep(2);

    while(num_dados_buffer == 0)
        pthread_cond_wait(&tem_dados_buffer, &ex_mut);

    x = buffer[prox_retirar];
    prox_retirar = (prox_retirar + 1) % TAM;
    --num_dados_buffer;

    pthread_cond_signal(&tem_vaga_buffer);

    pthread_mutex_unlock(&ex_mut);

    return x;
}
