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

#include "containerVehicles.h"

#include "veins/modules/utility/Consts80211p.h"
#include "veins/base/utils/SimpleAddress.h"
#include "veins/base/modules/BaseMacLayer.h"
#include "veins/base/utils/FindModule.h"

#include <algorithm>
#include <vector>
#include <string>

using namespace veins;
using namespace std;

Define_Module(containerVehicles);

containerVehicles::~containerVehicles() {
    // Auto-generated Destructor-stub
}

void containerVehicles::initialize(int stage) {
    BaseApplLayer::initialize(stage);

    if (stage == 0) {
        mac = FindModule<DemoBaseApplLayerToMac1609_4Interface*>::findSubModule(getParentModule());
        simGod = FindModule<SimGod*>::findSubModule(getParentModule()->getParentModule());
        ASSERT(mac);
        ASSERT(simGod);

        mobility = TraCIMobilityAccess().get(getParentModule());
        traciVehicle = mobility->getVehicleCommandInterface();
        defaultSpeed = traciVehicle->getSpeed();
        traciVehicle->setSpeed(0);


        maxPacketLength = par("maxPacketLength");
        beaconPacketLength = par("beaconPacketLength");
        beaconInterval = par("beaconIntervalDefault");
        sendBeaconsMode = par("sendBeacons");
        migrationStart = par("migrationStart");
        migrationType = par("migrationType");
        preMigrationInterval = par("preMigrationInterval");
        isClusterHead = par("clusterHead");


        applId = myApplAddr();
        if (applId == 0){
            isClusterHead = true;
            EV << applId << " - I'm the cluster head." << endl;
        }

        string container_cmd = "sudo podman run -d --rm --image-volume ignore --name broker";
        container_cmd += to_string(applId);
        container_cmd += " -v /home/antedo/migration/config/mosquitto.conf:/mosquitto/config/mosquitto.conf docker.io/eclipse-mosquitto";
        run_command(container_cmd);

        sendBeaconEvt = new cMessage("BeaconEVNT", SELF_SEND_BEACON);
        moveEvt = new cMessage("MoveEVT", SELF_SEND_MOVE);
        preMigrationEvt = new cMessage("PreMigration", SELF_PRE_MIGRATION);

        simtime_t firstBeacon = simTime();

        cm = new ColdMigration(simGod->getPath());
        pm = new PreMigration(simGod->getPath());
    }
    else if (stage == 1) {
        applId = myApplAddr();
        myMAC = mac->getMACAddress();

        EV << "ApplID: " << applId << " - MAC ADDR: " << myMAC << endl;

        if (sendBeaconsMode) {
            EV << applId << " - Setting beacon time." << endl;
            simtime_t randomOffset = dblrand() * beaconInterval;
            firstBeacon = simTime() + randomOffset;
            scheduleAt(firstBeacon+1, sendBeaconEvt);
        }

    }
}


void containerVehicles::handleSelfMsg(cMessage* msg) {
    switch (msg->getKind()) {
        case SELF_SEND_BEACON: {
            if (defaultSpeed == 0){
                defaultSpeed = traciVehicle->getSpeed();
            }
            EV << applId << " - Sending beacon at time " << simTime() << "..." << endl;
            C2XMessage* pkt = new C2XMessage("BeaconMSG");
            populateAndSendPacket(pkt, LAddress::L2BROADCAST(), beaconPacketLength, BEACON_PRIORITY);
            scheduleAt(simTime() + beaconInterval, msg);
            break;
        }

        case SELF_SEND_MOVE: {
//            EV << "Given speed: " << defaultSpeed << endl;
//            traciVehicle->setSpeed(defaultSpeed);
            EV << applId << " - Timer expired. Moving now." << endl;
            if (isClusterHead){
                if (migrationType == 0){
                    // start cold migration
                    cm->start();

                    sendMigrationPacket(cm->getCheckpointSize(), "COLD_checkpoint");
                    sendMigrationPacket(cm->getDBSize(), "COLD_database");
                }

                if (migrationType == 1){
//                    EV << "Check p size: " << pm->getCheckpointSize() << " bytes. Database size: " << pm->getDBSize() << " bytes."<< endl;

                    pm->startFinal();
                    sendMigrationPacket(pm->getCheckpointSize(), "PRE_checkpoint");
                    sendMigrationPacket(pm->getDBSize(), "PRE_database");
                }
                EV << "Check p size: " << cm->getCheckpointSize() << " bytes. Database size: " << cm->getDBSize() << " bytes."<< endl;

                migrationDone = true;
            }
            delete msg;
            break;
        }

        case SELF_PRE_MIGRATION: {
            if (migrationDone == false){
                preMigrationSize.push_back(pm->runIteration());
                preMigrationTime.push_back(pm->getIterationTime());
                scheduleAt(simTime() + preMigrationInterval, msg);
            }
            break;
        }

        default: {
            throw new cRuntimeError("unknown self message type");
            break;
        }
    }
}


