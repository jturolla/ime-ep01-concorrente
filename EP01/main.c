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

#include "data_types.h"
#include "parse_input.c"

void logPilot(char logMessage[], struct Pilot pilotData){
    printf("[PILOTO %d | VOLTA %d | SPEED: %d | FUEL: %d] %s \n",
           pilotData.pilotNumber, pilotData.currentLap, pilotData.currentSpeed, pilotData.fuelLevel, logMessage);
}

void *piloto(void *arg) {
    int pilotNumber = (int) arg;

    struct Pilot pilot;

    pilot.pilotNumber = pilotNumber;
    pilot.currentLap = 0;
    pilot.fuelLevel = (totalLaps/2)+1;
    pilot.currentSpeed = 0;

    logPilot("PARTIU!", pilot);

    while (pilot.currentLap != totalLaps) {

        if (pilot.fuelLevel == 0) {
            // Enter boxes
            sleep(1); // Time to change the tires.
            pilot.fuelLevel = (totalLaps/2)+1;
        }

        pilot.currentLap++;
        pilot.fuelLevel--;
        logPilot("COMPLETOU VOLTA!", pilot);
    }

    logPilot("PASSOU NA LINHA DE CHEGADA!", pilot);

    return NULL;
}

int main(int argc, const char * argv[]) {

    printf("[FORMULA EP1] Quantas equipes estão competindo?  \n");
    scanf("%d", &totalTeams);

    if (!totalTeams || totalTeams <= 10) {
        printf("[FORMULA EP1] Por favor entre um número de voltas válido entre 11 e 127. \n");
        exit(0);
    }

    if (totalLaps > 150) {
        printf("[FORMULA EP1] Essa vai cansar os pilotos em!\n");
    }

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

