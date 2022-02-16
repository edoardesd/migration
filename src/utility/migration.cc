#include "migration.h"

using namespace std;

// TODO broker name as param (broker0)

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

/** PREMIGRATION */
//TODO new file


PreMigration::PreMigration(std::string path){
    this->path = path;
    this->iterationNumber = 0;
}

long PreMigration::runIteration(){
    cout << " ---> Trigger for the Periodic Premigration -- Iteration number: " << this->iterationNumber << endl;

    auto start = high_resolution_clock::now();

    string dumpDatabase = "sudo podman cp broker0:/mosquitto/data/mosquitto.db ";
    dumpDatabase += this->path + "/" + to_string(iterationNumber) + "_database.db ";
    this->iterationDatabaseTime.push_back(command_time(dumpDatabase));

    string convertDB = "sudo /home/antedo/mosquitto/apps/db_dump/mosquitto_db_dump ";
    convertDB += this->path + "/" + to_string(this->iterationNumber) + "_database.db > " + this->path + "/" + to_string(this->iterationNumber) + "_db.txt";
    return_command(convertDB);

    if (this->iterationNumber > 0){
        string diffFileName = "diff_" + to_string(this->iterationNumber) + ".txt";
        cout << "Doing the db difference..." << endl;
        string diffCmd = "sudo diff " + this->path + "/" + to_string(this->iterationNumber) + "_db.txt ";
        diffCmd += this->path + "/" + to_string(this->iterationNumber-1) + "_db.txt > ";
        diffCmd += this->path + "/" + diffFileName;
        return_command(diffCmd);

        this->iterationSize = GetFileSize(this->path + "/" + diffFileName) * 2;
    } else {
        this->iterationSize = GetFileSize(this->path + "/0_database.db");
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    this->iterationTime = duration.count()/1000000.0;
    this->iterationNumber++;

    return this->iterationSize;
}

float PreMigration::getIterationTime(){
    return this->iterationTime;
}

long PreMigration::getCheckpointSize(){
    string path = this->path + "/final_checkpoint.tar.gz";
    this->checkpointSize = GetFileSize(path);
    return this->checkpointSize;
}

long PreMigration::getDBSize(){
    string path = this->path + "/final_diff.txt";
    this->dbSize = GetFileSize(path);
    return this->dbSize;
}

void PreMigration::startFinal(){
    cout << endl;
    cout << "+++ Starting final pre migration +++" << endl;

    EV << "PATH check " << this->path <<endl;

    string cmd_checkpoint = "sudo podman container checkpoint --leave-running --tcp-established --export ";
    cmd_checkpoint += this->path + "/final_checkpoint.tar.gz broker0";
    this->timeCheckpoint = command_time(cmd_checkpoint);

    cout << "+++ Dumping the database +++" << endl;
    string cmd_db = "sudo podman cp broker0:/mosquitto/data/mosquitto.db " + this->path + "/final_database.db";
    this->timeDB = command_time(cmd_db);

    string convertDB = "sudo /home/antedo/mosquitto/apps/db_dump/mosquitto_db_dump ";
    convertDB += this->path + "/final_database.db > " + this->path + "/final_database.txt";
    return_command(convertDB);

    cout << "Doing the db difference..." << endl;
    string diffCmd = "sudo diff " + this->path + "/" + "final_database.txt ";
    diffCmd += this->path + "/" + to_string(this->iterationNumber-1) + "_db.txt > ";
    diffCmd += this->path + "/final_diff.txt";
    return_command(diffCmd);

//    print_time(time_checkpoint, time_db);
}