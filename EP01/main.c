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
#include <string.h>

#include "data.h"

#define IGNOREINPUT
#define MAXTEAMS 50

FILE *file;

void logPilot(char logMessage[], struct Pilot pilotData){
    printf("[PILOTO %d | VOLTA %d | SPEED: %d | FUEL: %d] %s \n",
           pilotData.pilotNumber, pilotData.currentLap, pilotData.currentSpeed, pilotData.fuelLevel, logMessage);
}

void parseInput(const char * filename) {

    file = fopen(filename, "r");

    if (file == NULL) {
        printf("Arquivo não encontrado ou corrompido.\n");
        exit(0);
    }

    char line[255];

    int lineNumber = 0;
    int evaluatingTrackStepInterval = 0;
    int trackStepNumberIntervalStart = 0;
    int trackStepNumberIntervalEnd = 0;

    while (fgets(line, sizeof(line), file)) {

        // Cleanup line
        size_t charnumber = strlen(line) - 1;
        if (line[charnumber] == '\n'){
            line[charnumber] = '\0';
        }

        if (lineNumber == 0) { // Laps
            totalLaps = atoi(line);

            if (!totalLaps || totalLaps <= 10) {
                printf("[FORMULA EP1] Por favor entre um número de voltas válido a partir de 10. \n");
                exit(0);
            }
        }

        if (lineNumber == 1) { // Teams
            totalTeams = atoi(line);
        }

        if (lineNumber == 2) { // Run Type (U = Uniform velocity 50m/s | A = Changing Velocity)
            if (strcmp("U", line) == 0) {
                uniformVelocity = 1;
            } else if (strcmp("A", line) == 0) {
                uniformVelocity = 0;
            } else {
                printf("Tipo de corrida não identificado no arquivo. \n");
                exit(EXIT_FAILURE);
            }
        }

        if (uniformVelocity == 0 && lineNumber == 3) {
            alternateVelocityPercentage = atoi(line);
        }

        // Setup track where cars can run over.
        if ((lineNumber >= 3 && uniformVelocity == 1) || (lineNumber > 3 && uniformVelocity == 0)) {

            if (evaluatingTrackStepInterval == 0) {
                evaluatingTrackStepInterval = 1;
                trackStepNumberIntervalStart = atoi(line);

                if (trackStepNumberIntervalStart > 159 || trackStepNumberIntervalStart < 0) {
                    printf("Pista duplicada fora do intervalo. \n");
                    exit(EXIT_FAILURE);
                }
            }else if (evaluatingTrackStepInterval == 1) {
                evaluatingTrackStepInterval = 0;
                trackStepNumberIntervalEnd = atoi(line);

                if (trackStepNumberIntervalEnd > 159 || trackStepNumberIntervalEnd < 0) {
                    printf("Pista duplicada fora do intervalo. \n");
                    exit(EXIT_FAILURE);
                }

                for (int i = trackStepNumberIntervalStart; i <= trackStepNumberIntervalEnd; i++) {
                     pista[i].isDouble = 1;
                }
                printf("PISTA DUPLICADA PARA TRECHO: %d ~ %d (inclusive). \n", trackStepNumberIntervalStart, trackStepNumberIntervalEnd);
            }
        }

        lineNumber++;
    }
}

void *piloto(void *arg) {
    struct Pilot pilot;

    logPilot("PARTIU!", pilot);

    while (pilot.currentLap != totalLaps) {

        if (pilot.fuelLevel == 0) {
            // Enter boxes
            logPilot("ENTRANDO NO BOX!", pilot);
            sleep(1); // Time to change the tires.
            pilot.fuelLevel = (totalLaps/2)+1;
            logPilot("SAINDO DO BOX", pilot);
        }

        pilot.currentLap++;
        pilot.fuelLevel--;
        logPilot("COMPLETOU VOLTA!", pilot);
    }

    logPilot("PASSOU NA LINHA DE CHEGADA!", pilot);

    return NULL;
}

int main(int argc, const char * argv[]) {

    const char *fileName;

#ifndef IGNOREINPUT
    if (argc != 2 && argv[1] == NULL) {
        printf("Por favor informe o nome do arquivo de entrada.\n");
        exit(0);
    } else {
        fileName = argv[1];
    }
#else
    fileName = "/Users/jturolla/dev/ime/EP01/EP01/inputdata.txt";
#endif

    // Initialize track.
    for (int i = 0; i == 159; i++) {
        pista[i].isDouble = 0;
    }

    parseInput(fileName);

    // Initialize pilots
    printf("Iniciando equipes... \n");

    struct Pilot pilots[MAXTEAMS * 2]; // Max teams is 50.

    int pilotNumber = 0;
    for (int teamNumber = 0; teamNumber <= totalTeams; teamNumber++) {

        struct Pilot pilotA;

        pilotA.pilotNumber = pilotNumber;
        pilotA.teamNumber = teamNumber;
        pilotA.teamPilotNumber = 0;
        pilotA.currentLap = 0;
        pilotA.fuelLevel = (totalLaps/2)+1;
        pilotA.currentSpeed = 50; // 50m/s = 180km/h
        pilotA.score = rand() % 250;

        pilots[pilotNumber] = pilotA;
        pilotNumber++;

        struct Pilot pilotB;

        pilotB.pilotNumber = pilotNumber;
        pilotA.teamNumber = teamNumber;
        pilotB.teamPilotNumber = 1;
        pilotB.currentLap = 0;
        pilotB.fuelLevel = (totalLaps/2)+1;
        pilotB.currentSpeed = 50; // 50m/s = 180km/h
        pilotB.score = rand() % 250;

        pilots[pilotNumber] = pilotB;
        pilotNumber++;
    }
    
    pthread_t threads[MAXTEAMS * 2];

    // Line up pilots to the start.
    //
    // All pilots will be lined up, two in each
    // step of the track for the first <totalTeams>
    // number of tracksteps.
    //
    // @TODO: Organize pilots by points before lineup.
    pilotNumber = 0;
    for (int i = 0; i <= totalTeams; i++) { // Each Trackstep
        struct TrackStep step = pista[i];

        // Tracksteps da largada devem ser sempre duplos!!!
        //
        // Apenas ignorar que não são duplos e tratar como duplos,
        // check de duplo ou não deve ser feito quando um piloto vai
        // entrar em um step, portanto é só ignorar a validação.
        step.doubleTrack.pilot0 = pilots[pilotNumber];
        pilotNumber++;
        step.doubleTrack.pilot1 = pilots[pilotNumber];
        pilotNumber++;

        printf("POSIÇÃO %d |||| Piloto %d || Piloto %d |||| \n", i, step.doubleTrack.pilot0.pilotNumber, step.doubleTrack.pilot1.pilotNumber);
    }


    // Largada


//
//    printf("Initiating Threads... \n");
//
//    for (int i = 0; i < MAXTEAMS*2; i++) {
//        int mychar = (int) pilots[i];
//        pthread_create(&threads[i], NULL, piloto, (void*) mychar);
//    }
//
//    for (int i = 0; i < 10; i++) {
//        pthread_join(threads[i], NULL);
//    }

    printf("All Threads Finished... \n");
    printf("[FORMULA EP1] TODOS OS PILOTOS JÁ TERMINARAM A CORRIDA! \n");
    printf("[FORMULA EP1] CALCULANDO RESULTADOS...");

    return 0;
}

