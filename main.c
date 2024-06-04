#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PROCESSES 100
#define MAX_LOG_ENTRIES 1000

typedef struct {
    int pid;
    int arrival;
    int burstTime;
    int ioBurstTime;
    int priority;
    int remainingTime;
    int waitingTime;
    int turnAroundTime;
    int startTime;
    int endTime;
} Process;

typedef struct {
    int pid;
    int startTime;
    int endTime;
} GanttLog;

Process readyQueue[MAX_PROCESSES];
Process waitingQueue[MAX_PROCESSES];
GanttLog ganttlog[MAX_LOG_ENTRIES];
Process idle;
int readyCount = 0;
int waitingCount = 0;
int logCount = 0;

void Create_Process(Process processes[], int n) {
    srand(time(NULL));
    
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        processes[i].arrival = rand() % 20;  // 도착 시간: 0 ~ 19
        processes[i].burstTime = rand() % 10 + 1; // CPU 실행 시간: 1 ~ 10
        processes[i].ioBurstTime = rand() % 5 + 1; // I/O 실행 시간: 1 ~ 5
        processes[i].priority = rand() % 10 + 1; // 우선순위: 1 ~ 10
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].waitingTime = 0;
        processes[i].turnAroundTime = 0;
        processes[i].startTime = -1;
        processes[i].endTime = 0;
    }
}

// 준비 큐에 프로세스를 추가하는 함수
void enqueueReady(Process p) {
    if (readyCount < MAX_PROCESSES) {
        readyQueue[readyCount++] = p;
    } 
    else {
        printf("Ready queue is full.\n");
    }
}

// 대기 큐에 프로세스를 추가하는 함수
void enqueueWaiting(Process p) {
    if (waitingCount < MAX_PROCESSES) {
        waitingQueue[waitingCount++] = p;
    } 
    else {
        printf("Waiting queue is full.\n");
    }
}

// 시스템 환경을 설정하는 함수
void Config(Process processes[], int n) {
    readyCount = 0;
    waitingCount = 0;
    int fc;         //먼저 온 process index
    
    for (int i = 0; i < n; i++) {           
        enqueueReady(processes[i]);
    }

    for (int i = 0; i < n-1; i++){
        fc = i;
        for(int j = i+1; j < n; j++){
            if(readyQueue[j].arrival < readyQueue[fc].arrival){
                fc = j;
            }
        }
        Process temp = readyQueue[i];
        readyQueue[i] = readyQueue[fc];
        readyQueue[fc] = temp;
    }

}

void clearGanttLog() {
    logCount = 0;
}

void addGanttLog(int pid, int startTime, int endTime){
    if(logCount < MAX_LOG_ENTRIES){
        ganttlog[logCount].pid = pid;
        ganttlog[logCount].startTime = startTime;
        ganttlog[logCount].endTime = endTime;
        logCount++;
    }
    else {
        printf("Gantt Chart Log is full.\n");
    }
}

// Gantt 차트 출력
void printGanttChart() {
    printf("Gantt Chart:\n|");
    for (int i = 0; i < logCount; i++) {
        printf(" P%d |", ganttlog[i].pid);
    }
    printf("\n");

    for (int i = 0; i < logCount; i++) {
        printf("%d    ", ganttlog[i].startTime);
    }
    printf("%d\n", ganttlog[logCount-1].endTime);
}

void FCFS(Process processes[], int n) {
    clearGanttLog();
    int current = 0;
    for (int i = 0; i < n; i++) {
        if (current < processes[i].arrival) {
            addGanttLog(idle.pid, current, processes[i].arrival);
            current = processes[i].arrival;
        }
        processes[i].startTime = current;
        processes[i].endTime = current + processes[i].burstTime;
        processes[i].waitingTime = current - processes[i].arrival;
        processes[i].turnAroundTime = processes[i].waitingTime + processes[i].burstTime;
        current += processes[i].burstTime;
        addGanttLog(processes[i].pid, processes[i].startTime, processes[i].endTime);
    }
}

void SJF(Process processes[], int n) {
    clearGanttLog();
    int completed = 0, current = 0;
    int minIndex, minBurst;

    while (completed != n) {
        minIndex = -1;
        minBurst = 99999;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= current && processes[i].remainingTime > 0) {
                if(processes[i].burstTime < minBurst){
                    minBurst = processes[i].burstTime;
                    minIndex = i;
                }
            }
        }

        if (minIndex != -1) {
            if(processes[minIndex].startTime == -1){
                processes[minIndex].startTime = current;
            }
            current += processes[minIndex].burstTime;
            processes[minIndex].remainingTime = 0;
            processes[minIndex].endTime = current;
            processes[minIndex].waitingTime = processes[minIndex].startTime - processes[minIndex].arrival;
            processes[minIndex].turnAroundTime = processes[minIndex].endTime + processes[minIndex].arrival;
            completed++;
            addGanttLog(processes[minIndex].pid, processes[minIndex].startTime, processes[minIndex].endTime);
        } else {
            current++;
            if(logCount == 0 || ganttlog[logCount-1].pid != 0){
                addGanttLog(idle.pid, current - 1, current);
            }
            else{
                ganttlog[logCount-1].endTime = current;
            }
        }
    }
}

