#include "coldMigration.h"

using namespace std;

// TODO broker name as param (broker0)

ColdMigration::ColdMigration() {}

ColdMigration::ColdMigration(std::string path) {
//    this->path = "/home/antedo/migration/results_criu/";
//    this->path += getDay() + "/" + getTime();
//
//    createFolder(this->path);
    this->path = path;
}

void ColdMigration::start(){
    cout << endl;
    cout << "+++ Starting cold migration +++" << endl;

    EV << "PATH check " << this->path <<endl;

    string cmd_checkpoint = "sudo podman container checkpoint --leave-running --tcp-established --export ";
    cmd_checkpoint += this->path + "/final_checkpoint.tar.gz broker0";
    this->timeCheckpoint = command_time(cmd_checkpoint);

    cout << "+++ Dumping the database +++" << endl;
    string cmd_db = "sudo podman cp broker0:/mosquitto/data/mosquitto.db " + this->path + "/final_database.db";
    this->timeDB = command_time(cmd_db);

//    print_time(time_checkpoint, time_db);
}

long ColdMigration::getCheckpointSize(){
    string path = this->path + "/final_checkpoint.tar.gz";
    this->checkpointSize = GetFileSize(path);
    return this->checkpointSize;
}

long ColdMigration::getDBSize(){
    string path = this->path + "/final_database.db";
    this->dbSize = GetFileSize(path);
    return this->dbSize;
}

simtime_t ColdMigration::getTimeDB(){
    return this->timeDB;
}

simtime_t ColdMigration::getTimeCheckpoint(){
    return this->timeCheckpoint;
}
