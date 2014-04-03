//
//  data_types.c
//  EP01
//
//  Created by Júlio Turolla Ribeiro on 03/04/14.
//  Copyright (c) 2014 Julio Turolla. All rights reserved.
//

#include <stdio.h>

struct Pilot {
    unsigned int pilotNumber; // 0 - 2x totalTeams;
    unsigned int teamNumber; // totalTemas[this].number
    char teamPilotNumber; // A or B

    unsigned int currentLap;
    unsigned int fuelLevel;
    unsigned int currentSpeed;
};

struct DoubleTrackStep {
    struct Pilot pilot;
    struct Pilot secondPilot;
};

struct SimpleTrackStep {
    struct Pilot pilot;
};

int totalLaps;
int totalTeams;

struct DoubleTrackStep pista[160]; // each track position equals 25 meters and takes one pilot.
struct SimpleTrackStep boxes[10]; // pilots can be in boxes too, each position equals 25 meters.