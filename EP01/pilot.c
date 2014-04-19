//
//  pilot.c
//  EP01
//
//  Created by Julio Turolla Ribeiro on 03/04/14.
//  Copyright (c) 2014 Julio Turolla. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "data.h"
#include "helper.h"
#include "pilot.h"

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