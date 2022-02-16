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
#include "SimGod.h"

Define_Module(SimGod);

void SimGod::initialize()
{
    EV << "[SIMGOD] === Welcome from SimGod ===" << endl;

    MQTTpublishers = par("MQTTpublishers");
    MQTTsubscribers = par("MQTTsubscribers");
    MQTTmsgCount = par("MQTTmsgCount");
    MQTTmsgSize = par("MQTTmsgSize");
    MQTTmsgQoS = par("MQTTmsgQoS");
    MQTTmsgInterval = par("MQTTmsgInterval");
    MQTTmsgPretime = par("MQTTmsgPretime");
    debugMode = par("debugMode");

    clientsStartUp = par("clientsStartUp");

    cMessage* wait_clients = new cMessage("wait_clients", WAIT_CLIENTS);
    scheduleAt(simTime() + clientsStartUp, wait_clients); 

    resultPath = createPath();
    EV << "Path: " << resultPath << endl;
    createDirTreeRecursive(resultPath);

    if (!debugMode){
        run_command("sudo podman kill --all");
    }

}

void SimGod::handleMessage(cMessage *msg){
    switch (msg->getKind()) {
        case WAIT_CLIENTS: {
            runMQTTClients();
            break;
        }

        default: {
            EV_ERROR << "[SIMGOD] Message type error!!" << endl;
            break;
        }
    }
    delete msg;
}

void SimGod::registerMigrationMSG(C2XMessage* msg){
    migrationInfo migMsg;
    migMsg.msgId = msg->getId();
    migMsg.bytes = msg->getByteLength();
    migMsg.genTime = msg->getSendTime();
    migMsg.expectedMsg = msg->getExpectedPackets();
    migMsg.totSize = msg->getMigrationSize();

    EV_DEBUG << "Migration Message registered num " << msg->getName() << endl;

    auto patternCheckpoint {"checkpoint"};
    auto patternDB {"database"};
    auto rxCheckpoint = std::regex{ patternCheckpoint, std::regex_constants::icase };
    auto rxDatabase = std::regex{ patternDB, std::regex_constants::icase };
    bool isCheckpoint = std::regex_search(msg->getName(), rxCheckpoint);
    bool isDatabase = std::regex_search(msg->getName(), rxDatabase);

    if (isCheckpoint){
        migrationCheckpoint.push_back(migMsg);
    }

    if (isDatabase){
        migrationDatabase.push_back(migMsg);
    }
}

void SimGod::runMQTTClients(){
    EV << "[SIMGOD] Start clients NOW!" << endl;
    ipAddressCH = return_command("sudo podman inspect broker0 --format \"{{.NetworkSettings.IPAddress}}\"");

    string pubCmd = "sudo podman run -d --rm --name bench-pub docker.io/flipperthedog/mqtt-bench mqtt-bench -action=pub";
    pubCmd += " -broker=tcp://" + ipAddressCH + ":1883";
    pubCmd += " -clients=" + to_string(MQTTpublishers) ;
    pubCmd += " -count=" + to_string(MQTTmsgCount);
    pubCmd += " -size=" + to_string(MQTTmsgSize);
    pubCmd += " -qos=" + to_string(MQTTmsgQoS);
    pubCmd += " -intervaltime=" + to_string(MQTTmsgInterval);
    pubCmd += " -pretime=" + to_string(MQTTmsgPretime);
    pubCmd += " -x";

    string subCmd = "sudo podman run -d --rm --name bench-sub docker.io/flipperthedog/mqtt-bench mqtt-bench -action=sub";
    subCmd += " -broker=tcp://" + ipAddressCH + ":1883";
    subCmd += " -clients=" + to_string(MQTTsubscribers) ;
    subCmd += " -count=" + to_string(MQTTpublishers*MQTTmsgCount-MQTTpublishers*2);
    subCmd += " -qos=" + to_string(MQTTmsgQoS);
    subCmd += " -intervaltime=" + to_string(MQTTmsgInterval);
    subCmd += " -pretime=" + to_string(0);
    subCmd += " -topic=\"/mqtt-bench/benchmark/#\" -x";

    run_command(pubCmd);
    run_command(subCmd);

}

void SimGod::finish() {
    if (migrationCheckpoint.size() > 0){
        migrationSummary(migrationCheckpoint, "CHECKPOINT");
        migrationSummary(migrationDatabase, "DATABASE");
    }
    //TODO add recovery time on the second machine
}

void SimGod::migrationSummary(std::vector<migrationInfo> _vector, string type){
    int totalBytes = 0;
    for (auto i: _vector){
        totalBytes += i.bytes;
    }
    int totalMsg = _vector.size();
    simtime_t totalTime = _vector.back().recvTime - _vector.front().genTime;
    EV << "------------------------------------" << endl;
    EV << "FINAL SUMMARY: " << type << endl;
    EV << "Chunk transmitted: " << totalMsg << "/" << _vector.back().expectedMsg << percentage(totalMsg, _vector.back().expectedMsg) << endl;
    EV << "Bytes transmitted: " << totalBytes << "/" << _vector.back().totSize << percentage(totalBytes, _vector.back().totSize) << endl;
    EV << "Time elapsed: " << totalTime << " sec. " <<endl;
}

std::string SimGod::getPath(){
    return resultPath;
}