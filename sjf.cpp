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
    string name;
    double arrTime;
    double execTime;
    double burstTime;
    double remainingBurstTime;
    double ioBlockTime;
    int priority;
};

class compare{
    public:
    bool operator()(DataPoint a, DataPoint b) {
        if(a.arrTime == b.arrTime){
            if(a.remainingBurstTime == b.remainingBurstTime){
                return a.priority > b.priority;
            }
            return a.remainingBurstTime > b.remainingBurstTime;
        }
        else{
            return a.arrTime > b.arrTime;
        }
    }
};


int main(){

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
    priority_queue<DataPoint,vector<DataPoint>, compare> tmp;
    map<string,double> tat;
    map<string,double> wt;
    // double timer = data[0].arrTime; 

    // Read data from the file into the vector
    while (true) {
        DataPoint point;
        if (inputFile >> point.name >> point.arrTime >> point.execTime >> point.burstTime >> point.ioBlockTime >> point.priority) {
            point.remainingBurstTime = point.burstTime;
            data.push_back(point);
            Queue.push(point);
        } else {
            break;
        }
    }

    map<string,double> initialArrTime;
    for(const auto& point : data){
        initialArrTime[point.name] = point.arrTime;
    }

    // Close the file
    inputFile.close();

    while (!Queue.empty()) {

        DataPoint currProcess = Queue.top();
        Queue.pop();

        // if(currProcess.arrTime < timer){
        //     while(!tmp.empty()){
        //         DataPoint temp = tmp.top();
        //         tmp.pop();
        //         if(temp.arrTime > timer){
        //             currProcess = temp;
        //             break;
        //         }
        //     }
        // }

        vector<DataPoint> processJustGreaterArr; //process with same arr will be maintained here and checked with just greater arr time
        bool noIncrementFound = true; //if false => we got an increment
        double increment;
        while (!Queue.empty() and noIncrementFound) {
            DataPoint process = Queue.top();
            processJustGreaterArr.push_back(process);
            Queue.pop();
            if (process.arrTime > currProcess.arrTime) {
                processJustGreaterArr.pop_back(); //removing to keep only those in arr which have same arr
                Queue.push(process);
                increment = process.arrTime - currProcess.arrTime;
                noIncrementFound = false;
            }
        }

        if (!noIncrementFound) {

            if (increment >= min(currProcess.execTime, currProcess.remainingBurstTime)) {

                for (int j = 0; j < processJustGreaterArr.size(); j++) {
                    wt[processJustGreaterArr[j].name] += min(currProcess.execTime, currProcess.remainingBurstTime);
                    processJustGreaterArr[j].arrTime += min(currProcess.execTime, currProcess.remainingBurstTime); 
                }

                currProcess.arrTime += min(currProcess.execTime, currProcess.remainingBurstTime) + currProcess.ioBlockTime;
                currProcess.execTime -= min(currProcess.execTime, currProcess.remainingBurstTime); 
                //timer += min(currProcess.execTime, currProcess.remainingBurstTime);
                currProcess.remainingBurstTime = currProcess.burstTime; //refresh
                if (currProcess.execTime > (1e-5))
                    Queue.push(currProcess);
                else
                    tat[currProcess.name] = currProcess.arrTime - currProcess.ioBlockTime - initialArrTime[currProcess.name];

            } else {
                currProcess.remainingBurstTime -= increment;
                currProcess.arrTime += increment;
                //timer+=increment;
                currProcess.execTime -= increment;
                for (int j = 0; j < processJustGreaterArr.size(); j++) {
                    wt[processJustGreaterArr[j].name] += increment;
                    processJustGreaterArr[j].arrTime += increment;
                }
                Queue.push(currProcess);
            }
        } else {
            //No Preemption

            for (int j = 0; j < processJustGreaterArr.size(); j++) {
                    wt[processJustGreaterArr[j].name] += min(currProcess.execTime, currProcess.remainingBurstTime);
                    processJustGreaterArr[j].arrTime += min(currProcess.execTime, currProcess.remainingBurstTime); 
                }

                currProcess.arrTime += min(currProcess.execTime, currProcess.remainingBurstTime) + currProcess.ioBlockTime;
                currProcess.execTime -= min(currProcess.execTime, currProcess.remainingBurstTime); 
                //timer+= min(currProcess.execTime, currProcess.remainingBurstTime);
                currProcess.remainingBurstTime = currProcess.burstTime;
                if (currProcess.execTime > (1e-5))
                    Queue.push(currProcess);
                else
                    tat[currProcess.name] = currProcess.arrTime - currProcess.ioBlockTime - initialArrTime[currProcess.name];
        }

        for (int j = 0; j < processJustGreaterArr.size(); j++)
            Queue.push(processJustGreaterArr[j]);

        tmp = Queue;

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
        cout<<"Waiting time of "<<it.first << " " << it.second <<endl;
    }

    return 0;
}