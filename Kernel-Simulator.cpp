//============================================================================
// Name        : Os.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <queue>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include<time.h>
#include<cmath>

using namespace std;

volatile sig_atomic_t flag = true;
int id = 1;
time_t start_timing=time(NULL);



struct CPU
{
	int number_of_instructions;
	string* instruction_set;
	int value;       //changes according to the insstructon is a part of instruction
	int pc;

};


class Process {

public:
	const int pid;
	 int ppid;
	time_t arrival_time;	// arrival time
	double CPU_TIME;	// cpu time used so far burst time
	double turn_around_time;
	string state;
	CPU obj;

	Process ():pid(id++)
	{
		obj.number_of_instructions=0;
		arrival_time=time(NULL);
		obj.pc = CPU_TIME = obj.value = 0;
		state = "";
		obj.instruction_set = NULL;
		turn_around_time=0;
		ppid=0;
	}

	Process (const int nid,const int& pcounter,
			const int& burst_t,const int& v,const string s,
			string* inst,int no):pid(id++),ppid(nid){

		arrival_time=time(NULL);
		obj.pc = pcounter;
		CPU_TIME = 0;
		obj.value = 0;
		state = "";
		obj.instruction_set = inst;
		obj.number_of_instructions=no-pcounter;
		//ppid=
	}

	void free_all()
	{
		delete [] (obj.instruction_set);
	}

	void print_itself()
	{
		 char buffer[26];
		    struct tm* tm_info;


		    tm_info = localtime(&arrival_time);

		cout<<"pid="<<pid<<", ppid="<<ppid<<", value="<<obj.value<<", start time=";
		strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
		 puts(buffer);cout
		 <<", CPU time used so far="<<CPU_TIME<<" micro-seconds"<<endl;
	}


};

int stoi(string s)
{

	int len=0;
	for(int i=0;s[i]!='\0';++i)
	{
		len++;
	}
	int num=0;
	for(int i=0;s[i]!='\0';++i)
	{
		num+=(s[i]-48)*pow(10,len-1);
		len--;
	}

	return num;

}

queue <Process> ready;
queue <Process> running;
queue <Process> block;
queue <double> turn_around_time;
queue<int> execution_record;
int execution_record_entries=0;
int time_slice=0;
int fd[2];

void context_switching()
{
	time_slice=0;
	ready.push(running.front());
	running.pop();
	running.push(ready.front());
	ready.pop();
	execution_record.push( ( ( running.front() ).pid) );
	execution_record_entries+=1;

}

