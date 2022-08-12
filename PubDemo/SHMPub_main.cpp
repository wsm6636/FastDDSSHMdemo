// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file SHMPub_main.cpp
 *
 */

#include "SHMPubPublisher.h"


#include <fastrtps/Domain.h>
#include <fastrtps/log/Log.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
using namespace eprosima;
using namespace fastrtps;
using namespace rtps;
SHMPubPublisher mypub;
std::string str;
struct period_info {
        struct timespec next_period;
        long period_ns;
};
std::string rand_str(const int len)  
{

    std::string str;                
    char c;                     
    int i;                  
    for(i = 0 ; i < len ; i++)
    {
 
        c = 'a' + rand()%26;
        //c = 'a';
	str.push_back(c);      
    }
    return str;                 
}
static void inc_period(struct period_info *pinfo) 
{
        pinfo->next_period.tv_nsec += pinfo->period_ns;
 
        while (pinfo->next_period.tv_nsec >= 1000000000) {
                /* timespec nsec overflow */
                pinfo->next_period.tv_sec++;
                pinfo->next_period.tv_nsec -= 1000000000;
        }
	//std::cout << "inc_period " << std::endl;
}
 
static void periodic_task_init(struct period_info *pinfo)
{
        /* for simplicity, hardcoding a 1ms period */
        pinfo->period_ns = 1000000;
 
        clock_gettime(CLOCK_MONOTONIC, &(pinfo->next_period));
	//std::cout << "periodic_task_init" << std::endl;
}
/* 
static void do_rt_task()
{
        // Do RT stuff here. 

    std::string str = rand_str(2000);		 
    mypub.publish(str);
}*/
 
static void wait_rest_of_period(struct period_info *pinfo)
{
        inc_period(pinfo);
 
        /* for simplicity, ignoring possibilities of signal wakes */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &pinfo->next_period, NULL);
}
void *thread_func(void *data)
{
        /* Do RT specific stuff here */
        struct period_info pinfo;
        
        int count = 60000;
	int a = 0 ;
        periodic_task_init(&pinfo);
        //std::string str = rand_str(2000);		 
        while (1) {
		if(a>=count){
			std::cout << "break" << std::endl;
			break;
		}
		else a++;             	
		str = rand_str(2000);		 
    		mypub.publish(str);
                wait_rest_of_period(&pinfo);
			
        }
        return NULL;
}

int main(
        int argc,
        char** argv)
{
   int fd = open("/sys/fs/cgroup/cpuset/isolated/cgroup.procs", O_WRONLY);
    if (fd < 0) {
       	perror("Can't open cpuset file...\n");
    }		
    write(fd, "0\n", 2);
  // str = rand_str(2000); 
    std::cout << "Starting " << std::endl;

    struct sched_param param;
    pthread_attr_t attr;
    pthread_t thread;
    int ret;	

    
    if (argc > 1)
    {

        std::cout << "publisher, subscriber or both argument needed" << std::endl;
        Log::Reset();
        ret = 1;
        goto out;
    }else std::cout << "publisher" << std::endl;
    std::cout << "mypub.init();" << std::endl;  
    
    if(!mypub.init()){
    	std::cout << "mypub.init() failed" << std::endl;
	ret = 1;
        goto out;	
    }
	
	thread = pthread_self();
    
    /* Lock memory */
        if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
     		std::cout << "mlockall failed: %m\n" << std::endl;
                exit(-2);
        }else std::cout << "Lock memory" << std::endl;
 
        /* Initialize pthread attributes (default values) */
        ret = pthread_attr_init(&attr);
        if (ret) {
                std::cout <<"init pthread attributes failed\n"<< std::endl;
                goto out;
        }else std::cout << "pthread_attr_init" << std::endl;
 
        /* Set a specific stack size  */
        ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
        if (ret) {
            std::cout <<"pthread setstacksize failed\n"<< std::endl;
            goto out;
        }else std::cout << "pthread_attr_setstacksize" << std::endl;
 
        /* Set scheduler policy and priority of pthread */
        ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        if (ret) {
                std::cout <<"pthread setschedpolicy failed\n"<< std::endl;
                goto out;
        }else std::cout << "pthread_attr_setschedpolicy" << std::endl;
        
	param.sched_priority = 99;
	std::cout << "param.sched_priority " << param.sched_priority  << std::endl;

        ret = pthread_attr_setschedparam(&attr, &param);
        if (ret) {
                std::cout <<"pthread setschedparam failed\n"<< std::endl;
                goto out;
        }else std::cout << "pthread_attr_setschedparam" << std::endl;

        /* Use scheduling parameters of attr */
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (ret) {
                std::cout <<"pthread setinheritsched failed\n"<< std::endl;
                goto out;
        }else std::cout << "pthread_attr_setinheritsched" << std::endl;
 
        /* Create a pthread with specified attributes */
        ret = pthread_create(&thread, &attr, thread_func, NULL);
        if (ret) {
               std::cout <<"create pthread failed\n"<< std::endl;
                goto out;
        }else std::cout << "pthread_create" << std::endl;
 
        /* Join the thread and wait until it is done */
        ret = pthread_join(thread, NULL);
        if (ret)
                std::cout <<"join pthread failed: %m\n"<< std::endl;
	else std::cout << "pthread_join" << std::endl;
 	
out:
	Domain::stopAll();
        Log::Reset();
   
        close(fd);
        return ret;       
     
	
    
    //return 0;
}
