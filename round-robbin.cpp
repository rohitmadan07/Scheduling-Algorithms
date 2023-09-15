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
            return a.priority > b.priority;
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

void print(priority_queue<DataPoint,vector<DataPoint>, compare>& q){
    while(!q.empty()){
        DataPoint point = q.top();
        q.pop();
        cout << point.name << " " << point.arrTime << " " << point.execTime <<" "<<point.burstTime << " " << point.remainingBurstTime << " " << point.ioBlockTime << " " << point.priority << endl;
    }
    cout<<endl;
}

int main() {
    // Open the input file
    string fileName;
    cout<<"Enter Filename:- "<<endl;
    cin>>fileName;
    ifstream inputFile(fileName);
    // Check if the file is open
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    cout<<"The name of the file to be read :- "<<fileName<<endl;

    vector<DataPoint> data; // Vector to store the data
    
    priority_queue<DataPoint,vector<DataPoint>, compare> readyQueue;

    // Read data from the file into the vector
    while (true) {
        DataPoint point;
        if (inputFile >> point.name >> point.arrTime >> point.execTime >> point.burstTime >> point.ioBlockTime >> point.priority) {
            point.remainingBurstTime = point.burstTime;
            data.push_back(point);
            readyQueue.push(point);
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
        cout << point.name << " " << point.arrTime << " " << point.execTime << " "<<point.burstTime<<" "  << point.remainingBurstTime << " " << point.ioBlockTime << " " << point.priority << endl;    
        cout<<endl;
    }

    map<string,double> initialArrTime;
    for(const auto& point : data){
        initialArrTime[point.name] = point.arrTime;
    }

    map<string,double> tat;
    map<string,double> wt;
    double timeSlice = 3.0;
    double timer = data[0].arrTime;
    priority_queue<DataPoint,vector<DataPoint>, compare> tmp = readyQueue;
    print(tmp);

    while(true){
        //in every time slice a process has to be picked with min arrival time
        //every iteration is a single new time slice
        DataPoint curr;
        if(!readyQueue.empty()){
            curr = readyQueue.top(); //pointer to current process
            readyQueue.pop();
            cout<<"Process execution start:- "<<curr.name<<endl;
        }
        double currNextArrival = -1; 
        double startTime = timer; //starting timer
        cout<<"Starting time:- "<<startTime<<endl;
        if(startTime < curr.arrTime){
            timer = curr.arrTime; //taking timer to arrival time of current
            startTime = curr.arrTime;
            //CPU IDLE
        }
        else{
            wt[curr.name] += timer - curr.arrTime; 
        }

        cout<<"Before Updating "<<curr.name << " " << curr.arrTime << " " << curr.execTime <<" "<<curr.burstTime<< " " << curr.remainingBurstTime << " " << curr.ioBlockTime << " " << curr.priority << endl;    


        //have to give CPU to curr for timeSlice, if it gets over in b/w then CPU idle
        if(curr.remainingBurstTime > timeSlice){
            //would have run for entire timeSlice
            if(curr.execTime > curr.remainingBurstTime){
                curr.remainingBurstTime -= timeSlice;
                curr.execTime -= timeSlice;
                timer+=timeSlice; //complete time slice
                //calculate next arrival
                currNextArrival = max(startTime,curr.arrTime) + timeSlice ; //=timer,as exec stopped im b/w

            }
            else if(curr.execTime <= curr.remainingBurstTime){
                //exec finishes in b/w
                //calculate tat
                //out of timeSlice it executed for curr->execTime
                //startTime -> when this process picked
                timer+=curr.execTime;
                tat[curr.name] = startTime + curr.execTime - initialArrTime[curr.name];
                currNextArrival = -1; //will not arrive after this
            }
        }
        else if(curr.remainingBurstTime <= timeSlice) {
             cout<<"HERE"<<endl;   
            //burst ends in b/w 
            if(curr.execTime > curr.remainingBurstTime){
                curr.execTime -= curr.remainingBurstTime;
                double temp = curr.remainingBurstTime; 
                curr.remainingBurstTime = 0;
                curr.remainingBurstTime = curr.burstTime; //Refresh

                //will go for IO
                //calculate next arrival
                timer+=temp;
                currNextArrival = max(startTime,curr.arrTime) + temp + curr.ioBlockTime;
            }
            else{
                //exec finishes in b/w
                //calculate tat
                timer+=curr.execTime;
                tat[curr.name] = startTime + curr.execTime - initialArrTime[curr.name];
                currNextArrival = -1;
            }
        }
        if(currNextArrival == -1){
            //no new entry in queue
            if(readyQueue.empty()) break;
            else continue; //nothing to push
        }

        DataPoint updated;
        updated.name = curr.name;
        updated.arrTime = currNextArrival;
        updated.remainingBurstTime = curr.remainingBurstTime;
        updated.burstTime = curr.burstTime;
        updated.execTime = curr.execTime;
        updated.ioBlockTime = curr.ioBlockTime;
        updated.priority = curr.priority;

        readyQueue.push(updated);
        tmp = readyQueue;
        
        cout<<endl;
        cout<<"After Updating "<<updated.name << " " << updated.arrTime << " " << updated.execTime << " " <<updated.burstTime<<" "<< updated.remainingBurstTime << " " << updated.ioBlockTime << " " << updated.priority << endl; 
        cout<<endl;
        print(tmp);
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