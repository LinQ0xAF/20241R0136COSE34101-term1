#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 100

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

Process readyQueue[MAX_PROCESSES];
Process waitingQueue[MAX_PROCESSES];
int readyCount = 0;
int waitingCount = 0;

void Create_Process(Process processes[], int n) {
    // 시드 초기화
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
        printf("준비 큐가 가득 찼습니다.\n");
    }
}

// 대기 큐에 프로세스를 추가하는 함수
void enqueueWaiting(Process p) {
    if (waitingCount < MAX_PROCESSES) {
        waitingQueue[waitingCount++] = p;
    } 
    else {
        printf("대기 큐가 가득 찼습니다.\n");
    }
}

// 시스템 환경을 설정하는 함수
void Config(Process processes[], int n) {
    readyCount = 0;
    waitingCount = 0;
    
    for (int i = 0; i < n; i++) {
        // 도착 시간에 따라 준비 큐에 추가
        if (processes[i].arrival <= 10) {
            enqueueReady(processes[i]);
        } 
        else {
            enqueueWaiting(processes[i]);
        }
    }
}

void FCFS(Process processes[], int n) {
    int current = 0;
    for (int i = 0; i < n; i++) {
        if (current < processes[i].arrival) {
            current = processes[i].arrival;
        }
        processes[i].startTime = current;
        processes[i].endTime = current + processes[i].burstTime;
        processes[i].waitingTime = current - processes[i].arrival;
        processes[i].turnAroundTime = processes[i].waitingTime + processes[i].burstTime;
        current += processes[i].burstTime;
    }
}

void SJF(Process processes[], int n) {
    int completed = 0, current = 0;
    int minIndex;
    while (completed != n) {
        minIndex = -1;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= current && (minIndex == -1 || processes[i].burstTime < processes[minIndex].burstTime) && processes[i].turnAroundTime == 0) {
                minIndex = i;
            }
        }
        if (minIndex != -1) {
            processes[minIndex].startTime = current;
            current += processes[minIndex].burstTime;
            processes[minIndex].waitingTime = current - processes[minIndex].arrival - processes[minIndex].burstTime;
            processes[minIndex].turnAroundTime = processes[minIndex].waitingTime + processes[minIndex].burstTime;
            processes[minIndex].endTime = current;
            completed++;
        } else {
            current++;
        }
    }
}

void SJF_p(Process processes[], int n) {
    int completed = 0, current = 0, minIndex;
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
            current++;
            processes[minIndex].remainingTime--;
            if (processes[minIndex].remainingTime == 0) {
                processes[minIndex].endTime = current;
                processes[minIndex].waitingTime = processes[minIndex].endTime - processes[minIndex].arrival - processes[minIndex].burstTime;
                processes[minIndex].turnAroundTime = processes[minIndex].endTime - processes[minIndex].arrival;
                completed++;
            }
        } else {
            current++;
        }
    }
}

void Priority(Process processes[], int n) {
    int completed = 0, current = 0;
    int minIndex;
    while (completed != n) {
        minIndex = -1;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= current && (minIndex == -1 || processes[i].priority < processes[minIndex].priority) && processes[i].turnAroundTime == 0) {
                minIndex = i;
            }
        }
        if (minIndex != -1) {
            processes[minIndex].startTime = current;
            current += processes[minIndex].burstTime;
            processes[minIndex].waitingTime = current - processes[minIndex].arrival - processes[minIndex].burstTime;
            processes[minIndex].turnAroundTime = processes[minIndex].waitingTime + processes[minIndex].burstTime;
            processes[minIndex].endTime = current;
            completed++;
        } else {
            current++;
        }
    }
}

void Priority_p(Process processes[], int n) {
    int completed = 0, current = 0, minIndex;
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
            current++;
            processes[minIndex].remainingTime--;
            if (processes[minIndex].remainingTime == 0) {
                processes[minIndex].endTime = current;
                processes[minIndex].waitingTime = processes[minIndex].endTime - processes[minIndex].arrival - processes[minIndex].burstTime;
                processes[minIndex].turnAroundTime = processes[minIndex].endTime - processes[minIndex].arrival;
                completed++;
            }
        } else {
            current++;
        }
    }
}

