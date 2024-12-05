#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>
#include <algorithm>
#include "Process.h"
using namespace std;

void readInput(vector<Process>& processes);
void sortProcesses(vector<Process>&processes);
void addWaitTime(deque<Process>&schedule, int count);
void addTurnaroundTime(deque<Process>&schedule, int count);
void addWaitTime(priority_queue<Process>&schedule, int count);
void addTurnaroundTime(priority_queue<Process>&schedule, int count);
void addTurnaroundTime(priority_queue<Process>&schedule, int count);
void removeProcess(priority_queue<Process>&schedule, Process p);
// void AlgorithmA(vector<Process>processes);
// void AlgorithmB(vector<Process>processes);
void AlgorithmC(vector<Process>processes);

template <typename T>
void printInfo(string head, T completion_time, T wait_time, T turnaround_time, T context_switches){
    cout << head <<endl;
    cout << "Time of completion: " << completion_time << "ms" << endl;
    cout << "Time spent waiting: " << wait_time << "ms" << endl;
    cout << "  Turn Around time: " << turnaround_time << "ms" << endl;
    cout << "  Context Switches: " << context_switches << endl << endl;
}

int main(int argc, const char * argv[]){
    int quantum = 1;
    vector<Process>processes;
    readInput(processes); 
    sortProcesses(processes);
    // AlgorithmA(processes); // fifo
    // AlgorithmB(processes); // RR
    AlgorithmC(processes); // srtf

    return 0;
}

// read input from command line
void readInput(vector<Process>& processes) {
    int numProcesses, PID, arrivalTime, CPUTime;
    cin >> numProcesses;
    for (int i = 0; i < numProcesses; ++i) {
        Process process;
        cin >> PID >> arrivalTime >> CPUTime;
        process.setPID(PID);
        process.setArrivalTime(arrivalTime);
        process.setCPUTime(CPUTime);
        processes.push_back(process);
    }
}

// sort processes based on the arrival time in ascending order
void sortProcesses(vector<Process>&processes){
    std::sort(processes.begin(), processes.end(), [](const Process& p1, const Process& p2) {
        return p1.getArrivalTime() < p2.getArrivalTime();
    });
}

void addWaitTime(deque<Process>&schedule, int count){
    for(int i = 0; i < schedule.size(); ++i){
        schedule[i].addWaitTime(count);
    }
}

void addTurnaroundTime(deque<Process>&schedule, int count){
    for(int i = 0; i < schedule.size(); ++i){
        schedule[i].addTurnaroundTime(count);
    }
}

void addWaitTime(priority_queue<Process>&schedule, int count){
    Process temp;
    for(int i = 0; i < schedule.size(); ++i){
        temp = schedule.top();
        temp.addWaitTime(count);
        schedule.pop();
        schedule.push(temp);
    }
}

void addTurnaroundTime(priority_queue<Process>&schedule, int count){
    Process temp;
    for(int i = 0; i < schedule.size(); ++i){
        temp = schedule.top();
        temp.addTurnaroundTime(count);
        schedule.pop();
        schedule.push(temp);
    }
}

void removeProcess(priority_queue<Process>&schedule, Process p){
    vector<Process>copy;
    
    for(int i = 0; i < schedule.size(); ++i){
        copy.push_back(schedule.top());
        schedule.pop();
    }
    
    for(int i = 0; i < copy.size(); ++i){
        if(copy[i].getPID() == p.getPID()){
            copy.erase(copy.begin() + i);
        }
    }
    
    for(int i = 0; i < copy.size(); ++i){
        schedule.push(copy[i]);
    }
}

