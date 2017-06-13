#include <stdio.h>
#include<stdlib.h>
struct PCB{
	int pid;
	int priority;
	int arrival_time;
	int burst_time;
	int running_time;
	int waiting_time;
	int scheduling_check;
}Data[10];
struct Jobque{
	struct PCB *include;
}job[10];
struct Readyque{
	struct PCB *include;
	struct Readyque *next;
}*ready_FCRR=NULL,*last_FCRR,*ready_pree=NULL,*last_pree;
struct compare{
	float cpuUsage;
	float avgWait;
	float avgRespone;
	float avgTurn;
}comparray[3];
void FCFS();
int ready(int,int);
void RoundRobing(char *);
void preemitive(char*);
int preemitive_ready(int,int);
void preemitive_ready_sort();
void aging(char *);
void Set();
int schedule_count=0;
FILE *output;
int main(int argc,char *argv[])
{
	FILE *input;
	int i=0;
	double avg=0;
	input=fopen(argv[1],"r");
	output=fopen(argv[2],"w");
	if(input==NULL || output==NULL)
	{
		printf("File error\n");
		exit(1);
	}
	while(!feof(input))
	{
		fscanf(input,"%d %d %d %d",&Data[i].pid,&Data[i].priority,&Data[i].arrival_time,&Data[i].burst_time);
		job[i].include=&Data[i];
		i++;
	}
	fclose(input);
	Set();
	FCFS();
	ready_FCRR=NULL;
	last_FCRR=NULL;
	Set();
	RoundRobing(argv[3]);
	Set();
	preemitive(argv[4]);

	fprintf(output,"Avarage cpu usage -\n%f\n%f\n%f\n",comparray[0].cpuUsage,comparray[1].cpuUsage,comparray[2].cpuUsage);
	fprintf(output,"Avarage waiting time -\n%f\n%f\n%f\n",comparray[0].avgWait/10.0,comparray[1].avgWait/10.0,comparray[2].avgWait/10.0);
	fprintf(output,"Avarage response time -\n%f\n%f\n%f\n",comparray[0].avgRespone/10.0,comparray[1].avgRespone/10.0,comparray[2].avgRespone/10.);
	fprintf(output,"Avarage turnaround time -\n%f\n%f\n%f\n",comparray[0].avgTurn/10.0,comparray[1].avgTurn/10.0,comparray[2].avgTurn/10.0);
	fclose(output);
	return;
}
void FCFS()
{
	struct Readyque *ptr=NULL,*tmp;
	int i=0;
	int check=-1,cpuRunningCheck=0;
	comparray[0].avgRespone=0;
	comparray[0].avgTurn=0;
	comparray[0].avgWait=0;
	while(check)
	{
		check=ready(i,cpuRunningCheck);

		if(cpuRunningCheck==0&&check==1)//cpu no running
		{
			fprintf(output,"<time %d>",i);
			fprintf(output,"---- system is idle ----\n");
			comparray[0].cpuUsage++;
		}
		else if(check==2)//new process arrive
		{
			fprintf(output,"<time %d>",i);
			fprintf(output,"[new arrival] process %d\n",last_FCRR->include->pid);
			if(cpuRunningCheck==0)
				ptr=last_FCRR;
			cpuRunningCheck=1;
			continue;
		}
		else if(cpuRunningCheck==1)//cpu running
		{
			if(ptr->include->running_time==0){//waiting time 
				ptr->include->waiting_time=i-ptr->include->arrival_time;
				comparray[0].avgRespone+=i-ptr->include->waiting_time;
				comparray[0].avgWait+=ptr->include->waiting_time;
			}
			ptr->include->running_time++;

			if(ptr->include->running_time==ptr->include->burst_time)//process finish
			{
				fprintf(output,"<time %d>process %d is finished\n",i,ptr->include->pid);

				comparray[0].avgTurn+=ptr->include->waiting_time+ptr->include->burst_time;
				if(ptr->next==NULL)//ptr 다음이 없는데 ptr을 NULL 로 만들면 다음이 도착했을때 그것을 실행 불가능하기 때문에 디펜시브 코딩
				{
					ptr=NULL;
					cpuRunningCheck=0;
					continue;
				}
				tmp=ptr;
				ptr=ptr->next;
				free(tmp);
				fprintf(output,"-----------context switching-------------\n");
				i++;
				continue;

			}
			fprintf(output,"<time %d>",i);
			fprintf(output,"process %d running\n",ptr->include->pid);
		}
		i++;
	}
	i--;
	comparray[0].cpuUsage=(i-comparray[0].cpuUsage)/(float)i;
}

