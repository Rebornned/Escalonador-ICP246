#include "icp246_libs.h"

// Cria um log com base no momento atual da máquina
FILE *createLogFile() {
    char currDate[32], path[150];
    getCurrentDate(currDate, sizeof(currDate));
    sprintf(path, "../logs/log_%s.txt", currDate);
    FILE *newLog = fopen(path, "w");
    return newLog;
}

// Pega o momento atual da máquina e passa para uma string
void getCurrentDate(char *destiny, size_t size) {
    time_t now = time(NULL);                // Pega o tempo atual
    struct tm *info = localtime(&now);      // Converte para data local

    // Formata como "dia/mes/anos hora:minuto:segundo"
    strftime(destiny, size, "%d_%m_%Y-%H_%M_%S", info);
}

// Recebe um arquivo de log e coloca uma string dentro dele, também printa a string.
int logPrintf(FILE *logFile, const char *format, ...) {
    char buffer[1024]; // buffer para montar a string
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args); // monta a string formatada
    va_end(args);

    printf("%s", buffer);

    // Escreve no arquivo, se fornecido
    if (logFile) {
        fprintf(logFile, "%s", buffer);
        fflush(logFile); // Atualiza o arquivo em tempo real
    }

    return 0;
}
