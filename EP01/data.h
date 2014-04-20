//
//  data.h
//  EP01
//
//  Created by Julio Turolla Ribeiro on 03/04/14.
//  Copyright (c) 2014 Julio Turolla. All rights reserved.
//

#ifndef EP01_data_h
#define EP01_data_h

struct Pilot {
    unsigned int pilotNumber; // 0 - 2x totalTeams;
    unsigned int teamNumber; // totalTemas[this].number
    unsigned int teamPilotNumber; // 0 or 1
    
    unsigned int currentLap;
    unsigned int fuelLevel;
    unsigned int currentSpeed;
    int score;
};

struct DoubleTrackStep {
    struct Pilot pilot0;
    struct Pilot pilot1;
};

struct SingleTrackStep {
    struct Pilot pilot;
};

struct TrackStep {
    struct DoubleTrackStep doubleTrack;
    struct SingleTrackStep singleTrack;
    unsigned int isDouble;
};

int totalLaps;
int totalTeams;
int uniformVelocity;
int alternateVelocityPercentage;

struct TrackStep pista[160]; // each track position equals 25 meters and takes one pilot.
struct TrackStep boxes[10]; // pilots can be in boxes too, each position equals 25 meters.

#endif
