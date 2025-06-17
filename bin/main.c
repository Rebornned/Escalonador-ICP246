#include "icp246_libs.h"

// Linha de acesso CD: C:/Users/Dhemerson/Desktop/Programacao/GKT_C/Escalonador-ICP246/bin
// Linha de compilação: gcc -g -fexec-charset=UTF-8 -o  main.exe main.c queue_libs.c process_libs.c

int main() {
    setlocale(LC_ALL, ""); // Seta o local para não haver caracteres estranhos exibidos no terminal
    srand(time(NULL)); // Semente de randomização
    // ----------------------------------------------------
    //           Declaração e inicialização de filas
    // ----------------------------------------------------
    Queue highQueue, lowQueue, ioQueue;
    initQueue(&highQueue);
    initQueue(&lowQueue);
    initQueue(&ioQueue);
    // ----------------------------------------------------
    //              Gerando processos aleatórios
    // ----------------------------------------------------
    for(int i=0; i<PROCESS_LIMIT; i++) {
        Process newProcess = randomNewProcess();
        if(newProcess.priority == HIGH_PRIORITY)
            enqueueProcess(&highQueue, newProcess);
        else if(newProcess.priority == LOW_PRIORITY)
            enqueueProcess(&lowQueue, newProcess);
    }
    // ----------------------------------------------------
    //                      Simulação
    // ----------------------------------------------------
    printf("==================================================\n");
    printf("FILA DE ALTA PRIORIDADE \n");
    displayQueue(&highQueue, TRUE);
    printf("FILA DE BAIXA PRIORIDADE \n");
    displayQueue(&lowQueue , TRUE);
    startSimulation(&highQueue, &lowQueue, &ioQueue);
    return 0;
}