void AlgorithmA(vector<Process>processes){
    int i = 0, time = 0, c = 0;
    double averageCompletion = 0, avgWait = 0, avgTurn = 0, totalSwitch = 0;
    cout << "--Algorithm A starts--"<<endl;
    cout << "-- Grantt Chart -- (PID->CPU Burst Time)" << endl << "|";
    

    while(i < processes.size()){
        processes[i].addWaitTime(abs(time - processes[i].getArrivalTime()));
        while(true){
            if(processes[i].getCPUTime() > c){
                ++time;
                ++c;
            }else{
                processes[i].addTurnaroundTime(processes[i].getWaitTime() + processes[i].getCPUTime());
                processes[i].incNumOfContextSwitching();
                processes[i].setTimeOfCompletion(time);
                c = 0;
                break;
            }
        }
        ++i;
    }
    
    for(int i = 0; i < processes.size(); ++i){
        cout << " " << processes[i].getPID() << "->" << processes[i].getCPUTime() << " |";
    }
    
    cout << endl << endl << "---- Process List ----" << endl;
    
    for(int i = 0; i < processes.size(); ++i){
        cout << i + 1 << ":" << endl;
        if(i == processes.size() - 1) {
            processes[i].decNumOfContextSwitching();
        }
        printInfo("PID: " + std::to_string(processes[i].getPID()), processes[i].getTimeOfCompletion(), processes[i].getWaitTime(), processes[i].getTurnaroundTime(), processes[i].getNumOfContextSwitching());
        averageCompletion += processes[i].getTimeOfCompletion();
        avgWait += processes[i].getWaitTime();
        avgTurn += processes[i].getTurnaroundTime();
        totalSwitch += processes[i].getNumOfContextSwitching();
    }
    printInfo("Average STATS of algorithm A: ", ceil(averageCompletion / processes.size()), ceil(avgWait / processes.size()), ceil(avgTurn / processes.size()), totalSwitch);
    cout << "--Algorithm A ends--"<<endl;
}


void AlgorithmB(vector<Process>processes){

    int quantum = 3,time = 0, counter = 0, runTime = 0;
    double averageCompletion = 0, avgWait = 0, avgTurn = 0, totalSwitch = 0;
    bool next = false, change = false;
    Process *run = nullptr;
    deque<Process>schedule;
    vector<Process>copy;
    vector<Process>finished;

    
    for(int i = 0; i < processes.size(); ++i){
        Process *p = new Process();
        copy.push_back(*p);
        delete p;
    }
    
    for(int i = 0; i < processes.size(); ++i){
        copy[i] = processes[i];
    }
    
    while(true){
        if(copy[0].getArrivalTime() > time){
            ++time;
        }else{
            break;
        }
    }
    
    while(!copy.empty() || !schedule.empty() || counter != 0){
        while(true){
            if(copy[0].getArrivalTime() <= time && !copy.empty()){
                schedule.push_back(copy[0]);
                copy.erase(copy.begin());
            }else{
                break;
            }
        }
        
        if(counter <= 0){
            run = &schedule.front();
            schedule.pop_front();
            run->incNumOfContextSwitching();
        }else if(counter == quantum){
            if(run->getCPUTime() <= 0){
                run->setTimeOfCompletion(time);
                cout << " " << run->getPID() << "->" << runTime << " |";
                change = true;
                finished.push_back(*run);
                counter = -1;
                next = true;
                --time;
            }else{
                cout << " " << run->getPID() << "->" << runTime << " |";
                change = true;
                schedule.push_back(*run);
                counter = -1;
                next = true;
                --time;
            }
        }
        
        if(!next){
            if(run->getCPUTime() <= 0){
                cout << " " << run->getPID() << "->" << runTime << " |";
                change = true;
                run->setTimeOfCompletion(time);
                finished.push_back(*run);
                counter = -1;
                --time;
            }else{
                addTurnaroundTime(schedule, 1);
                addWaitTime(schedule, 1);
                run->addTurnaroundTime(1);
                run->setCPUTime(run->getCPUTime() - 1);
            }
        }
        
        if(next){
            next = false;
        }
        
        ++runTime;
        ++counter;
        ++time;
        
        if(change){
            change = false;
            runTime = 0;
        }
    }
    cout << endl << endl << "---- Process List ----" << endl;
    for(int i = 0; i < finished.size(); ++i){
        cout << i + 1 << ":" << endl;
        if(i == finished.size() - 1) {
            finished[i].decNumOfContextSwitching();
        }
        printInfo("PID: " + std::to_string(finished[i].getPID()), finished[i].getTimeOfCompletion(), finished[i].getWaitTime(), finished[i].getTurnaroundTime(), finished[i].getNumOfContextSwitching());
        averageCompletion += finished[i].getTimeOfCompletion();
        avgWait += finished[i].getWaitTime();
        avgTurn += finished[i].getTurnaroundTime();
        totalSwitch += finished[i].getNumOfContextSwitching();
    }
    printInfo("Average STATS of algorithm B: ", ceil(averageCompletion / finished.size()), ceil(avgWait / finished.size()), ceil(avgTurn / finished.size()), totalSwitch);
    cout << "--Algorithm B ends--"<<endl;
}



