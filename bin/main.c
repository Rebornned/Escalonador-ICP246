#include "icp246_libs.h"


int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF8");
    srand(time(NULL)); // Semente de randomização
    // ----------------------------------------------------
    //           Declaração e inicialização de filas
    // ----------------------------------------------------
    Queue highQueue, lowQueue, ioQueue, arrivalQueue;
    initQueue(&highQueue);
    initQueue(&lowQueue);
    initQueue(&ioQueue);
    initQueue(&arrivalQueue);
    // ----------------------------------------------------
    //              Declaração de variáveis
    // ----------------------------------------------------
    int option = 0, menu_break = FALSE, mode, totalProcesses = 0;
    char *modesAvailable[50] = {"Passo a Passo", "Automático"};
    // ----------------------------------------------------
    //                        Menu
    // ----------------------------------------------------
    while(!menu_break) { // Enquanto o programa rodar, menu funciona 
        printf("*=*=*=*=*=*=*=*=*=*=*=*=**=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n");
        printf("             Simulador - Escalonador de processos             \n");
        printf("*=*=*=*=*=*=*=*=*=*=*=*=**=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n");
        printf("    Seja bem vindo (a) ao nosso simulador de escalonador      \n");
        printf("*=*=*=*=*=*=*=*=*=*=*=*=**=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n");
        // *************************************************************************
        //                 Primeira camada do menu - Tipo de simulação
        // *************************************************************************
        printf("[1] Passo a Passo\n");
        printf("[2] Automático\n");
        printf("[3] Sair\n");
        while(option != 1 && option != 2 && option != 3) {
            printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
            printf("Opção: ");
            scanf("%d", &option);
        }
        printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
        if(option == 1) {
            mode = 0;
        }
        else if(option == 2) {
            mode = 1;
        }
        else if(option == 3) {
            printf("##############################################################\n");
            printf("Finalizando programa...\n");
            exit(0);
        }
        option = 0; // Reseta opção
        // *************************************************************************
        //              Segunda camada do menu - Iniciar simulação
        // *************************************************************************
        printf("#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n");
        printf("Modo selecionado: %s | Processos: Aleatórios\n", modesAvailable[mode]);
        printf("#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#\n");
        printf("[1] Iniciar Simulação\n");
        printf("[2] Cancelar\n");
        
        while(option != 1 && option != 2) {
            printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
            printf("Opção: ");
            scanf("%d", &option);
        }
        printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
        if(option == 1) {
            while(totalProcesses < 1 || totalProcesses > PROCESS_LIMIT) {           
                printf("Quantos processos deseja simular [1 a 50]? ");
                scanf("%d", &totalProcesses);
                getchar();
            }
            printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
            printf("Quantidade de processos escolhidos: %d\n", totalProcesses);
            for(int i=0; i<totalProcesses; i++) {
                Process newProcess = randomNewProcess();
                 // Checa se há processos que entrarão no ciclo 0
                if(newProcess.arrivalCycles == 0 && newProcess.isActive == FALSE) { 
                    newProcess.isActive = TRUE; // Ativa o processo para ser inserido nas filas
                    if(newProcess.priority == HIGH_PRIORITY) {
                        enqueueProcess(&highQueue, newProcess);
                    }
                    else if(newProcess.priority == LOW_PRIORITY) {
                        enqueueProcess(&lowQueue, newProcess);
                    }
                }
                else if(newProcess.arrivalCycles > 0)
                    enqueueProcess(&arrivalQueue, newProcess); // Coloca todos os processos que irão chegar na lista
            }

        
            // ----------------------------------------------------
            //                      Simulação
            // ----------------------------------------------------
            printf("==================================================\n");
            printf("FILA DE PROCESSOS PREVISTOS \n");
            displayQueue(&arrivalQueue, TRUE);
            startSimulation(&highQueue, &lowQueue, &ioQueue, &arrivalQueue, mode);
            totalProcesses = 0;
            option = 0; // Reseta opção
            continue;
        }
        else if(option == 2) { // Cancela e retorna ao menu principal
            option = 0; // Reseta opção
            continue;
        }
    }
    
    return 0;
}

