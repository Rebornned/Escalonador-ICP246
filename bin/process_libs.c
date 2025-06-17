#include "icp246_libs.h"

// Retorna um novo processo totalmente aleatório
Process randomNewProcess() {
    IOType typesIO[3] = {DISK, MAGNETIC_TAPE, PRINTER};  // Vetor de tipos de I/O
    
    // Processo
    int priority = random_choice(LOW_PRIORITY, HIGH_PRIORITY);  // Escolhe uma prioridade para o processo
    int totalCycles = random_choice(MIN_CYCLES_PER_PROCESS, MAX_CYCLES_PER_PROCESS); // Escolhe um total de ciclos por processo

    // Request I/O
    int requiredIO = random_choice(FALSE, TRUE);
    int typeIndex = random_choice(0,2); // Escolhe um entre os 3 tipos de I/O do vetor
    int afterCycles = random_choice(1, totalCycles-1); // O request I/O deve ser chamado antes do processo finalizar
    char randomName[100];
    sprintf(randomName, "Random process n=%d", random_choice(1, 99999)); // Seta um nome aleatório

    // Criação de request e processos aleatórios
    IO_Request request = newRequest(typesIO[typeIndex], afterCycles, requiredIO); // Cria um request para o pedido aleatório
    Process process = newProcess(randomName, priority, totalCycles, request); // Cria um novo processo com os valores preenchidos aleatoriamente

    return process;
}

// Retorna um novo processo preenchido com os parametros
Process newProcess(char *name, int priority, int totalCycles, IO_Request request) {
    Process process;
    memset(&process, 0, sizeof(Process));
    if(totalCycles <= 0) {                  // Processos instantâneos não servem para simulação
        printf("ERROR, tempos de ciclos inválidos, impossível criar o processo: %s.\n", name);
        strcpy(process.name, "ERROR");
        return process;
    }
    process.id = random_choice(1, 999999);  // Obtém um ID
    process.priority = priority;            // Seta a prioridade
    process.totalCycles = totalCycles;      // Ciclos totais do processo
    process.remainingCycles = totalCycles;  // Ciclos restantes
    process.request = request;              // I/O do processo.
    process.isActive = TRUE;                // Processo começa sempre ativo
    strcpy(process.name, name);             // Copia o nome para o nome do processo
       
    return process;
}

// Retorna um novo request preenchido com os parametros
IO_Request newRequest(IOType type, int afterCycles, int required) {
    IO_Request request;
    memset(&request, 0, sizeof(IO_Request)); // Inicializando struct request
    if(required == TRUE) {
        if(type == DISK)  {                  // I/O do tipo Disco
            request.afterPriority = LOW_PRIORITY;
            request.remainingCycles = DISK_CYCLES;
        }
        else if(type == MAGNETIC_TAPE)  {    // I/O do tipo Fita Magnética
            request.afterPriority = HIGH_PRIORITY;
            request.remainingCycles = MAGNETIC_TAPE_CYCLES;
        }
        else if(type == PRINTER)  {          // I/O do tipo Impressora
            request.afterPriority = HIGH_PRIORITY;
            request.remainingCycles = PRINTER_CYCLES;
        }
        request.afterCycles = afterCycles;   // Tempo em ciclos até o request ser executado
        request.type = type;    
        request.isRequired = TRUE;           // Sinaliza que o processo terá request I/O
        request.isActive = FALSE;            // Request é criado inativamente.
    }
    else {
        request.isRequired = FALSE;          // Significa que o processo não tem request de I/O.
    }

    return request;
}

// Retorna um processo com os ciclos reduzidos pelo quantum
Process cycleProcess(Process process, int *quantumRemaining) {
    // Execução do processo
    if(process.request.isActive == FALSE) {
        process.remainingCycles--; // Processo utilizando o quantum dispónivel
        *quantumRemaining -= 1; // Sobra Quantum para o próximo processo
    }
    // Ciclos restantes para I/O
    if(process.request.isRequired == TRUE && process.request.isActive == FALSE && process.request.afterCycles > 0) {
        process.request.afterCycles--; // Decrementa os ciclos de espera para o I/O
    }

    if(process.remainingCycles <= 0) { // Se todos os ciclos do processo foram atendidos
        process.isActive = FALSE; // Processo concluído e desativado.
    }
    return process;
}

// Retorna um valor aleatório entre o intervalo
int random_choice(int min, int max) {
    return rand() % (max - min + 1) + min;
}