#ifndef MIGRATION_H
#define MIGRATION_H

#include <omnetpp.h>
#include <string>
#include <sys/stat.h>

#include "helpers.h"

using namespace omnetpp;

class PreMigration {

    public:
    PreMigration(std::string path);

    /** Premigration iteration */
    long runIteration();

    void startFinal();

    /** Getters */
    long getCheckpointSize();
    long getDBSize();
    float getIterationTime();

    private:
    int iterationNumber;
    std::string path;
//    std::vector<simtime_t> iterationCheckpointTime;
    std::vector<simtime_t> iterationDatabaseTime;
    float iterationTime = 0;
    double iterationSize = 0;
    long dbSize;
    long checkpointSize;

    simtime_t timeDB;
    simtime_t timeCheckpoint;
};

#endif