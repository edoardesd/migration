//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __SIMGOD_H_
#define __SIMGOD_H_

#include <vector>
#include <regex>
#include <stdlib.h>

#include "App2GodInterface.h"
#include "helpers.h"
#include "../message/C2XMessage_m.h"

using namespace std;

class SimGod : public cSimpleModule, App2GodInterface
{
    enum PktEvents {
           WAIT_CLIENTS
       };

       struct migrationInfo{
        long msgId;
        long totSize;
        int expectedMsg;
        int bytes;
        simtime_t genTime;
        simtime_t recvTime = simTime();
    };

public:
    /** Store struct msg in a vector for further analysis */
    void registerMigrationMSG(C2XMessage *msg);
    std::string getPath();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void runMQTTClients();

private:
    std::vector<migrationInfo> migrationCheckpoint;
    std::vector<migrationInfo> migrationDatabase;
    std::vector<migrationInfo> migrationIterations[10];
    int expectedCheckpoint;
    int expectedDatabase;
    int iterNum = 0;
    long sizeCheckpoint;
    long sizeDatabase;
    bool debugMode = true;
    std::string ipAddressCH;
    std::string resultPath;
    simtime_t clientsStartUp;

    int MQTTpublishers;
    int MQTTsubscribers;
    int MQTTmsgCount;
    int MQTTmsgSize;
    int MQTTmsgQoS;
    int MQTTmsgInterval;
    int MQTTmsgPretime;

    /** Print stats for migration type */
    void migrationSummary(std::vector<migrationInfo> _vector, string type);
    void preMigrationSummary();


};

#endif
