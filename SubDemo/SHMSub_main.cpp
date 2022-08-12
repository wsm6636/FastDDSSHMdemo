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

#include "SHMSubSubscriber.h"


#include <fastrtps/Domain.h>
#include <fastrtps/log/Log.h>


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

using namespace eprosima;
using namespace fastrtps;
using namespace rtps;
static int f = 0;
int main(
        int argc,
        char** argv)
{
    /*int fds = open("/sys/fs/cgroup/cpuset/isolatedsub/cgroup.procs", O_WRONLY);
    if (fds < 0) {
       	perror("Can't open cpuset file...\n");
    }		
    write(fds, "0\n", 2);
	*/
    std::cout << "Starting " << std::endl;
    const char* subfilname;
    int type = 2;
    int count = 10;
    long sleep = 100;
    int *flag = &f;
    //struct timespec next_period;
    //next_period.tv_nsec = 1000;
    if (argc > 2)
    {
	std::cout << "publisher, subscriber or both argument needed" << std::endl;
        Log::Reset();
       	return 0;
        
    }else if (argc == 2)
            {
		
		subfilname = argv[1];
		SHMSubSubscriber mysub;
    		if (mysub.init(&flag))
    		{
       			mysub.run();
    		}
		
		while(1)
                {
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
			//std::cout << " sleep " << sleep << std::endl;
			//clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_period, NULL);
			if (f == 1) {
				std::cout<<"found"<<std::endl;
				mysub.wfile(subfilname);
				break;
			}
		}
    		
   	    }
    SHMSubSubscriber mysub;
    if (mysub.init(&flag))
    {
        mysub.run();
    }
   
            
           
        

    Domain::stopAll();
    Log::Reset();
    //close(fds);
    return 0;
}
