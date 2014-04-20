//
//  main.c
//  EP01
//
//  Created by Júlio Turolla Ribeiro on 02/04/14.
//  Copyright (c) 2014 Julio Turolla. All rights reserved.
//

#define IGNOREINPUT
#define MAXTEAMS 50

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "data.h"

FILE *file;
pthread_t threads[MAXTEAMS * 2];
pthread_mutex_t pista_mutex = PTHREAD_MUTEX_INITIALIZER;

void logPilot(char logMessage[], struct Pilot *pilotData){
    printf("[PILOTO %d | VOLTA %d | SPEED: %d | FUEL: %d] %s \n",
           pilotData->pilotNumber, pilotData->currentLap, pilotData->currentSpeed, pilotData->fuelLevel, logMessage);
}

void logTrack(int trackPosition) {
    struct TrackStep *step = &pista[trackPosition];

    if (step->isDouble == 1) {
        struct Pilot *pilot0;
        struct Pilot *pilot1;
        int pilotNumber0;
        int pilotNumber1;
        int pilotLap0;
        int pilotLap1;

        if (step->doubleTrack.pilot0_id != -1){
            pilot0 = &pilots[step->doubleTrack.pilot0_id];
            pilotNumber0 = pilot0->pilotNumber;
            pilotLap0 = pilot0->currentLap;
        } else {
            pilotNumber0 = -1;
            pilotLap0 = -1;
        }

        if (step->doubleTrack.pilot1_id != -1){
            pilot1 = &pilots[step->doubleTrack.pilot1_id];
            pilotNumber1 = pilot1->pilotNumber;
            pilotLap1 = pilot1->currentLap;
        } else {
            pilotNumber1 = -1;
            pilotLap1 = -1;
        }

        printf("PISTA DUPLA --------> [ PILOTO-0: %d (LAP: %d) | PILOTO-1: %d (LAP: %d) ]  (%d)\n", pilotNumber0, pilotLap0, pilotNumber1, pilotLap1, trackPosition);
    } else {
        struct Pilot *pilot;
        int pilotNumber;
        int pilotLap;

        if (step->singleTrack.pilot_id != -1){
            pilot = &pilots[step->doubleTrack.pilot0_id];
            pilotNumber = pilot->pilotNumber;
            pilotLap = pilot->currentLap;
        } else {
            pilotNumber = -1;
            pilotLap = -1;
        }

        printf("PISTA SIMPLES ------> [ PILOTO: %d (LAP: %d) ] (%d)\n", pilotNumber, pilotLap, trackPosition);
    }

}

void logEntireTrack() {
    // Print lineup to validation purposes.
    for (int a = 0; a < 160; a++) {
        logTrack(a);
    }
}

