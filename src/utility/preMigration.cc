#include "preMigration.h"

using namespace std;

// TODO broker name as param (broker0)

/** PREMIGRATION */

PreMigration::PreMigration(std::string path, double size){
    this->path = path;
    this->iterationNumber = 0;
    this->size = size;
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

//        this->iterationSize = GetFileSize(this->path + "/" + diffFileName) * 2;
        this->iterationSize = this->size * 1/7;
    } else {
        this->startPreMigTime = high_resolution_clock::now();
//        this->iterationSize = GetFileSize(this->path + "/0_database.db");
        this->iterationSize = this->size * 3/7;
    }

    cout << "Size before: " << this->size << " size after: " << this->iterationSize << endl;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    this->iterationTime = duration.count()/1000000.0;
    this->iterationNumber++;
//    lastPreMigTime = high_resolution_clock::now();
    this->sentSize += iterationSize;
    return this->iterationSize;
}

float PreMigration::getIterationTime(){
    return this->iterationTime;
}

void PreMigration::setTotalPreMigrationTime(){
//    cout << "coumpte sthis stajisad" << this->startPreMigTime << endl;
    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - this->startPreMigTime);
    cout << duration.count() << endl;
    this->totalPreMigrationTime = duration.count()/1000000.0;
//    cout  << final << endl;
//    return final;
}

float PreMigration::getTotalPreMigrationTime(){
    return this->totalPreMigrationTime;
}

long PreMigration::getCheckpointSize(){
    string path = this->path + "/final_checkpoint.tar.gz";
    this->checkpointSize = GetFileSize(path);
    return this->checkpointSize;
}

long PreMigration::getDBSize(){
    string path = this->path + "/final_diff.txt";
    this->dbSize = GetFileSize(path);
    if (!this->dbSize)
        this->dbSize = 1;

    if (this->dbSize < this->size * 1/7)
        this->dbSize = this->size * 1/7;

    this->sentSize += this->dbSize;
    return this->dbSize;
}

long PreMigration::getSentSize(){
    return this->sentSize;
}

void PreMigration::startFinal(){
    cout << endl;
    cout << "+++ Starting final pre migration +++" << endl;

    setTotalPreMigrationTime();
    cout << "Total PreMigration time: " << getTotalPreMigrationTime() << endl;
//    EV << "PATH check " << this->path <<endl;

    string cmd_checkpoint = "sudo podman container checkpoint --leave-running --tcp-established --export ";
    cmd_checkpoint += this->path + "/final_checkpoint.tar.gz broker0";
    this->timeCheckpoint = command_time(cmd_checkpoint);
    cout << "Time check " << this->timeCheckpoint << endl;

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

simtime_t PreMigration::getTimeDB(){
    return this->timeDB;
}

simtime_t PreMigration::getTimeCheckpoint(){
    return this->timeCheckpoint;
}

std::string PreMigration::getIterationName(){
    string name = to_string(this->iterationNumber) + "-ITER";
    return name;
}