void SJF_p(Process processes[], int n) {
    clearGanttLog();
    int completed = 0, current = 0, minIndex;
    int prev = -1;
    while (completed != n) {
        minIndex = -1;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= current && processes[i].remainingTime > 0 &&
                (minIndex == -1 || processes[i].remainingTime < processes[minIndex].remainingTime)) {
                minIndex = i;
            }
        }
        if (minIndex != -1) {
            if (processes[minIndex].startTime == -1) {
                processes[minIndex].startTime = current;
            }
            if(minIndex != prev && prev != -1){
                if(processes[prev].remainingTime != 0){
                    addGanttLog(processes[prev].pid, processes[prev].startTime, current);
                }
                processes[minIndex].startTime = current;
            }
            current++;
            processes[minIndex].remainingTime--;
            if (processes[minIndex].remainingTime == 0) {
                processes[minIndex].endTime = current;
                processes[minIndex].waitingTime = processes[minIndex].endTime - processes[minIndex].arrival - processes[minIndex].burstTime;
                processes[minIndex].turnAroundTime = processes[minIndex].endTime - processes[minIndex].arrival;
                completed++;
                addGanttLog(processes[minIndex].pid, processes[minIndex].startTime, current);
            }
        } else {
            current++;
            if(logCount == 0 || ganttlog[logCount-1].pid != 0){
                addGanttLog(idle.pid, current - 1, current);
            }
            else{
                ganttlog[logCount-1].endTime = current;
            }
        }
        prev = minIndex;
    }
}

void Priority(Process processes[], int n) {
    clearGanttLog();
    int completed = 0, current = 0;
    int minIndex;
    while (completed != n) {
        minIndex = -1;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= current && processes[i].remainingTime > 0 && (minIndex == -1 || processes[i].priority < processes[minIndex].priority)) {
                minIndex = i;
            }
        }
        if (minIndex != -1) {
            if (processes[minIndex].startTime == -1){
                processes[minIndex].startTime = current;
            }
            current += processes[minIndex].burstTime;
            processes[minIndex].remainingTime = 0;
            processes[minIndex].endTime = current;
            processes[minIndex].waitingTime = processes[minIndex].startTime - processes[minIndex].arrival;
            processes[minIndex].turnAroundTime = processes[minIndex].endTime - processes[minIndex].arrival;
            completed++;
            addGanttLog(processes[minIndex].pid, processes[minIndex].startTime, processes[minIndex].endTime);
        } else {
            current++;
            if(logCount == 0 || ganttlog[logCount-1].pid != 0){
                addGanttLog(idle.pid, current - 1, current);
            }
            else{
                ganttlog[logCount-1].endTime = current;
            }
        }
    }
}

void Priority_p(Process processes[], int n) {
    clearGanttLog();
    int completed = 0, current = 0;
    int minIndex;
    int prev = -1;

    while (completed != n) {
        minIndex = -1;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= current && processes[i].remainingTime > 0 &&
                (minIndex == -1 || processes[i].priority < processes[minIndex].priority)) {
                minIndex = i;
            }
        }
        if (minIndex != -1) {
            if (processes[minIndex].startTime == -1) {
                processes[minIndex].startTime = current;
            }
            if(minIndex != prev && prev != -1){
                if(processes[prev].remainingTime != 0){
                    addGanttLog(processes[prev].pid, processes[prev].startTime, current);
                }
                processes[minIndex].startTime = current;
            }
            current++;
            processes[minIndex].remainingTime--;
            if (processes[minIndex].remainingTime == 0) {           //process 완료 시
                processes[minIndex].endTime = current;
                processes[minIndex].waitingTime = processes[minIndex].endTime - processes[minIndex].arrival - processes[minIndex].burstTime;
                processes[minIndex].turnAroundTime = processes[minIndex].endTime - processes[minIndex].arrival;
                completed++;
                addGanttLog(processes[minIndex].pid, processes[minIndex].startTime, current);
            }
            
        } 
        else {
            current++;      //아무 process도 실행되지 않음
            if(logCount == 0 || ganttlog[logCount-1].pid != 0){
                addGanttLog(idle.pid, current - 1, current);
            }
            else{
                ganttlog[logCount-1].endTime = current;
            }
        }
        prev = minIndex;    //직전 process 확인용
    }
}