void parseInput(const char * filename) {

    file = fopen(filename, "r");

    if (file == NULL) {
        printf("Arquivo não encontrado.\n");
        exit(EXIT_FAILURE);
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
            } else if (evaluatingTrackStepInterval == 1) {
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

    fclose(file);
}

// This method keeps pilot and pista data in sync.
void addPilot(int pilotNumber, int trackStepNumber, int position) {

    struct Pilot *pilot = &pilots[pilotNumber];
    struct TrackStep *step = &pista[trackStepNumber];

    if (position == 0) {
        step->doubleTrack.pilot0_id = pilot->pilotNumber;
    }

    if (position == 1) {
        step->doubleTrack.pilot1_id = pilot->pilotNumber;
    }

    if (position == 2) {
        step->singleTrack.pilot_id = pilot->pilotNumber;
    }

    pilot->currentTrackStepNumber = trackStepNumber;
    pilot->currentTrackStepPosition = position;

}

void removePilot(struct Pilot *pilot) {

    struct TrackStep *step = &pista[pilot->currentTrackStepNumber];

    if (pilot->currentTrackStepPosition == 0) {
        step->doubleTrack.pilot0_id = -1;
    }

    if (pilot->currentTrackStepPosition == 1) {
        step->doubleTrack.pilot1_id = -1;
    }

    if (pilot->currentTrackStepPosition == 2) {
        step->singleTrack.pilot_id = -1;
    }

}

void moveForward(int pilotNumber) {

    //pthread_mutex_lock(&pista_mutex);
    struct Pilot *pilot = &pilots[pilotNumber];

    if (pilot->currentTrackStepNumber == INT16_MAX - 1) {
        //lol
        printf("Current track step number will overflow.");
        exit(0);
    }

    int nextStepNumber = pilot->currentTrackStepNumber + 1;

    // Se o piloto chegar na posição 160, ele completou uma volta.
    // Ele nunca deve ser posicionado na posição 160 pois ela não existe,
    // Redirecioná-lo à posição zero.
    if (nextStepNumber == 160) {
        nextStepNumber = 0;
        pilot->currentLap++;
        pilot->fuelLevel--;

        if (pilot->currentLap % 10 == 0) {
            logPilot("COMPLETOU VOLTA!", pilot);
        }
    }

    struct TrackStep *nextStep = &pista[nextStepNumber];

    // Tem vaga na nextStep?
    int nextStepPositionAvailable = -1;

    // Qual vaga está disponível?
    // Enquanto não houver um espaço disponível no nextStep, aguardar.

    // AWAIT AVAILABLE TRACKSTEP
    while (nextStepPositionAvailable == -1 ) {
        if (nextStep->isDouble == 1) {
            if (nextStep->doubleTrack.pilot0_id == -1) {
                nextStepPositionAvailable = 0;
            } else if (nextStep->doubleTrack.pilot1_id == -1) {
                nextStepPositionAvailable = 1;
            }
        } else {
            if (nextStep->singleTrack.pilot_id == -1) {
                nextStepPositionAvailable = 2;
            }
        }

        usleep(1);
        //printf("Não pode prosseguir: PILOTO: %d | PROXIMA POSIÇÃO: %d \n", pilot.pilotNumber, nextStep.stepNumber );
    }

    //printf("PODE PROSSEGUIR SIM! PILOTO: %d | PRÓXIMA TRACKSTEP: %d | POSIÇÃO: %d \n", pilot->pilotNumber, nextStep->stepNumber, nextStepPositionAvailable);

    removePilot(pilot);
    addPilot(pilot->pilotNumber, nextStep->stepNumber, nextStepPositionAvailable);

    //pthread_mutex_unlock(&pista_mutex);
}

void *piloto(void *arg) {

    int pilotNumber = (int) arg;

    //@TODO: Pilot sometimes is not initialized.
    struct Pilot *pilot = &pilots[pilotNumber];

    logPilot("PARTIU!", pilot);

    while (pilot->currentLap != totalLaps+10) {
        if (pilot->fuelLevel == 0) {
            logPilot("ACABOU A GASOLINA!", pilot);
            return NULL;
        } else {

            if (pilot->fuelLevel <= 2) {
                // Enter boxes
                // @TODO: Sessão crítica de entrada nos boxes.
                //
                logPilot("ENTRANDO NO BOX!", pilot);
                sleep(1); // Time to change the tires :)
                pilot->fuelLevel = (totalLaps/2)+1;
                logPilot("SAINDO DO BOX", pilot);
            }

            // TODO
            // SESSAO CRITICA DE ENTRAR EM UM TRACKSTEP.
            moveForward(pilot->pilotNumber);
        }
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
    for (int i = 0; i <= 159; i++) {
        pista[i].isDouble = 0;
        pista[i].stepNumber = i;
        pista[i].singleTrack.pilot_id = -1;
        pista[i].doubleTrack.pilot0_id = -1;
        pista[i].doubleTrack.pilot1_id = -1;
    }

    parseInput(fileName);

    // Initialize pilots
    printf("Iniciando equipes... \n");
    printf("PILOTOS: %d \n", (totalTeams*2));

    int pilotNumber = 0;
    for (int teamNumber = 0; teamNumber < totalTeams; teamNumber++) {

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

    // Line up pilots to the start.
    //
    // All pilots will be lined up, two in each
    // step of the track for the first <totalTeams>
    // number of tracksteps.
    //
    // @TODO: Organize pilots by points before lineup.
    pilotNumber = 0;
    for (int trackStepNumber = 0; trackStepNumber < totalTeams; trackStepNumber++) { // Each Trackstep from 0 ~ <totalTeams>

        // Tracksteps da largada devem ser sempre duplos!!!
        //
        // Apenas ignorar que não são duplos e tratar como duplos,
        // check de duplo ou não deve ser feito quando um piloto vai
        // entrar em um step, portanto é só ignorar a validação.
        addPilot(pilotNumber, trackStepNumber, 0);
        pilotNumber++;

        addPilot(pilotNumber, trackStepNumber, 1);
        pilotNumber++;

        struct TrackStep step = pista[trackStepNumber];
        step.isDouble = 1; // Assure that the start tracks are double.
        pista[trackStepNumber] = step;
    }

    logEntireTrack();

//    sleep(1);
//    printf("@@@@@@@@@@@@| -     READY    - |@@@@@@@@@@@@ \n\n");
//    sleep(1);
//    printf("@@@@@@@@@@@@| -     SET      - |@@@@@@@@@@@@ \n\n");
//    sleep(1);
//    printf("@@@@@@@@@@@@| -     GO!      - |@@@@@@@@@@@@ \n\n");

    // Largada
    printf("LARGADA! \n");

    for (int i = 0; i < (totalTeams*2); i++) {
        int pilotNumberInArray = (int) i;
        pthread_create(&threads[i], NULL, piloto, (void*) pilotNumberInArray);
    }

    for (int i = 0; i < (totalTeams*2); i++) {
        pthread_join(threads[i], NULL);
    }

    // Code below will run after all pilots arrive.

    logEntireTrack();

    printf("[FORMULA EP1] TODOS OS PILOTOS JÁ TERMINARAM A CORRIDA! \n");
    printf("[FORMULA EP1] CALCULANDO RESULTADOS...");

    return 0;
}

