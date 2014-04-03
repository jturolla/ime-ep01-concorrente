//
//  main.c
//  EP01
//
//  Created by Júlio Turolla Ribeiro on 02/04/14.
//  Copyright (c) 2014 Julio Turolla. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int totalLaps;

struct PilotData {
    int pilotNumber;
    int currentLap;
    int fuelLevel;
    int currentSpeed;
};

void logPilot(char logMessage[], int pilotNumber){
    printf("[PILOTO %d] %s \n", pilotNumber, logMessage);
}

void *piloto(void *arg) {
    int pilotNumber = (int) arg;
    logPilot("PILOTO ESTÁ SE PREPARANDO!", pilotNumber);

    struct PilotData pilot;
    pilot.pilotNumber = pilotNumber;
    pilot.currentLap = 0;
    pilot.fuelLevel = totalLaps/2;
    pilot.currentSpeed = 0;

    logPilot("PARTIU!", pilotNumber);

    while (pilot.currentLap != totalLaps) {

        if (pilot.fuelLevel == 0) {
            // Enter boxes
        }

        pilot.currentLap++;


    }

    return NULL;
}

int main(int argc, const char * argv[]) {

    printf("[FORMULA EP1] Quantas voltas serão dadas?  (n > 10) \n");
    scanf("%d", &totalLaps);

    if (!totalLaps || totalLaps <= 10) {
        printf("[FORMULA EP1] Por favor entre um número de voltas válido entre 11 e 127. \n");
        exit(0);
    }

    if (totalLaps > 150) {
        printf("[FORMULA EP1] Essa vai cansar os pilotos em!\n");
    }

    printf("[FORMULA EP1] FOI DADA A LARGADA, %d VOLTAS!\n", totalLaps);

    int myArray[10] = {
       0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    };

    arc4random();

    pthread_t threads[10];

    printf("Initiating Threads... \n");

    for (int i = 0; i < 10; i++) {
        int mychar = (int) myArray[i];
        pthread_create(&threads[i], NULL, piloto, (void *)mychar);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All Threads Finished... \n");
    printf("[FORMULA EP1] TODOS OS PILOTOS JÁ TERMINARAM A CORRIDA! \n");
    printf("[FORMULA EP1] CALCULANDO RESULTADOS...");

    return 0;
}

