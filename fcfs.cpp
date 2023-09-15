#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <math.h>

using namespace std;

// Define a structure to represent the data
struct DataPoint {
    int index;
    string name;
    double arrTime;
    double execTime;
    double burstTime;
    double ioBlockTime;
    int priority;
};

class compare{
    public:
    bool operator()(DataPoint a, DataPoint b) {
        if(a.arrTime == b.arrTime){
            return a.index > b.index;
        }
        else{
            return a.arrTime > b.arrTime;
        }
    }
};


// Function to compare two DataPoint objects for sorting
bool compareByarrTime(const DataPoint& a, const DataPoint& b) {
    return a.arrTime < b.arrTime;
}

DataPoint* getMinArrTimeProcess(vector<DataPoint>& arr){
    double mini = 1e9;
    DataPoint* res;
    for(int i=0; i<arr.size(); i++){
        if(arr[i].arrTime == -1) continue; //its execution is already completed
        if(mini > arr[i].arrTime){
            mini = arr[i].arrTime;
            res = &(arr[i]);
        }
    }
    if(mini == 1e9) res = NULL; 
    return res;
}

int main() {
    // Open the input file
    string fileName;
    cout<<"Enter Filename:- "<<endl;
    cin>>fileName;
    ifstream inputFile(fileName);
    // Check if the file is open
    if (!inputFile.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    cout<<"The name of the file to be read :- "<<fileName<<endl;

    vector<DataPoint> data; // Vector to store the data
    priority_queue<DataPoint,vector<DataPoint>, compare> Queue;

    // Read data from the file into the vector
    int dataPtr = 0;
    while (true) {
        DataPoint point;
        if (inputFile >> point.name >> point.arrTime >> point.execTime >> point.burstTime >> point.ioBlockTime >> point.priority) {
            point.index = dataPtr;
            data.push_back(point);
            Queue.push(point);
            dataPtr++;
        } else {
            break;
        }
    }

    // Close the file
    inputFile.close();

    // Sort the data based on the first arrTime
    sort(data.begin(), data.end(), compareByarrTime);

    // Print the sorted data
    for (const auto& point : data) {
        cout << point.name << " " << point.arrTime << " " << point.execTime << " " << point.burstTime << " " << point.ioBlockTime << " " << point.priority << endl;    
        cout<<endl;
    }

    map<string,double> initialArrTime;
    for(const auto& point : data){
        initialArrTime[point.name] = point.arrTime;
    }

    map<string,double> tat;
    map<string,double> wt;
    double timer = data[0].arrTime;

    while(true){
        DataPoint process;
        if(!Queue.empty()){ 
            process = Queue.top();
            Queue.pop();
        }
        else break;
        DataPoint* execProcess = &process;
        if(execProcess == NULL) break;
        cout<<"Process which starts to execute " <<execProcess->name<<endl;
        cout<<"Time of exec start: "<<timer<<endl; //current process starts executing at this time
        wt[execProcess->name] += max(0.0,timer-execProcess->arrTime);
        double prevTime = timer; //before updating

        // updated timer
        if(execProcess->execTime >= execProcess->burstTime){
            execProcess->execTime -= execProcess->burstTime;
            timer = max(prevTime,execProcess->arrTime) + execProcess->burstTime;
        }
        else{
            //time left is less than burst time
            timer = max(prevTime,execProcess->arrTime) + execProcess->execTime;
            execProcess->execTime = 0.0;
        }
        
        //timer has been updated 
        if(execProcess->execTime == 0.0){
            tat[execProcess->name] = timer - initialArrTime[execProcess->name];
            execProcess->arrTime = -1;
        }
        else{
            execProcess->arrTime = max(prevTime,execProcess->arrTime) + execProcess->burstTime + execProcess->ioBlockTime;
        }

        if(execProcess->arrTime != -1){
            DataPoint updated;
            updated.name = execProcess->name;
            updated.arrTime = execProcess->arrTime;
            updated.burstTime = execProcess->burstTime;
            updated.execTime = execProcess->execTime;
            updated.ioBlockTime = execProcess->ioBlockTime;
            updated.priority = execProcess->priority;

            Queue.push(updated);
        }

        for (const auto& point : data) {
            cout << point.name << " " << point.arrTime << " " << point.execTime << " " << point.burstTime << " " << point.ioBlockTime << " " << point.priority <<endl;    
            cout<<endl;
        }
    }

    cout<<"DONE"<<endl;

    for(auto &it:tat){
        cout<<"Turnaround time of "<<it.first << " " << it.second <<endl;
    }

    for(auto &it:data){
        if(wt.find(it.name) == wt.end()){
            wt[it.name] = 0;
        }
    }

    for(auto &it:wt){
        cout<<"Waiting Time of "<<it.first << " " << it.second <<endl;
    }

    return 0;
}