void RR(Process processes[], int n, int quantum) {
    clearGanttLog();
    int current = 0;
    int completed = 0;
    int emptycheck;

    while (completed != n) {
        emptycheck = 0;
        for (int i = 0; i < n; i++) {
            if (processes[i].remainingTime > 0 && processes[i].arrival <= current) {
                if (processes[i].remainingTime == processes[i].burstTime) {                 //process 최초 실행 시
                    processes[i].startTime = current;
                }

                if (processes[i].remainingTime > quantum) {
                    current += quantum;
                    processes[i].remainingTime -= quantum;
                    addGanttLog(processes[i].pid, current - quantum, current);
                } else {
                    current += processes[i].remainingTime;
                    addGanttLog(processes[i].pid, current - processes[i].remainingTime, current);
                    processes[i].waitingTime = current - processes[i].arrival - processes[i].burstTime;
                    processes[i].turnAroundTime = current - processes[i].arrival;
                    processes[i].remainingTime = 0;
                    processes[i].endTime = current;
                    completed++;
                }
            }
            else{
                emptycheck++;
            }
        }
        if(emptycheck == n){
            current++;
            if(logCount == 0 || ganttlog[logCount-1].pid != 0){
                addGanttLog(idle.pid, current - 1, current);
            }
            else{
                ganttlog[logCount-1].endTime = current;
            }
        }
    }
}

// 각 scheduling 알고리즘의 평균 waiting time과 평균 turnaround time을 계산하는 함수
void Evaluation(Process processes[], int n) {
    Process processesCopy[n];
    int totalWaitingTime, totalTurnAroundTime;
    float avgWaitingTime, avgTurnAroundTime;
    int quantum = 4;  // Round Robin time quantum
    idle.pid = 0;
    idle.startTime = -1;
    idle.endTime = 0;

    // FCFS
    for (int i = 0; i < n; i++) processesCopy[i] = readyQueue[i];
    FCFS(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    avgWaitingTime = (float)totalWaitingTime / n;
    avgTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("FCFS - average Waiting Time: %.2f, average Turnaround Time: %.2f\n", avgWaitingTime, avgTurnAroundTime);
    printGanttChart();

    // SJF
    for (int i = 0; i < n; i++) processesCopy[i] = readyQueue[i];
    SJF(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    avgWaitingTime = (float)totalWaitingTime / n;
    avgTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("SJF - average Waiting Time: %.2f, average Turnaround Time: %.2f\n", avgWaitingTime, avgTurnAroundTime);
    printGanttChart();

    // preemptive SJF 
    for (int i = 0; i < n; i++) processesCopy[i] = readyQueue[i];
    SJF_p(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    avgWaitingTime = (float)totalWaitingTime / n;
    avgTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("Preemptive SJF - average Waiting Time: %.2f, average Turnaround Time: %.2f\n", avgWaitingTime, avgTurnAroundTime);
    printGanttChart();

    // non-preemptive Priority Scheduling
    for (int i = 0; i < n; i++) processesCopy[i] = readyQueue[i];
    Priority(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    avgWaitingTime = (float)totalWaitingTime / n;
    avgTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("Non-Preemptive Priority - average Waiting Time: %.2f, average Turnaround Time: %.2f\n", avgWaitingTime, avgTurnAroundTime);
    printGanttChart();

    // preemptive Priority Scheduling
    for (int i = 0; i < n; i++) processesCopy[i] = readyQueue[i];
    Priority_p(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    avgWaitingTime = (float)totalWaitingTime / n;
    avgTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("Preemptive Priority - average Waiting Time: %.2f, average Turnaround Time: %.2f\n", avgWaitingTime, avgTurnAroundTime);
    printGanttChart();

    // Round Robin
    for (int i = 0; i < n; i++) processesCopy[i] = readyQueue[i];
    RR(processesCopy, n, quantum);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    avgWaitingTime = (float)totalWaitingTime / n;
    avgTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("Round Robin - average Waiting Time: %.2f, average Turnaround Time: %.2f\n", avgWaitingTime, avgTurnAroundTime);
    printGanttChart();
}

int main() {
    int n, choice, quantum;
    printf("Enter the number of Processes: ");
    scanf("%d", &n);

    Process processes[n];
    Create_Process(processes, n);

    printf("PID\tA_T\tB_T\tIO_T\tPr\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival, processes[i].burstTime, processes[i].ioBurstTime, processes[i].priority);
    }

    Config(processes, n);
    Evaluation(processes, n);
    
    return 0;
}

