#ifndef PREMIGRATION_H
#define PREMIGRATION_H

#include <math.h>
#include <omnetpp.h>
#include <string>
#include <sys/stat.h>

#include "helpers.h"

using namespace omnetpp;

class PreMigration {

    public:
    PreMigration(std::string path, double size);

    /** Premigration iteration */
    long runIteration();

    void startFinal();
    float getTotalPreMigrationTime();


    /** Getters */
    long getCheckpointSize();
    long getDBSize();
    long getSentSize();
    float getIterationTime();
    simtime_t getTimeDB();
    simtime_t getTimeCheckpoint();
    std::string getIterationName();


    private:
    int iterationNumber;
    std::string path;
    std::vector<simtime_t> iterationDatabaseTime;
    float iterationTime = 0;
    double iterationSize = 0;
    float totalPreMigrationTime;
    long dbSize;
    long checkpointSize;
    long size;
    long sentSize = 0;

    simtime_t timeDB;
    simtime_t timeCheckpoint;
    std::chrono::high_resolution_clock::time_point startPreMigTime;
//    auto lastPreMigTime;

    void setTotalPreMigrationTime();
};

#endif