void RoundRobing(char *quantum)
{
	struct Readyque *ptr=NULL,*tmp;
	int i=0,timeQuantum=atoi(quantum);
	int check=-1,cpuRunningCheck=0;
	while(check)
	{

		check=ready(i,cpuRunningCheck);
		if(cpuRunningCheck==0&&check==1)//cpu no running
		{
			fprintf(output,"<time %d>",i);
			fprintf(output,"---- system is idle ----\n");
			comparray[1].cpuUsage++;
		}
		else if(check==2)//new process arrive
		{
			fprintf(output,"<time %d>",i);
			fprintf(output,"[new arrival] process %d\n",last_FCRR->include->pid);
			if(cpuRunningCheck==0)//queue가 비어있을때
				ptr=last_FCRR;
			cpuRunningCheck=1;
			continue;
		}
		else if(cpuRunningCheck==1)//cpu running
		{
			if(ptr->include->running_time==0)
				comparray[1].avgRespone+=i-ptr->include->arrival_time;
			ptr->include->running_time++;
			if(ptr->include->running_time==ptr->include->burst_time)//process finish
			{
				fprintf(output,"<time %d>process %d is finished\n",i,ptr->include->pid);
				ptr->include->waiting_time = i-ptr->include->burst_time - ptr->include->arrival_time;//waiting time is finish time - arrival -running;
				comparray[1].avgWait+=ptr->include->waiting_time;
				comparray[1].avgTurn+=ptr->include->waiting_time+ptr->include->burst_time;

				if(ptr->next==NULL)//ptr 다음이 없는데 ptr을 NULL 로 만들면 다음이 도착했을때 그것을 실행 불가능하기 때문에 디펜시브 코딩
				{
					cpuRunningCheck=0;
					continue;
				}
				tmp=ptr;
				ptr=ptr->next;
				free(tmp);
				fprintf(output,"-----------context switching-------------\n");
			}
			else if(((ptr->include->running_time)%(timeQuantum))==0)//timequantum
			{
				fprintf(output,"<time %d>",i);
				fprintf(output,"process %d running\n",ptr->include->pid);
				if(ptr->next==NULL)//처리한것이 마지막일경우 뒤로 넘기지 않는다
				{
					i++;
					continue;
				}
				fprintf(output,"-----------context switching-------------\n");
				//first process를 last 로 넘김
				tmp=ptr;
				ptr=ptr->next;
				last_FCRR->next=tmp;
				last_FCRR=tmp;
				last_FCRR->next=NULL;
				i++;
				continue;
			}
			fprintf(output,"<time %d>",i);
			fprintf(output,"process %d running\n",ptr->include->pid);
		}
		i++;

	}
	i--;
	comparray[1].cpuUsage=(i-comparray[1].cpuUsage)/(float)i;
}