void Execute_instruction(Process*p)
{
	string to_execute="";
	timeval stop, start;
	gettimeofday(&start, NULL);
			  double final=0;
	if((p->obj).pc==(p->obj).number_of_instructions)
	{
		 time_t ttime;
		 double final;
    	 ttime = time(NULL);
    	 final= difftime(ttime,p->arrival_time);
		 running.pop();
		 turn_around_time.push(final);
		 return;
	}
	else
	{


		for(int i=0;(p->obj).instruction_set[(p->obj).pc][i]!='\0';++i)
		{
			to_execute+=(p->obj).instruction_set[(p->obj).pc][i];
		}
		(p->obj).pc+=1;


		int traverse;

		for(traverse=0;to_execute [traverse]==' ';++traverse);

		if(to_execute[traverse]=='S')
		{
			traverse+=1;

			for(;to_execute [traverse]==' ';++traverse);

			string num="";

			for(;to_execute[traverse]!=' ' && to_execute[traverse]!='\n' && to_execute[traverse]!='\0';++traverse)
			{
				num+=to_execute[traverse];
			}

			int no=stoi(num);
			(p->obj).value=no;


		}
		else if(to_execute[traverse]=='A')
		{

			traverse+=1;

			for(;to_execute [traverse]==' ';++traverse);

			string num="";

			for(;to_execute[traverse]!=' '&& to_execute[traverse]!='\n'&& to_execute[traverse]!='\0';++traverse)
			{
				num+=to_execute[traverse];
			}

			int no=stoi(num);

			(p->obj).value+=no;
		}
		else if(to_execute[traverse]=='D')
		{

			traverse+=1;

			for(;to_execute [traverse]==' ';++traverse);

			string num="";

			for(;to_execute[traverse]!=' '&& to_execute[traverse]!='\n'&& to_execute[traverse]!='\0';++traverse)
			{
				num+=to_execute[traverse];
			}

			int no=stoi(num);

			(p->obj).value-=no;

		}
		else if(to_execute[traverse]=='B')
		{
					block.push(*p);
					context_switching();
		}
		else if(to_execute[traverse]=='E')
		{
			p->free_all();
			running.pop();

		}
		else if(to_execute[traverse]=='R')
		{

			traverse+=1;
			for(;to_execute [traverse]==' ';++traverse);

			string fname="";

			for(;to_execute[traverse]!=' '&& to_execute[traverse]!='\n'&& to_execute[traverse]!='\0';++traverse)
			{
				fname+=to_execute[traverse];
			}


			ifstream ifile;
			ifile.open(fname.c_str(),ios::in);

			if (!ifile)
			{
				cout << "File can  not  be opened." << endl;
				exit(-1);
			}

				string total="";
				string temp="";

				int count = 0;

				while (!ifile.eof())
				{
					getline(ifile,temp,'\n');
					total+=temp;
					total+="\n";
					count+=1;
				}

				ifile.close();

				string* instr = new string[count];

				for(int i=0;i<count;++i)
				{
					instr[i]="";
				}

				int traverse=0;
				for (int i = 0; i < count; ++i)
				{
					for(int j=0;total[traverse]!='\n' && total[j]!='\0' ;++j)
					{
	    				instr[i]+=total[traverse];
	    				traverse+=1;
					}
				traverse+=1;

				}

				(p->obj).instruction_set=instr;
				(p->obj).number_of_instructions=count;
				(p->obj).pc=0;
				(p->obj).value=0;
		}
		else
		{
			printf("Invalid instruction: %s",to_execute);

			return;
		}

		gettimeofday(&stop, NULL);

		cout<<"Final is "<<stop.tv_usec - start.tv_usec;
		p->CPU_TIME+=stop.tv_usec - start.tv_usec;
		return;
	}


}





void handler(int signo)
{
	flag = true;
	char buffer[255];
	close(fd[0]);  //closing the read end of  the pipe
	cout << "Enter the command:   ";
	fgets(buffer,255,stdin);
	write(fd[1],buffer, 255); //write end fd[1] writing in pipe


}

void PRT()
{

	pid_t id=fork();
	if(id==0)
	{time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);
		 time_t mytime;
		 mytime = time(NULL);
		 double final=difftime( mytime,start_timing);

			cout<<"**************************************************************** "<<endl;
			cout<<"The current system state is as follows: "<<endl;
			cout<<"**************************************************************** "<<endl;
			cout<<"CURRENT TIME:";
			 strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
			 puts(buffer);
			 cout<<"TOTAL TIME PASSED:"<<final<<"time units"<<endl;
			cout<<"RUNNING PROCESS:"<<endl;

			(running.front()).print_itself();
			cout<<"Next Instruction:";
			cout<<((running.front()).obj).instruction_set[(((running.front()).obj).pc)+1]<<endl;

			cout<<"CURRENT Instruction:";
			cout<<((running.front()).obj).instruction_set[(((running.front()).obj).pc)]<<endl;
			cout<<"BLOCKED PROCESSES:"<<endl;
			cout<<"Queue of processes: "<<endl;
			for(int i=0;i<block.size();++i)
			{
				(block.front()).print_itself();
				block.push(block.front());
				block.pop();
			}
			cout<<"READY PROCESSES:"<<endl;
			cout<<"Queue of processes: "<<endl;
			for(int i=0;i<ready.size();++i)
			{
					(ready.front()).print_itself();
					ready.push(ready.front());
					ready.pop();
			}


			cout<<"The order of execution for each process \n";

			for(int i=0;i<execution_record.size();++i)
			{
				cout<<"p"<<execution_record.front()<<endl;
				execution_record.push(execution_record.front());
				execution_record.pop();
			}

			exit(0);

	}
	else
	{
		int dd;
		wait(&dd);
		return;
	}
}

