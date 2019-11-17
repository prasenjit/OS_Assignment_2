#include <stdio.h>

typedef struct
{
	int id;
	int priority;
	int arrival_time;
	int io_count, cpu_count;
	int cpu_burst[20];
	int io_burst[20];
} JobDB;

int count = 0;

int getSortKey(JobDB temp, int index)
{
	if (index == 1)
	{
		return -temp.priority;
	}
	else if (index == 2)
	{
		return temp.arrival_time;
	}
}

void sortJobs(JobDB store[], int value, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n - 1 - i; j++)
		{
			if (getSortKey(store[j], value) > getSortKey(store[j + 1], value))
			{
				JobDB temp = store[j];
				store[j] = store[j + 1];
				store[j + 1] = temp;
			}
		}
	}
}

void readJobs(JobDB sample[20])
{
	printf("Trying to read jobs...\n");
	FILE *f = fopen("JobProfiles.txt", "r");

	int id, priority, arrival_time;
	while (f)
	{
		int no = fscanf(f, "%d %d %d", &id, &priority, &arrival_time);
		if (id == -1)
			break;
		sample[count].id = id;
		sample[count].priority = priority;
		sample[count].arrival_time = arrival_time;
		sample[count].io_count = 0;
		sample[count].cpu_count = 0;
		int burstcount = 0;
		do
		{
			int tmpvalue;
			fscanf(f, "%d", &tmpvalue);
			if (tmpvalue == -1)
				break;
			if (burstcount % 2 == 0)
			{
				sample[count].cpu_burst[burstcount / 2] = tmpvalue;
				sample[count].cpu_count++;
			}
			else
			{
				sample[count].io_burst[burstcount / 2] = tmpvalue;
				sample[count].io_count++;
			}
			burstcount++;
		} while (1);
		// fscanf(f,"\n");
		count++;
	}
	for (int i = 0; i < count; i++)
	{
		printf("JobId: %d    Priority: %d    ArrivalTime(s): %d\n", sample[i].id,
				 sample[i].priority, sample[i].arrival_time);
	}
	printf("=======\n");
}

void FCFS(JobDB sample[20])
{
	printf("FCFS:=\n");
	sortJobs(sample, 2, count);
	int timer = sample[0].arrival_time;
	int waitingTime = 0, turnaroundTime = 0;
	// int totalCPU=0,totalIO;
	for (int i = 0; i < count; i++)
	{
		int totalCPU = 0;
		for (int j = 0; j < sample[i].cpu_count; j++)
		{
			totalCPU += sample[i].cpu_burst[j];
		}
		int totalIO = 0;
		for (int j = 0; j < sample[i].io_count; j++)
		{
			totalIO += sample[i].io_burst[j];
		}
		waitingTime += timer - sample[i].arrival_time;
		timer += totalIO + totalCPU;
		turnaroundTime += timer - sample[i].arrival_time;
	}
	float avg_wait = waitingTime / (float)count;
	float avg_turnaround = turnaroundTime / (float)count;
	printf("Avg Waiting Time for %d processes: %f\n", count, avg_wait);
	printf("Avg Turnaround Time for %d processes: %f\n", count, avg_turnaround);
	printf("======\n");
}

void NonPreemptive(JobDB sample[20])
{
	printf("NonPreemptive:=\n");
	sortJobs(sample, 2, count);
	int timer = sample[0].arrival_time;
	int waitingTime = 0, turnaroundTime = 0;

	for (int i = 0; i < count; i++)
	{
		int totalCPU = 0;
		for (int j = 0; j < sample[i].cpu_count; j++)
		{
			totalCPU += sample[i].cpu_burst[j];
		}
		int totalIO = 0;
		for (int j = 0; j < sample[i].io_count; j++)
		{
			totalIO += sample[i].io_burst[j];
		}
		waitingTime += timer - sample[i].arrival_time;
		timer += totalIO + totalCPU;
		turnaroundTime += timer - sample[i].arrival_time;
	}
	float avg_wait = waitingTime / (float)count;
	float avg_turnaround = turnaroundTime / (float)count;
	printf("Avg Waiting Time for %d processes: %f\n", count, avg_wait);
	printf("Avg Turnaround Time for %d processes: %f\n", count, avg_turnaround);
	printf("======\n");
}

int queue[20];
int front, rear;
int front = -1;
int rear = -1;

void enqueue(int a)
{
	queue[(rear + 1) % count] = a;
	if (front == -1)
		front = 0;
	rear = (rear + 1) % count;
}

int dequeue()
{
	if (front == -1)
		return -1;
	int a = queue[front];
	if (front == rear)
		front = rear = -1;
	else
		front = (front + 1) % count;
	return a;
}

#define timeSlice 4
void RoundRobin(JobDB sample[20])
{
	printf("RoundRobin:=\n");
	sortJobs(sample, 2, count);
	int waitT[20];
	int turnAT[20];
	int burstT[20];

	for (int i = 0; i < 20; ++i) {
		waitT[i] = turnAT[i] = burstT[i] = 0;
	}

	for (int i = 0; i < count; i++)
	{
		int totalCPU = 0;
		for (int j = 0; j < sample[i].cpu_count; j++)
		{
			totalCPU += sample[i].cpu_burst[j];
		}
		int totalIO = 0;
		for (int j = 0; j < sample[i].io_count; j++)
		{
			totalIO += sample[i].io_burst[j];
		}
		burstT[i] = totalCPU + totalIO;
	}

	int timer = sample[0].arrival_time - 1;
	int p = -1;
	int inQ = 0;
	while (1) {
		++timer;
		for (int i = 0; i < count; ++i) {
			if (sample[i].arrival_time == timer) {
				enqueue(i);
				++inQ;
			}
		}
		p = (p == -1) ? dequeue() : p;
		
		burstT[p]--;
		if (burstT[p] == 0 || timer % timeSlice == 0) {
			int temp = p;
			p = dequeue();
			// printf("Process: %d\n", p);
			--inQ;
			if (burstT[temp]) {
				enqueue(temp);
				++inQ;
			} else {
				turnAT[temp] = timer;
				printf("Process %d finished at %d\n", temp, timer);
			}
		}

		for (int i = 0; i < inQ; ++i) {
			int k = dequeue();
			waitT[k]++;
			enqueue(k);
		}
		if (inQ == 0) {
			break;
		}
	}

	float avgWait = 0, avgTurn = 0;
	for (int i = 0; i < count; i++)
	{
		printf("Process: %d     Wait: %d    Turnaround: %d\n", i, waitT[i], turnAT[i]);
		avgWait += waitT[i];
		avgTurn += turnAT[i];
	}
	avgWait /= count;
	avgTurn /= count;
	printf("Avg Waiting Time for %d processes: %f\n", count, avgWait);
	printf("Avg Turnaround Time for %d processes: %f\n", count, avgTurn);
	printf("======\n");
}


int main()
{
	JobDB sample[20];
	readJobs(sample);
	FCFS(sample);
	NonPreemptive(sample);
	RoundRobin(sample);
}