int startSimulation(FILE *log, Queue *highQueue, Queue* lowQueue, Queue *ioQueue, Queue *arrivalQueue, int mode) {
    // ----------------------------------------------------
    //       Declaração e inicialização de variáveis
    // ----------------------------------------------------
    //              Variáveis estatísticas
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++
    int currentCycle = 0; // Quantidade de ciclos decorridos
    int finishedProcesses = 0; // Quantidade de processos finalizados
    int premptionOcurrences = 0; // Quantidade de preempções realizadas
    int ioOcurrences = 0; // Quantidade de I/O executados
    int idleCpuOcurrences = 0; // Tempo que a cpu ficou ociosa
    int averageProcessCycles = 0; // Tempo médio por processo
    int ioTypesOcurrences[3] = {0, 0, 0}; 
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++
    int arrivalSize = arrivalQueue->size; // Salva o tamanho atual da fila de Chegada
    int breakPoint = FALSE; // Ponto de quebra para sair do looping
    int cpuIdle = FALSE; // Controle de ociosidade da CPU
    int quantumUsed = 0; // Quantum utilizado através dos ciclos
    int quantumRemaining = QUANTUM_CLOCK; // Quantum universal do simulador
    char *ioTypes[3] = {"Disco", "Fita magnética", "Impressora"}; // Tradução dos index I/O para strings
    char *boolTypes[2] = {"Falso", "Verdadeiro"}; // Tradução dos index booleanos para strings
    Process currentProcess, currentIO; // Variaveis para absorver os processos e I/Os atuais
    memset(&currentProcess, 0, sizeof(Process)); // Inicialização
    memset(&currentIO, 0, sizeof(Process)); // Inicialização
    // -----------------------------------------------------------------------
    //                          Inicio da simulação
    // -----------------------------------------------------------------------
    logPrintf(log, "##############################################################\n");
    logPrintf(log, "                    INICIANDO SIMULAÇÃO\n");
    logPrintf(log, "##############################################################\n");
    while(highQueue->size != 0 || lowQueue->size != 0 || ioQueue->size != 0 || arrivalQueue->size != 0) { // Enquanto as 3 listas terem processos, o simulador roda.
        if(quantumRemaining == 0) {// Repõe o quantum de clock
            quantumRemaining = QUANTUM_CLOCK;
            cpuIdle = FALSE; // Retira a cpu do modo ocioso
            quantumUsed = 0; // Reseta a contagem de quantuns utilizados
        }
        if(highQueue->size > 0) { // Verifica se a fila de prioridade contém elementos
            currentProcess = dequeueProcess(highQueue);
            cpuIdle = FALSE;
        }
        else if(lowQueue->size > 0) { // Verifica se a fila de baixa contém elementos
            currentProcess = dequeueProcess(lowQueue);
            cpuIdle = FALSE;
        }
        else if(highQueue->size == 0 && lowQueue->size == 0 && ioQueue->size > 0) { // Verifica se não sobrou um processo congelado
            cpuIdle = TRUE; // CPU Ociosa aguardando processos saírem da I/O
            memset(&currentProcess, 0, sizeof(Process));
        }
        else if(highQueue->size == 0 && lowQueue->size == 0 && ioQueue->size == 0 && arrivalQueue->size > 0) { // Verifica ainda há processos para chegarem
            cpuIdle = TRUE; // CPU Ociosa aguardando processos vindos da fila de chegada
            memset(&currentProcess, 0, sizeof(Process));
        }

        // Loop de clock
        while (quantumRemaining > 0) {
            if((highQueue->size > 0 || lowQueue->size > 0) && cpuIdle == TRUE) { // Verifica se há processos para serem executados
                quantumRemaining = 0; // Reseta o quantum para atender os novos processos
                break; // Processos foram encontrados, Quantum resetado.
            }
            currentCycle++; // Atualiza o contador em 1 ciclo
            logPrintf(log, "\n=+=+=+=+=+=++=+=+=+=+=++=+=+=+=+=+=++=+=+=+=+=+=++=+=+=+=+=+=+\n");
            logPrintf(log, "                CICLO ATUAL: %d | QUANTUM: %d\n", currentCycle, quantumRemaining);
            logPrintf(log, "**************************************************************\n");
            logPrintf(log, "FILA DE ALTA PRIORIDADE: ");
            displayQueue(log, highQueue , FALSE);
            logPrintf(log, "FILA DE BAIXA PRIORIDADE: ");
            displayQueue(log, lowQueue , FALSE);
            logPrintf(log, "FILA I/O: ");
            displayQueue(log, ioQueue , FALSE);
            logPrintf(log, "**************************************************************\n");
            if(arrivalQueue->size > 0) { // Roda em paralelo, faz o processo chegar a simulação
                arrivalSize = arrivalQueue->size;
                for(int i=0; i<arrivalSize; i++) {
                    Process currP = dequeueProcess(arrivalQueue); // Processo atual verificado
                    if(currP.arrivalCycles > 0) {
                        currP.arrivalCycles -= 1; // Decrementa os ciclos para chegada do processo
                    }
                    if(currP.arrivalCycles == 0 && currP.isActive == FALSE) {
                        currP.isActive = TRUE; // Ativa o processo para ser inserido nas filas
                        if(currP.priority == HIGH_PRIORITY) {
                            logPrintf(log, "Processo ID: %d | Situação: Iniciado | Enviado para fila de Alta prioridade\n", currP.id);
                            enqueueProcess(highQueue, currP);
                        }
                        else if(currP.priority == LOW_PRIORITY) {
                            logPrintf(log, "Processo ID: %d | Situação: Iniciado | Enviado para fila de Baixa prioridade\n", currP.id);
                            enqueueProcess(lowQueue, currP);
                        }
                        logPrintf(log, "$=$=$=$=$=$=$=$=$=$=$=$=$=$=$=$$=$=$=$=$=$=$=$=$=$=$=$=$=$=$=$\n");
                        //if(cpuIdle) // Quebra a ociosiodade da CPU
                        //    breakPoint = TRUE;
                    }
                    else if(currP.arrivalCycles > 0)
                        enqueueProcess(arrivalQueue, currP);
                }
            }
            if(!cpuIdle) { // Se a cpu não está em espera, ocorre tudo normalmente
                logPrintf(log, "##############################################################\n");
                logPrintf(log, "                Executando Processo ID: %d\n", currentProcess.id);
                logPrintf(log, "##############################################################\n");
                logPrintf(log, "Ciclos totais: %d | Ciclos restantes: (%d - %d) -> %d\n",
                currentProcess.totalCycles, currentProcess.remainingCycles, 1, currentProcess.remainingCycles-1);
                logPrintf(log, "Possui I/O: %s\n", boolTypes[currentProcess.request.isRequired]);
                if(currentProcess.request.isRequired == TRUE) 
                    logPrintf(log, "Tipo de I/O: %s | Ciclos I/O: %d | Ciclos restantes para I/O: (%d - 1) -> %d\n", 
                    ioTypes[currentProcess.request.type], currentProcess.request.remainingCycles, currentProcess.request.afterCycles, currentProcess.request.afterCycles-1);
                currentProcess = cycleProcess(currentProcess, &quantumRemaining); // Executa o processo por um ciclo
                quantumUsed += 1;
                logPrintf(log, "##############################################################\n");
                // Checagem de finalização de processo
                if(currentProcess.isActive == FALSE) { // Processo finalizado
                    logPrintf(log, "Processo ID: %d | Situação: Finalizado | Removido | Quantum utilizado: %d\n", currentProcess.id, quantumUsed);
                    quantumUsed = 0;
                    finishedProcesses += 1;
                    breakPoint = TRUE;
                }
                // Checagem de ciclos de I/O
                else if(currentProcess.request.isRequired && currentProcess.request.afterCycles == 0 && !currentProcess.request.isActive && currentProcess.request.remainingCycles > 0) { // Parar o processo e enviar para fila de I/O
                    printf("Processo ID: %d | Situação: Paralisado | Enviado -> fila de I/O\n", currentProcess.id);
                    quantumUsed = 0;
                    currentProcess.request.isActive = TRUE; // Ativa o modo espera do processo para I/O
                    enqueueProcess(ioQueue, currentProcess);
                    ioOcurrences += 1;
                    ioTypesOcurrences[currentProcess.request.type] += 1;
                    breakPoint = TRUE;
                }
                // Checagem de premptação de processo
                else if(quantumRemaining == 0 && currentProcess.remainingCycles > 0) { // Processo não completado, enviado para baixa prioridade
                    logPrintf(log, "Processo ID: %d | Situação: Premptado | Enviado -> fila de baixa prioridade | Quantum utilizado: %d\n", currentProcess.id, quantumUsed);
                    quantumUsed = 0;
                    enqueueProcess(lowQueue, currentProcess);
                    premptionOcurrences += 1;
                }
                logPrintf(log, "#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n");
            }
            else { // CPU em espera - Executa apenas os ciclos de I/O
                printf("CPU em espera, aguardando até o proximo Quantum.\n");
                printf("--------------------------------------------------------------\n");
                idleCpuOcurrences += 1; // Registra a quantidade de ciclos de ociosidade
                quantumRemaining -= 1; // Decrementa o quantum mesmo se a cpu estiver em modo de espera
            }
            // Passagem de ciclos paralela na fila de I/O
            if(ioQueue->size > 0) { // Se a fila do I/O conter dispositivos em espera
                int ioSize = ioQueue->size; // Salva o tamanho atual da fila de I/O
                for(int i=0; i<ioSize; i++) {
                    currentIO = dequeueProcess(ioQueue);
                    currentIO.request.remainingCycles -= 1; // Decrementa um ciclo do processo dentro da I/O
                    logPrintf(log, "Processo ID: %d | Situação: Aguardando I/O | Ciclos I/O restantes: (%d - 1) -> %d\n", currentIO.id, currentIO.request.remainingCycles+1, currentIO.request.remainingCycles);
                    if(currentIO.request.remainingCycles == 0) { // Se os ciclos de I/O já foram encerrados
                        if(currentIO.request.afterPriority == HIGH_PRIORITY) {
                            currentIO.request.isActive = FALSE; // Desativa o request de I/O
                            enqueueProcess(highQueue, currentIO); // Processo retorna a fila de Alta prioridade
                            logPrintf(log, "Processo ID: %d | Situação: I/O finalizado | Enviado -> fila de Alta prioridade\n", currentIO.id);
                        }
                        else if(currentIO.request.afterPriority == LOW_PRIORITY) { 
                            currentIO.request.isActive = FALSE; // Desativa o request de I/O
                            enqueueProcess(lowQueue, currentIO); // Processo retorna a fila de Baixa prioridade
                            logPrintf(log, "Processo ID: %d | Situação: I/O finalizado | Enviado -> fila de Baixa prioridade\n", currentIO.id);
                        }
                    }
                    else if(currentIO.request.remainingCycles > 0) { // Se ainda restarem ciclos dentro do I/O
                        enqueueProcess(ioQueue, currentIO); // Processo retorna a fila de I/O
                    }
                }
            }
            
            if(mode == STEP_BY_STEP_MODE) {
                logPrintf(log, "Pressione ENTER para continuar... \n");
                logPrintf(log, "*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+**+*+*+**+*+**+*+*+*+*+*\n");
                getchar();
            }

            if(breakPoint) { // Quando um processo já foi finalizado, premptado ou enviado para I/O
                breakPoint = FALSE;
                break;
            }
        }
    }
    // ----------------------------------------------------------------------
    //          Exibe todas as filas vazias no final da simulação
    // ----------------------------------------------------------------------
    logPrintf(log, "==================================================\n");
    logPrintf(log, "FILA DE ALTA PRIORIDADE: ");
    displayQueue(log, highQueue , TRUE);
    logPrintf(log, "FILA DE BAIXA PRIORIDADE: ");
    displayQueue(log, lowQueue , TRUE);
    logPrintf(log, "FILA I/O: ");
    displayQueue(log, ioQueue , TRUE);
    // ----------------------------------------------------------------------
    //                              Estatísticas
    // ----------------------------------------------------------------------
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("                       Estatísticas Finais                    \n");
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("Total de ciclos executados: %d\n", currentCycle);
    printf("Total de processos finalizados: %d\n", finishedProcesses);
    printf("Preempções registradas: %d\n", premptionOcurrences);
    printf("Operações de I/O realizadas: %d\n", ioOcurrences);
    printf("Ciclos com CPU ociosa: %d\n", idleCpuOcurrences);
    printf("Tempo médio por processo: %.2f\n", currentCycle / (float) finishedProcesses);
    printf("Processos que retornaram da I/O: %d\n", ioOcurrences);
    printf("Tipos de I/O utilizados:\n");
    printf("  - Disco: %d\n", ioTypesOcurrences[0]);
    printf("  - Fita magnética: %d\n", ioTypesOcurrences[1]);
    printf("  - Impressora: %d\n", ioTypesOcurrences[2]);
}

