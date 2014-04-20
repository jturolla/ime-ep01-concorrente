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
    unsigned int currentTrackStepNumber;
    unsigned int currentTrackStepPosition; // 0 => doubleTrack[0], 1 => doubleTrack[1], 2 => singleTrack.

    unsigned int fuelLevel;
    unsigned int currentSpeed;

    int score;
} Pilot;

struct Pilot pilots[MAXTEAMS * 2]; // Max teams is 50.

struct DoubleTrackStep {
    int pilot0_id;
    int pilot1_id;
} DoubleTrackStep;

struct SingleTrackStep {
    int pilot_id;
} SingleTrackStep;

struct TrackStep {
    struct DoubleTrackStep doubleTrack;
    struct SingleTrackStep singleTrack;
    unsigned int isDouble;
    unsigned int stepNumber;
} TrackStep;

int totalLaps;
int totalTeams;
int uniformVelocity;
int alternateVelocityPercentage;

struct TrackStep pista[160]; // each track position equals 25 meters and takes one pilot.
struct TrackStep boxes[10]; // pilots can be in boxes too, each position equals 25 meters.

#endif
