//
//  helper.c
//  EP01
//
//  Created by Julio Turolla Ribeiro on 03/04/14.
//  Copyright (c) 2014 Julio Turolla. All rights reserved.
//

#ifndef EP01_helper_h
#define EP01_helper_h

void logPilot(char logMessage[], struct Pilot pilotData){
    printf("[PILOTO %d | VOLTA %d | SPEED: %d | FUEL: %d] %s \n",
           pilotData.pilotNumber, pilotData.currentLap, pilotData.currentSpeed, pilotData.fuelLevel, logMessage);
}

#endif