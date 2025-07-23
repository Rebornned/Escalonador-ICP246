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

// Mostra todos os elementos da fila, e cria um log dos seus dados, pode ser detalhado ou não
void displayQueue(FILE *log, Queue *queue, int details) {
    Queue cloneQueue;
    memcpy(&cloneQueue, queue, sizeof(Queue)); // Cria um clone da fila, para não altera-la ao exibir.
    if(!isEmpty(queue)) {
        if(details == TRUE) { // Impressão da fila detalhada
            logPrintf(log, "=====================================================================\n");
            logPrintf(log, "              Front: %d | Rear: %d | Tamanho atual: %d\n", queue->front, queue->rear, queue->size);
            logPrintf(log, "=====================================================================\n");
            for(int i=0; i < queue->size; i++) { // Percorre a fila inteira exibindo cada processo presente.
                Process curr = dequeueProcess(&cloneQueue);
                logPrintf(log, "| Processo -> Nome: %s | ID: %d | Ciclos totais: %d | Ciclos restantes: %d | Ativo: %d | Entrada em %d ciclos\n", 
                curr.name, curr.id, curr.totalCycles, curr.remainingCycles, curr.isActive, curr.arrivalCycles);
                logPrintf(log, "| I/O -> Prioridade: %d | Ciclos até I/O: %d | Tipo: %d | Ativo: %d | Requerido: %d | Ciclos I/O: %d\n",
                curr.request.afterPriority, curr.request.afterCycles, curr.request.type, curr.request.isActive, curr.request.isRequired, curr.request.remainingCycles);
                logPrintf(log, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n");
            }
            return;
        }
        else { // Impressão da fila de forma mais simplificada
            logPrintf(log, "[");
            for(int i=0; i < queue->size; i++) { // Percorre a fila inteira exibindo cada processo presente.
                Process curr = dequeueProcess(&cloneQueue);
                logPrintf(log, "| ID: %d ", curr.id);
            }
            logPrintf(log, "]\n");

        }
    }
    else
        logPrintf(log, "Fila vazia, nada a imprimir.\n");
}