void CRT(const string& file_name, char buffer[],int id)
{
	cout << file_name.length();

	ifstream ifile;
	ifile.open(file_name.c_str(),ios::in);  //jiss file kooo open karr rahay hainn uss main instruction parii huvii hainnn

	if (!ifile)
	{
		cout << "File is not opened." << endl;
		exit(-1);
	}

	string total="";
	string temp="";

	int count = 0;

	while (!ifile.eof())
	{
		getline(ifile,temp,'\n');
		total+=temp;
		total+="\n";
		count+=1;
	}
	ifile.close();


	string* instr = new string[count];

	for(int i=0;i<count;++i)
	{
		instr[i]="";
	}

	int traverse=0;
	for (int i = 0; i < count; ++i)
	{
		for(int j=0;total[traverse]!='\n' && total[j]!='\0' ;++j)
		{
	    	instr[i]+=total[traverse];
	    	traverse+=1;
		}
		traverse+=1;

	}
	Process simu_process(id,0,0,0,"ready",instr,count);	// Create Simulated Process

	ready.push(simu_process);
	if(running.empty())
	{
		running.push(ready.front());
		ready.pop();
		execution_record.push( ( ( running.front() ).pid) );
		execution_record_entries+=1;
	}

}





int main() {




	int res = pipe(fd); //checking the validity of creation of pipe in the if statement
	if (res < 0)
	{
		cout << "Pipe is not created" << endl;
		exit(-1);
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		cout << "Error in fork." << endl;
		exit(-1);
	}
	else if (pid > 0)
	{
		signal(SIGALRM, handler);

		for (;;)
		{
			if (flag)
			{

				flag = false;
				//cout << "haseeb" << endl;
				alarm(2);

			}
		}

	}
	else
	{
		while(true)
		{
			close(fd[1]); //close write end fd[1]
			char buffer[255];
			read(fd[0],buffer,255);

			if (buffer[0] == 'C' and buffer[1] == 'R' and buffer[2] == 'T') // Check input as CRT <file name>
			{
				string file_name = ""; //for file name in CRT <file name>

					for (int i = 4; buffer[i] != '\0' && buffer[i] != '\n' && i < 255; ++i) ///gurmani sabb kaa space walla flaw remove karna hai jiss tarha asad kaa kiaa thhaa

					{
						//cout << int(buffer[i]) << endl;
						file_name += buffer[i];           //concatinating filename
					}

					CRT(file_name,buffer,getpid());


			}
			else if (buffer[0] == 'U' and buffer[1] == 'N' and buffer[2] == 'B')
			{
				if (!block.empty())
				{


					ready.push(block.front());
					block.pop();
				}
			}
			else if(buffer[0] == 'E' and buffer[1] == 'N' and buffer[2] == 'D')
			{
				PRT();
				double avg=0;
				int num=0;
				while(turn_around_time.empty())
				{
					num+=1;
					avg+=turn_around_time.front();
					turn_around_time.pop();
				}
				avg=avg/num;
				cout<<"Turnaround time : "<<avg<<endl;//getTurnaroundTime<<endl;
				cout<<"Terminated!!!"<<endl;
				kill(getppid() ,SIGINT);
				exit(0);
			}
			else if (buffer[0] == 'I' and buffer[1] == 'N' and buffer[2] == 'C')
			{
				Execute_instruction(&running.front());
				time_slice+=1;
				if(time_slice==3)
				{
					context_switching();
				}
			}

			else if (buffer[0] == 'P' and buffer[1] == 'R' and buffer[2] == 'T')
			{
				PRT();
			}

		}
	}


	return 0;
}