int ready(int time,int check)
{
	struct Readyque *ptr;
	int i=0;
	while(i<10)
	{
		if(job[i].include->scheduling_check==0)//이미 프로세스가 스케쥴링 되었을경우 스케쥴링이 되어있지 않다면 스케쥴링 체크는 -1
		{
			i++;
			continue;
		}

		if(job[i].include->arrival_time==time)//스케쥴링
		{
			ptr=(struct Readyque*)malloc(sizeof(struct Readyque));
			ptr->include=job[i].include;
			ptr->next=NULL;
			if(last_FCRR==NULL)
			{
				ready_FCRR=ptr;
				last_FCRR=ptr;
			}
			else
			{
				last_FCRR->next=ptr;
				last_FCRR=ptr;
			}
			schedule_count++;
			ptr->include->scheduling_check=0;
			return 2;
		}
		i++;
	}
	if(schedule_count==10 &&check==0)//scheduling finish
		return 0;

	return 1;
}
void preemitive(char *alpha)
{
	struct Readyque *ptr=NULL;
	int i=0;
	int check=-1,cpuRunningCheck=0,runningPid=-1,processFinishCheck=0;
	while(check)
	{
		check=preemitive_ready(i,cpuRunningCheck);
		ptr=ready_pree;

		if(cpuRunningCheck==0&&check==1)//cpu no running
		{
			fprintf(output,"<time %d>",i);
			fprintf(output,"---- system is idle ----\n");
			comparray[2].cpuUsage++;
		}
		else if(check==2)//new process arrive
		{
			fprintf(output,"<time %d>",i);
			fprintf(output,"[new arrival] process %d\n",last_pree->include->pid);
			cpuRunningCheck=1;
			continue;
		}
		else if(cpuRunningCheck==1)//cpu running
		{
			if(ptr->include->running_time==0)
				comparray[2].avgRespone+=(i-ptr->include->arrival_time);
			if(runningPid==-1)
				runningPid=ptr->include->pid;
			else if(runningPid!=ptr->include->pid)
			{
				runningPid=ptr->include->pid;
				fprintf(output,"-----------context switching-------------\n");
			}

			ptr->include->running_time++;
			processFinishCheck=0;

			if(ptr->include->running_time==ptr->include->burst_time)//process finish
			{
				fprintf(output,"<time %d>process %d is finished\n",i,ptr->include->pid);
				comparray[2].avgTurn+=(ptr->include->burst_time+ptr->include->waiting_time);
				comparray[2].avgWait+=ptr->include->waiting_time;
				if(ready_pree->next==NULL)//실행한프로그램이 마지막일경우
				{
					cpuRunningCheck=0;
					i++;
					ready_pree=ready_pree->next;
					continue;
				}
				ptr=ready_pree;
				ready_pree=ready_pree->next;
				free(ptr);
				i++;
				continue;
			}
			fprintf(output,"<time %d>",i);
			fprintf(output,"process %d running\n",ptr->include->pid);

		}
		aging(alpha);
		i++;
	}
	i--;
	comparray[2].cpuUsage=(i-comparray[2].cpuUsage)/(float)i;
}
int preemitive_ready(int time,int check)
{
	struct Readyque *ptr,*trv_ptr,*trv_pre_ptr;
	int i=0;
	static int count=0 ,repet_check=-1;
	trv_ptr=ready_pree;
	trv_pre_ptr=ready_pree;
	if(check==0 && count==10)
		return 0;

	if(repet_check!=time)//현재 시간이 반복된 시간이면 sort 와 aging 을 할필요가 없다
	{
		preemitive_ready_sort();
		repet_check=time;
	}
	while(i<10)
	{
		if(job[i].include->scheduling_check==0)//이미 프로세스가 스케쥴링 되었을경우 또는 이미 모두 스케쥴링 된 경우 스케쥴링 pass
		{
			i++;
			continue;
		}

		if(job[i].include->arrival_time==time)
		{
			ptr=(struct Readyque*)malloc(sizeof(struct Readyque));
			ptr->include=job[i].include;
			ptr->next=NULL;
			ptr->include->scheduling_check=0;
			count++;
			if(ready_pree==NULL)
			{
				ready_pree=ptr;
				last_pree=ptr;
				return 2;
			}
			else
			{
				while(trv_ptr)
				{

					if(trv_ptr==last_pree)
					{
						last_pree->next=ptr;
						last_pree=ptr;
						return 2;
					}

					if(ptr->include->priority > trv_ptr->include->priority )
					{

						if(trv_ptr==ready_pree)
						{
							ptr->next=trv_ptr;
							ready_pree=ptr;
						}
						else
						{
							trv_pre_ptr->next=ptr;
							ptr->next=trv_ptr;
						}
						return 2;
					}


					trv_pre_ptr=trv_ptr;
					trv_ptr=trv_ptr->next;
				}
			}

		}
		i++;
	}
	return 1;
}
void preemitive_ready_sort()
{
	struct Readyque *ptr,*tmp,*pre_ptr;
	int sort_finish=1;

	while(sort_finish){
		ptr=ready_pree;
		pre_ptr=ptr;
		sort_finish=0;
		while(ptr)
		{
			if(ptr->next==NULL)
				break;

			if(ptr->include->priority < ptr->next->include->priority)
			{
				sort_finish++;
				if(ptr==ready_pree)
				{
					tmp=ptr;
					ptr=ptr->next;
					tmp->next=ptr->next;
					ptr->next=tmp;
					ready_pree=ptr;
					break;
				}

				if(ptr->next==last_pree)
				{
					pre_ptr->next=last_pree;
					last_pree->next=ptr;
					last_pree=ptr;
					ptr->next=NULL;
					break;
				}
				tmp=ptr;
				pre_ptr->next=ptr->next;
				ptr=ptr->next;
				tmp->next=ptr->next;
				ptr->next=tmp;
				break;

			}
			pre_ptr=ptr;
			ptr=ptr->next;
		}
	}
}
void aging(char *alpha)
{
	float alp=atof(alpha);
	struct Readyque *ptr=ready_pree;
	if(ready_pree==NULL)
		return;
	ptr=ptr->next;
	while(ptr)
	{
		ptr->include->waiting_time++;
		ptr->include->priority +=(ptr->include->waiting_time*alp);
		ptr=ptr->next;
	}
}
void Set()
{
	int i;
	for(i=0;i<10;i++)
	{
		Data[i].running_time=0;
		Data[i].scheduling_check=-1;
		Data[i].waiting_time=0;
	}
	schedule_count=0;
}