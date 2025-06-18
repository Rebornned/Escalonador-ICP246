#include "icp246_libs.h"

// (RE) Inicializa a fila
void initQueue(Queue *queue) {
    memset(queue, 0, sizeof(Queue)); // Reseta todos os campos da fila.
}

// Enfileira um novo processo na fila
int enqueueProcess(Queue *queue, Process process) {
    if(!isFull(queue)) {
        queue->data[queue->rear] = process; // Último elemento da fila
        queue->rear = (queue->rear+1) % PROCESS_LIMIT; // Lógica circular
        queue->size += 1;
        return TRUE; // Operação bem sucedida
    }    
    printf("Não é possível empilhar o processo: %s | Fila cheia.\n", process.name);
    return FALSE; // Fila cheia
}

// Desinfileira o primeiro elemento da fila
Process dequeueProcess(Queue *queue) {
    if(!isEmpty(queue)) {
        Process pGetted = queue->data[queue->front]; // Primeiro elemento da fila
        queue->front = (queue->front+1) % PROCESS_LIMIT; // Lógica circular
        queue->size -= 1;
        return pGetted; // Retorna o processo capturado;
    }
    printf("Não é possível desenfileirar o processo, Fila vazia.\n");
    Process empty;
    empty.isActive = FALSE;
    return empty;
}

Process getFrontProcess(Queue *queue) {
    Queue cloneQueue;
    memcpy(&cloneQueue, queue, sizeof(Queue)); // Cria um clone da fila, para não altera-la ao exibir.
    if(!isEmpty(queue)) {
        Process pGetted = cloneQueue.data[queue->front]; // Primeiro elemento da fila
        return pGetted; // Retorna o processo capturado;
    }
    printf("Não é possível desenfileirar o processo, Fila vazia.\n");
    Process empty;
    empty.isActive = FALSE;
    return empty;
}

// Verifica se a fila está cheia
int isFull(Queue *queue) {
    if(queue->size == PROCESS_LIMIT)
        return TRUE; // Fila cheia
    return FALSE; // Fila contém espaços
}

// Verifica se a fila está vazia
int isEmpty(Queue *queue) {
    if(queue->size == 0) 
        return TRUE; // Fila vazia
    return FALSE; // Fila contém elementos
}

void displayQueue(Queue *queue, int details) {
    Queue cloneQueue;
    memcpy(&cloneQueue, queue, sizeof(Queue)); // Cria um clone da fila, para não altera-la ao exibir.
    if(!isEmpty(queue)) {
        if(details == TRUE) {
            printf("=====================================================================\n");
            printf("Front: %d | Rear: %d | Tamanho atual: %d\n", queue->front, queue->rear, queue->size);
            printf("=====================================================================\n");
            for(int i=0; i < queue->size; i++) { // Percorre a fila inteira exibindo cada processo presente.
                Process curr = dequeueProcess(&cloneQueue);
                printf("Nome: %s | ID: %d | Ciclos totais: %d | Ciclos restantes: %d | Ativo: %d | Entrada em %d ciclos\n", 
                curr.name, curr.id, curr.totalCycles, curr.remainingCycles, curr.isActive, curr.arrivalCycles);
                printf("Prioridade: %d | After cycles: %d | type: %d | Ativo: %d | Requerido: %d | Ciclos I/O: %d\n",
                curr.request.afterPriority, curr.request.afterCycles, curr.request.type, curr.request.isActive, curr.request.isRequired, curr.request.remainingCycles);
            }
            printf("*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n");
            return;
        }
        else {
            printf("[");
            for(int i=0; i < queue->size; i++) { // Percorre a fila inteira exibindo cada processo presente.
                Process curr = dequeueProcess(&cloneQueue);
                printf("| ID: %d ", curr.id);
            }
            printf("]\n");

        }
    }
    else
        printf("Fila vazia, nada a imprimir.\n");
}
