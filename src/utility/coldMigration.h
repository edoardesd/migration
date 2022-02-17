#ifndef COLDMIGRATION_H
#define COLDMIGRATION_H

#include <omnetpp.h>
#include <string>
//#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>

#include "helpers.h"

using namespace omnetpp;

class ColdMigration {

    public:
    ColdMigration();
    ColdMigration(std::string path);
    /** Activate cold migration */
    void start();


    /** Getters */
    long getCheckpointSize();
    long getDBSize();
    simtime_t getTimeDB();
    simtime_t getTimeCheckpoint();

    private:
    long dbSize;
    long checkpointSize;

    simtime_t timeDB;
    simtime_t timeCheckpoint;

    std::string path;
};

//TODO new file
//
//class PreMigration {
//
//    public:
//    PreMigration(std::string path);
//
//    /** Premigration iteration */
//    long runIteration();
//
//    void startFinal();
//
//    /** Getters */
//    long getCheckpointSize();
//    long getDBSize();
//    float getIterationTime();
//
//    private:
//    int iterationNumber;
//    std::string path;
////    std::vector<simtime_t> iterationCheckpointTime;
//    std::vector<simtime_t> iterationDatabaseTime;
//    float iterationTime = 0;
//    double iterationSize = 0;
//    long dbSize;
//    long checkpointSize;
//
//    simtime_t timeDB;
//    simtime_t timeCheckpoint;
//};

#endif