int startSimulation(Queue *highQueue, Queue* lowQueue, Queue *ioQueue) {
    int currentCycle = 0; // Seta o ciclo inicial
    int breakPoint = FALSE;
    int quantumRemaining = QUANTUM_CLOCK;
    char *ioTypes[3] = {"Disco", "Fita magnética", "Impressora"}; // Tradução dos index I/O para strings
    char *boolTypes[2] = {"Falso", "Verdadeiro"}; // Tradução dos index booleanos para strings
    Process currentProcess, currentIO; // Variaveis para absorver os processos e I/Os atuais
    printf("##############################################################\n");
    printf("                    INICIANDO SIMULAÇÃO\n");
    printf("##############################################################\n");
    while(highQueue->size != 0 || lowQueue->size != 0 || ioQueue->size != 0) { // Enquanto as 3 listas terem processos, o simulador roda.
        if(quantumRemaining == 0) // Repõe o quantum de clock
            quantumRemaining = QUANTUM_CLOCK;
        if(highQueue->size > 0) { // Verifica se a fila de prioridade contém elementos
            currentProcess = dequeueProcess(highQueue);
        }
        else if(lowQueue->size > 0) { // Verifica se a fila de baixa contém elementos
            currentProcess = dequeueProcess(lowQueue);
        }

        // Loop de clock
        while (quantumRemaining > 0) {
            currentCycle++; // Atualiza o contador em 1 ciclo
            printf("=+=+=+=+=+=++=+=+=+=+=++=+=+=+=+=+=++=+=+=+=+=+=++=+=+=+=+=+=+\n");
            printf("                CICLO ATUAL: %d | QUANTUM: %d\n", currentCycle, quantumRemaining);
            printf("**************************************************************\n");
            printf("FILA DE ALTA PRIORIDADE: ");
            displayQueue(highQueue , FALSE);
            printf("FILA DE BAIXA PRIORIDADE: ");
            displayQueue(lowQueue , FALSE);
            printf("FILA I/O: ");
            displayQueue(ioQueue , FALSE);
            printf("****************************************************************\n");
            printf("Executando Processo ID: %d\n", currentProcess.id);
            printf("Ciclos totais: %d | Ciclos restantes: (%d - %d) -> %d\n",
            currentProcess.totalCycles, currentProcess.remainingCycles, 1, currentProcess.remainingCycles-1);
            printf("Possui I/O: %s\n", boolTypes[currentProcess.request.isRequired]);
            if(currentProcess.request.isRequired == TRUE) 
                printf("Tipo de I/O: %s | Ciclos I/O: %d | Ciclos de pedido I/O: %d\n", 
                ioTypes[currentProcess.request.type], currentProcess.request.remainingCycles, currentProcess.request.afterCycles);
            currentProcess = cycleProcess(currentProcess, &quantumRemaining); // Executa o processo por um ciclo
            // Checagem de finalização de processo
            if(currentProcess.isActive == FALSE) { // Processo finalizado
                printf("Processo ID: %d | Situação: Finalizado | Removido\n", currentProcess.id);
                breakPoint = TRUE;
            }
            // Checagem de ciclos de I/O
            else if(!breakPoint && currentProcess.request.isRequired && currentProcess.request.afterCycles == 0 && !currentProcess.request.isActive && currentProcess.request.remainingCycles > 0) { // Parar o processo e enviar para fila de I/O
                printf("Processo ID: %d | Situação: Paralisado | Enviado para a fila de I/O\n", currentProcess.id);
                currentProcess.request.isActive = TRUE; // Ativa o modo espera do processo para I/O
                enqueueProcess(ioQueue, currentProcess);
                breakPoint = TRUE;
            }
            // Checagem de premptação de processo
            else if(!breakPoint && quantumRemaining == 0 && currentProcess.remainingCycles > 0) { // Processo não completado, enviado para baixa prioridade
                printf("Processo ID: %d | Situação: Premptado | Enviado para a fila de baixa prioridade\n", currentProcess.id);
                enqueueProcess(lowQueue, currentProcess);
                breakPoint = TRUE;
            }
            // Passagem de ciclos paralela na fila de I/O
            if(ioQueue->size > 0) { // Se a fila do I/O conter dispositivos em espera
                int ioSize = ioQueue->size; // Salva o tamanho atual da fila de I/O
                for(int i=0; i<ioSize; i++) {
                    currentIO = dequeueProcess(ioQueue);
                    currentIO.request.remainingCycles -= 1; // Decrementa um ciclo do processo dentro da I/O
                    printf("Processo ID: %d | Situação: Aguardando I/O | Ciclos I/O restantes: %d\n", currentIO.id, currentIO.request.remainingCycles);
                    if(currentIO.request.remainingCycles == 0) { // Se os ciclos de I/O já foram encerrados
                        if(currentIO.request.afterPriority == HIGH_PRIORITY) {
                            currentIO.request.isActive = FALSE; // Desativa o request de I/O
                            enqueueProcess(highQueue, currentIO); // Processo retorna a fila de Alta prioridade
                            printf("Processo ID: %d | Situação: I/O finalizado | Enviado para a fila de Alta prioridade\n", currentIO.id);
                        }
                        else if(currentIO.request.afterPriority == LOW_PRIORITY) { 
                            currentIO.request.isActive = FALSE; // Desativa o request de I/O
                            enqueueProcess(lowQueue, currentIO); // Processo retorna a fila de Baixa prioridade
                            printf("Processo ID: %d | Situação: I/O finalizado | Enviado para a fila de Baixa prioridade\n", currentIO.id);
                        }
                    }
                    else if(currentIO.request.remainingCycles > 0) { // Se ainda restarem ciclos dentro do I/O
                        printf("Processo ID: %d | Situação: Reenviado para a fila I/O | Ciclos I/O restantes: %d\n", currentIO.id, currentIO.request.remainingCycles);
                        enqueueProcess(ioQueue, currentIO); // Processo retorna a fila de I/O
                    }
                }
            }
            if(breakPoint) { // Quando um processo já foi finalizado, premptado ou enviado para I/O
                breakPoint = FALSE;
                break;
            }
        }
    }
    // Exibe todas as filas vazias no final da simulação
    printf("==================================================\n");
    printf("FILA DE ALTA PRIORIDADE \n");
    displayQueue(highQueue , TRUE);
    printf("FILA DE BAIXA PRIORIDADE \n");
    displayQueue(lowQueue , TRUE);
    printf("FILA I/O \n");
    displayQueue(ioQueue , TRUE);
}