void AlgorithmC(vector<Process>processes){
    int time = 0, runTime = 0;
    double averageCompletion = 0, avgWait = 0, avgTurn = 0, totalSwitch = 0;
    bool firstRun = true, change = false, preempt = false;
    priority_queue<Process>schedule;
    Process run;
    vector<Process>copy;
    vector<Process>finished;
    cout << "--Algorithm C starts--"<<endl;
    cout << "-- Gnatt Chart -- (PID->CPU Burst Time)" << endl << "|";
    
    for(int i = 0; i < processes.size(); ++i){
        Process *p = new Process();
        copy.push_back(*p);
        delete p;
    }
    
    for(int i = 0; i < processes.size(); ++i){
        copy[i] = processes[i];
    }
    
    while(true){
        if(processes[0].getArrivalTime() > time){
            ++time;
        }else{
            break;
        }
    }
    
    while(finished.size() < processes.size()){
        while(true){
            if(copy[0].getArrivalTime() <= time && !copy.empty()){
                schedule.push(copy[0]);
                copy.erase(copy.begin());
            }else{
                break;
            }
        }
        if(firstRun){
            run = schedule.top();
            schedule.pop();
            firstRun = false;
        }
        if(!schedule.empty() && schedule.top().getCPUTime() < run.getCPUTime()){
            cout<<1<<endl;
            run.incNumOfContextSwitching();
            cout << " " << run.getPID() << "->" << runTime << " |";
            schedule.push(run);
            run = schedule.top();
            change = true;
            preempt = true;
            schedule.pop();
        }else if(run.getCPUTime() <= 0){
            cout<<2<<endl;
            run.incNumOfContextSwitching();
            run.setTimeOfCompletion(time);
            cout << " " << run.getPID() << "->" << runTime << " |";
            finished.push_back(run);
            run = schedule.top();
            change = true;
            schedule.pop();
        }else{
            cout<<3<<endl;
            run.setCPUTime(run.getCPUTime() - 1);
            run.addTurnaroundTime(1);
            addTurnaroundTime(schedule, 1);
            addWaitTime(schedule, 1);
        }
        
        ++runTime;
        ++time;
        if(preempt) {
            --time;
            preempt = false;
        }
        if(change){
            change = false;
            runTime = 0;
        }
        cout<<" current time: "<<time-1<<", and current process: "<<run.getPID()<<", "<<run.getCPUTime()<<". "<<endl;
    }
    
    cout << endl << endl << "---- Process List ----" << endl;
    
    for(int i = 0; i < finished.size(); ++i){
        cout << i + 1 << ":" << endl;
        if(i == finished.size() - 1){
            finished[i].decNumOfContextSwitching();
        }
        printInfo("PID: " + std::to_string(finished[i].getPID()), finished[i].getTimeOfCompletion(), finished[i].getWaitTime(), finished[i].getTurnaroundTime(), finished[i].getNumOfContextSwitching());
        averageCompletion += finished[i].getTimeOfCompletion();
        avgWait += finished[i].getWaitTime();
        avgTurn += finished[i].getTurnaroundTime();
        totalSwitch += finished[i].getNumOfContextSwitching();
    }
    printInfo("Average STATS of algorithm C: ", ceil(averageCompletion / finished.size()), ceil(avgWait / finished.size()), ceil(avgTurn / finished.size()), totalSwitch);
    cout << "--Algorithm C ends--"<<endl;
    exit(0);
}