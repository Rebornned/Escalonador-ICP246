#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>

// ############################################################################
// ============================================================================
//                                Defines
// ============================================================================
// ############################################################################
#define PROCESS_LIMIT 50     // Quantidade de processos máxima
#define QUANTUM_CLOCK 4     // Tempo do clock de cada ciclo
#define TRUE 1              // 1 é verdadeiro
#define FALSE 0             // 1 é falso
//-----------------------------------------------------------------------------
//   Definição de ciclos totais de cada tipo de IO ## NENHUM PODE SER ZERO.
//-----------------------------------------------------------------------------
#define DISK_CYCLES 4           // Tempo definido para o Disco
#define MAGNETIC_TAPE_CYCLES 7  // Tempo definido para a Fita magnética
#define PRINTER_CYCLES 5        // Tempo definido para a Impressora
//-----------------------------------------------------------------------------
//                         Definição de prioridades
//-----------------------------------------------------------------------------
#define HIGH_PRIORITY 1  // Alta prioridade é 1
#define LOW_PRIORITY 0   // Baixa prioridade é 0
//-----------------------------------------------------------------------------
//                         Definição de Randomização
//-----------------------------------------------------------------------------
#define MIN_CYCLES_PER_PROCESS 2 // Define o piso de ciclos minimos durante a aleatoriazação do processo
#define MAX_CYCLES_PER_PROCESS 20 // Define o teto de ciclos máximos durante a aleatoriazação do processo
// ############################################################################
// ============================================================================
//                               Estruturas
// ============================================================================
// ############################################################################
typedef enum { 
    DISK,           // I/O do tipo "Disco" - Representa 0 em código
    MAGNETIC_TAPE,  // I/O do tipo "Fita magnética" - Representa 1 em código
    PRINTER         // I/O do tipo "Impressiora" - Representa 2 em código
} IOType;

typedef struct {
    int afterPriority;     // Prioridade de retorno após I/O (definida pela regra do tipo de I/O)
    int afterCycles;       // Após quantos ciclos de CPU o processo pedirá o I/O
    int remainingCycles;   // Quanto tempo o processo ainda precisa ficar na fila de I/O
    IOType type;           // Tipo do I/O: "disco", "fita magnética", "impressora"
    int isActive;          // Se esse pedido está ativo ou já foi resolvido.
    int isRequired;        // Verifica se o I/O é requerido, se não, o processo não possui I/O.
} IO_Request;

typedef struct {
    int id;                 // Identificador único do processo
    int priority;           // Nível de prioridade do processo
    int totalCycles;        // Total de ciclos que o processo deve durar
    int remainingCycles;    // Total de ciclos restantes do processo
    IO_Request request;     // Requerimento de I/O do processo
    char name[100];         // Nome do processo
    int isActive;           // Se o processo está ativo ou já foi realizado
} Process;

typedef struct {
    Process data[PROCESS_LIMIT];    // Fila
    int front;                      // Frente
    int rear;                       // Cauda
    int size;                       // Tamanho
} Queue;

// ############################################################################
// ============================================================================
//                                 Funções
// ============================================================================
// ############################################################################
//                       Arquivo: {app}/bin/queue_libs.c
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
void initQueue(Queue *queue); // Inicia a fila com todos os dados zerados
int enqueueProcess(Queue *queue, Process process); // Enfielira um processo na fila
Process dequeueProcess(Queue *queue); // Desenfileira um processo na fila
Process getFrontProcess(Queue *queue); // Retorno o primeiro processo da fila sem altera-la
int isFull(Queue *queue); // Verifica se a fila está cheia
int isEmpty(Queue *queue); // Verifica se a fila está vazia
void displayQueue(Queue *queue, int details); // Exibe todos os elementos da fila sem altera-la
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
// ############################################################################
//                       Arquivo: {app}/bin/process_libs.c
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Process newProcess(char *name, int priority, int totalCycles, IO_Request request);
Process randomNewProcess(); // Retorna um novo processo totalmente aleatório
IO_Request newRequest(IOType type, int afterCycles, int required); // Gera um novo request de I/O para ser inserido no processo
Process cycleProcess(Process process, int *quantumRemaining); // Cicla um processo através do quantum restante
int random_choice(int min, int max); // Retorna um número aleatório entre o intervalo
// ############################################################################
//                       Arquivo: {app}/bin/main.c
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
int startSimulation(Queue *highQueue, Queue* lowQueue, Queue *ioQueue); // Toda a lógica da simualação