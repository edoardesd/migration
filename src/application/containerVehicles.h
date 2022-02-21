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

#include "veins/base/modules/BaseApplLayer.h"
#include "veins/modules/mac/ieee80211p/DemoBaseApplLayerToMac1609_4Interface.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "../message/C2XMessage_m.h"
#include "../utility/SimGod.h"
#include "../utility/coldMigration.h"
#include "../utility/preMigration.h"

#include <numeric>
#include <string>

using namespace veins;
using namespace std;


class containerVehicles : public veins::BaseApplLayer{
public:
    ~containerVehicles() override;
    void initialize(int stage) override;
    void finish() override;

    LAddress::L2Type getMyMAC();

    enum C2XappMessageKinds {
        SELF_SEND_BEACON,
        SELF_SEND_MOVE,
        SELF_PRE_MIGRATION
    };

    enum PktEvents {
        MIGRATION_PRIORITY,
        BEACON_PRIORITY
//        PROXIMITY_PRIORITY,
    };

protected:
    /** @brief handle messages from lower layer, i.e., MAC */
    void handleLowerMsg(cMessage* msg) override;

    /** @brief handle self messages */
    void handleSelfMsg(cMessage* msg) override;

    /** @brief sets all the necessary fields in C2XMessage and send it to MAC */
    void populateAndSendPacket(C2XMessage* pkt, LAddress::L2Type address, long pktLength, int userType);

    /** Check whether new beacon vehicle is already in range */
    void beaconReceived(C2XMessage* newPkt);

    void sendMigrationPacket(long size, string basename);
    /**
     * @brief overloaded for error handling and stats recording purposes
     *
     * @param msg the message to be sent. Must be a BaseFrame1609_4
     */
    virtual void sendDown(cMessage* msg);


protected:
    long beaconPacketLength;
    long maxPacketLength;
    float defaultSpeed;
    int applId;
    int migrationStart;
    int migrationType;
    double dbSize;

    bool sendBeaconsMode;
    bool isClusterHead;
    bool migrationDone = false;

    DemoBaseApplLayerToMac1609_4Interface* mac;

    LAddress::L2Type myMAC = 0;
    simtime_t beaconInterval;
    simtime_t firstBeacon;
    simtime_t preMigrationInterval;

    vector<int> inRange;
    vector<float> preMigrationTime;
    vector<long> preMigrationSize;

    /* pointers set when used with TraCIMobility */
    TraCIMobility* mobility;
    TraCICommandInterface::Vehicle* traciVehicle;

    cMessage* sendBeaconEvt;
    cMessage* moveEvt;
    cMessage* preMigrationEvt;

    SimGod* simGod;
    ColdMigration* cm;
    PreMigration* pm;
//    cMessage* migrationTrigger;
//    cMessage* checkLifeMSG;
//    TimeoutMSG* timeoutMessage;

};