void containerVehicles::handleLowerMsg(cMessage *msg) {
    // received a packet from MAC
    C2XMessage* pkt = dynamic_cast<C2XMessage*>(msg);
    if (pkt) {
        switch (pkt->getUserPriority()) {
        case BEACON_PRIORITY:
            beaconReceived(pkt);
            break;

        case MIGRATION_PRIORITY:
            EV << applId << " - Migration received: " << pkt->getName() << endl;
            simGod->registerMigrationMSG(pkt);
            break;

        default:
            throw new cRuntimeError("unknown user priority of received packet!");
            break;
        }
    }
    cancelAndDelete (msg);
}


void containerVehicles::sendMigrationPacket(long size, string basename){
    C2XMessage* pkt;
    int numMigrationPackets = ceil((double)size/maxPacketLength);
    long msgLength = maxPacketLength;
    int totBytes = 0;
    string msgname;
    basename += "-#";

    EV << "Total # of packets " << numMigrationPackets << endl;

    for (int i=0; i<numMigrationPackets; i++){
        msgname = basename + to_string(i);
        pkt = new C2XMessage(msgname.c_str());
        pkt->setExpectedPackets(numMigrationPackets);
        pkt->setMigrationSize(size);
        if (i == numMigrationPackets-1){
            msgLength = size % maxPacketLength;
            totBytes += msgLength;
            EV_DEBUG << "Packet length of the last message " << msgLength << endl;
        }
        else {
            totBytes += maxPacketLength;
        }

        populateAndSendPacket(pkt, LAddress::L2BROADCAST(), msgLength, MIGRATION_PRIORITY);
    }
}


void containerVehicles::beaconReceived(C2XMessage* newPkt){
    int receivedID = newPkt->getSender();

    // For graphical purposes, setting the car green
    getParentModule()->getDisplayString().setTagArg("i", 1, "green");
    EV << myApplAddr() << " - Beacon # " << newPkt->getId() <<" received from " << receivedID << endl;

    if ( std::find(inRange.begin(), inRange.end(), receivedID) != inRange.end() ){
        EV_DEBUG << myApplAddr() << " - is already in range. Nothing to do." << endl;
    } else {
        inRange.push_back(receivedID);
        EV_DEBUG << myApplAddr() << " - Adding " << receivedID << " to the neighbour list" << endl;
//        defaultSpeed = traciVehicle->getSpeed();
//        traciVehicle->setSpeed(0);
        getParentModule()->getDisplayString().setTagArg("i", 1, "red");

        scheduleAt(simTime() + migrationStart, moveEvt);

        if (isClusterHead){
            if (migrationType == 1){
                scheduleAt(simTime(), preMigrationEvt);
            }
        }
    }
}


void containerVehicles::populateAndSendPacket(C2XMessage* pkt, LAddress::L2Type address, long pktLength, int userType){
//    pkt->setRecipientAddress(LAddress::L2BROADCAST());
    pkt->setRecipientAddress(address);

    pkt->setByteLength(pktLength);
    pkt->setPsid(-1);
    pkt->setChannelNumber(static_cast<int>(Channel::cch));
    pkt->setUserPriority(userType);
    pkt->setId(getSimulation()->getUniqueNumber());
    pkt->setSender(myMAC);
    pkt->setSendTime(simTime()); // TODO set right send time

    sendDown(pkt);
}


LAddress::L2Type containerVehicles::getMyMAC(){
    return myMAC;
}

void containerVehicles::sendDown(cMessage* msg) {
    BaseApplLayer::sendDown(msg);
}

void containerVehicles::finish() {

    if (isClusterHead){
        if (migrationType == 0){
        EV << "------------------------------------" << endl;
        EV << "Checkpoint dump time: " << cm->getTimeCheckpoint() << endl;
        EV << "Database dump time: " << cm->getTimeDB() << endl;
        }
        if (preMigrationTime.size() > 0){
            EV << "------------------------------------" << endl;
            EV << "Pre-Migration partial times: " << endl;
            for (auto t: preMigrationTime){
                EV << t << "    " ;
            }
            EV << endl;

            EV << "Pre-Migration partial size: " << endl;
            for (auto t: preMigrationSize){
                EV << t << "    " ;
            }
            EV << endl;

            EV << "Total pre migration time: " << pm->getTotalPreMigrationTime() << endl;
        }
    }

//    cancelAndDelete(sendBeaconEvt);
//    cancelAndDelete(moveEvt);
}