void RR(Process processes[], int n, int quantum) {
    int current = 0;
    int remainingBurstTime[n];
    for (int i = 0; i < n; i++) {
        remainingBurstTime[i] = processes[i].burstTime;
    }
    while (1) {
        int done = 1;
        for (int i = 0; i < n; i++) {
            if (remainingBurstTime[i] > 0) {
                done = 0;
                if (remainingBurstTime[i] == processes[i].burstTime) {
                    processes[i].startTime = current;
                }
                if (remainingBurstTime[i] > quantum) {
                    current += quantum;
                    remainingBurstTime[i] -= quantum;
                } else {
                    current += remainingBurstTime[i];
                    processes[i].waitingTime = current - processes[i].arrival - processes[i].burstTime;
                    processes[i].turnAroundTime = processes[i].waitingTime + processes[i].burstTime;
                    remainingBurstTime[i] = 0;
                    processes[i].endTime = current;
                }
            }
        }
        if (done == 1)
            break;
    }
}

// Gantt 차트를 출력하는 함수
void printGanttChart(Process processes[], int n) {
    printf("Gantt Chart:\n|");
    for (int i = 0; i < n; i++) {
        printf(" P%d |", processes[i].pid);
    }
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%d    ", processes[i].startTime);
    }
    printf("%d\n", processes[n-1].endTime);
}

// 각 스케줄링 알고리즘의 평균 대기 시간과 평균 턴어라운드 시간을 계산하는 함수
void Evaluation(Process processes[], int n) {
    Process processesCopy[n];
    int totalWaitingTime, totalTurnAroundTime;
    float averageWaitingTime, averageTurnAroundTime;
    int quantum = 4;  // Round Robin 타임 퀀텀

    // FCFS
    for (int i = 0; i < n; i++) processesCopy[i] = processes[i];
    FCFS(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    averageWaitingTime = (float)totalWaitingTime / n;
    averageTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("FCFS - 평균 대기 시간: %.2f, 평균 턴어라운드 시간: %.2f\n", averageWaitingTime, averageTurnAroundTime);
    printGanttChart(processesCopy, n);

    // SJN
    for (int i = 0; i < n; i++) processesCopy[i] = processes[i];
    SJF(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    averageWaitingTime = (float)totalWaitingTime / n;
    averageTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("SJN - 평균 대기 시간: %.2f, 평균 턴어라운드 시간: %.2f\n", averageWaitingTime, averageTurnAroundTime);
    printGanttChart(processesCopy, n);

    // 선점형 SJN (SRTF)
    for (int i = 0; i < n; i++) processesCopy[i] = processes[i];
    SJF_p(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    averageWaitingTime = (float)totalWaitingTime / n;
    averageTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("SRTF - 평균 대기 시간: %.2f, 평균 턴어라운드 시간: %.2f\n", averageWaitingTime, averageTurnAroundTime);
    printGanttChart(processesCopy, n);

    // 비선점형 Priority Scheduling
    for (int i = 0; i < n; i++) processesCopy[i] = processes[i];
    Priority(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    averageWaitingTime = (float)totalWaitingTime / n;
    averageTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("Priority (Non-Preemptive) - 평균 대기 시간: %.2f, 평균 턴어라운드 시간: %.2f\n", averageWaitingTime, averageTurnAroundTime);
    printGanttChart(processesCopy, n);

    // 선점형 Priority Scheduling
    for (int i = 0; i < n; i++) processesCopy[i] = processes[i];
    Priority_p(processesCopy, n);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    averageWaitingTime = (float)totalWaitingTime / n;
    averageTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("Preemptive Priority - 평균 대기 시간: %.2f, 평균 턴어라운드 시간: %.2f\n", averageWaitingTime, averageTurnAroundTime);
    printGanttChart(processesCopy, n);

    // Round Robin
    for (int i = 0; i < n; i++) processesCopy[i] = processes[i];
    RR(processesCopy, n, quantum);
    totalWaitingTime = 0;
    totalTurnAroundTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += processesCopy[i].waitingTime;
        totalTurnAroundTime += processesCopy[i].turnAroundTime;
    }
    averageWaitingTime = (float)totalWaitingTime / n;
    averageTurnAroundTime = (float)totalTurnAroundTime / n;
    printf("Round Robin - 평균 대기 시간: %.2f, 평균 턴어라운드 시간: %.2f\n", averageWaitingTime, averageTurnAroundTime);
    printGanttChart(processesCopy, n);
}

int main() {
    int n, choice, quantum;
    printf("프로세스 수를 입력하세요: ");
    scanf("%d", &n);

    Process processes[n];
    Create_Process(processes, n);

    printf("PID\tAT\tBT\tIOT\tP\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival, processes[i].burstTime, processes[i].ioBurstTime, processes[i].priority);
    }

    Config(processes, n);
    Evaluation(processes, n);
    
    return 0;
}

