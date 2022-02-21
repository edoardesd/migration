#include "helpers.h"

using namespace std;

#define BUFSIZE 128

using sysclock_t = std::chrono::system_clock;

int run_command(std::string cmd) {
    char buf[BUFSIZE];
    FILE *fp;

    cout << "CMD: " << cmd.c_str() << endl;

    if ((fp = popen(cmd.c_str(), "r")) == NULL) {
        cout << "Error opening pipe!" << endl;
        return -1;
    }
    while (fgets(buf, BUFSIZE, fp) != NULL) {
        cout << "OUTPUT: " << buf;
    }
    if(pclose(fp))  {
        cout << "Command not found or exited with error status" << endl;
        return -1;
    }
    return 0;
}

std::string return_command(std::string cmd) {
    char buf[BUFSIZE];
    string text;
    FILE *fp;

    cout << "CMD: " << cmd.c_str() << endl;

    if ((fp = popen(cmd.c_str(), "r")) == NULL) {
        cout << "Error opening pipe!" << endl;
        return "err";
    }
    while (fgets(buf, BUFSIZE, fp) != NULL) {
        cout << "OUTPUT: " << buf;
    }
    if(pclose(fp))  {
        cout << "Command not found or exited with error status" << endl;
        return "err";
    }

    text.assign(buf);

    return removeNewLine(text);
}

float command_time(std::string cmd) {
    char buf[BUFSIZE];
    FILE *fp;

    cout << "CMD: " << cmd.c_str() << endl;

    auto start = high_resolution_clock::now();
    if ((fp = popen(cmd.c_str(), "r")) == NULL) {
        cout << "Error opening pipe!" << endl;
        return -1;
    }
    while (fgets(buf, BUFSIZE, fp) != NULL) {
        cout << "OUTPUT: " << buf;
    }
    if(pclose(fp))  {
        cout << "Command not found or exited with error status" << endl;
        return -1;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    return duration.count()/1000000.0;
}

void print_time(simtime_t checkpoint, simtime_t database){
    cout << endl << "=== ELAPSED TIME ===" << endl;
    cout << "Checkpoint: " << checkpoint << " sec." << endl;
    cout << "Database dump: " << database << " sec." << endl;
    cout << endl << endl;
}

long GetFileSize(std::string filename){
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string percentage(long a, long b){
    std::stringstream stream;
    stream << "-->" << std::fixed << std::setprecision(2) << ((double)a/b)*100 << "%";
    return stream.str();
}

std::string removeNewLine(std::string s){
    if (!s.empty() && s[s.length()-1] == '\n') {
        s.erase(s.length()-1);
    }
    return s;
}

std::string getDay(){
    time_t now = sysclock_t::to_time_t(sysclock_t::now());

    char buf[30] = { 0 };
    strftime(buf, sizeof(buf), "%m%d", localtime(&now));

    return string(buf);
}

std::string getTime(){
    std::time_t now = sysclock_t::to_time_t(sysclock_t::now());

    char buf[30] = { 0 };
    std::strftime(buf, sizeof(buf), "%H-%M", std::localtime(&now));

    return std::string(buf);
}

//void createFolder(std::string path){
//    if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
//        if( errno == EEXIST ) {
//        } else {
//            cout << "cannot create folder, error:" << strerror(errno) << endl;
//            throw runtime_error( strerror(errno) );
//        }
//    }
//}

std::string createPath(){
    std::string path;
    path = "/home/antedo/migration/results_criu/";
    path += getDay() + "/" + getTime();
//    createFolder(path);
    return path;
}

int createDirTreeRecursive(std::string path){
    const char *charPath = path.c_str();
    if (strcmp(charPath, "/") == 0) // No need of checking if we are at root.
        return 0;

    // Check whether this dir exists or not.
    struct stat st;
    if (stat(charPath, &st) != 0 || !S_ISDIR(st.st_mode)){
        // Check and create parent dir tree first.
        char *path2 = strdup(charPath);
        char *parent_dir_path = dirname(path2);
        if (createDirTreeRecursive(parent_dir_path) == -1){
            cout << "cannot create folder, error." << endl;
//            throw runtime_error(strerror(errno));
            return -1;
        }

        // Create this dir.
        if (mkdir(charPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
            cout << "cannot create folder, error." << endl;
            return -1;
        }
    }

    return 0;
}

int charToInt(char s){
    stringstream str;
    str << s;
    int x;
    str >> x;
    return x;
}