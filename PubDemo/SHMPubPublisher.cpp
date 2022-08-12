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
 * @file SHMPubPublisher.cpp
 *
 */

#include "SHMPubPublisher.h"
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/Domain.h>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>

#include <thread>
#include<sys/time.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
using namespace eprosima::fastdds::rtps;
#define USEC_PER_SEC		1000000
int tag = 0;
SHMPubPublisher::SHMPubPublisher()
    : mp_participant(nullptr)
    , mp_publisher(nullptr)
{
}
static inline int64_t pubcalcdiff(struct timespec t1)
{
	int64_t diff = USEC_PER_SEC * ((long long)(int) t1.tv_sec);
	diff += ((int) t1.tv_nsec) / 1000;
	return diff;
}
bool SHMPubPublisher::init()
{
    m_pub = std::make_shared<SHMDemo>();
    m_pub->index(0);
    //m_Hello->message("HelloWorld");
    ParticipantAttributes PParam;
    PParam.rtps.builtin.discovery_config.discoveryProtocol = DiscoveryProtocol_t::SIMPLE;
    PParam.rtps.builtin.discovery_config.use_SIMPLE_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationReaderANDSubscriptionWriter = true;
    PParam.rtps.builtin.discovery_config.m_simpleEDP.use_PublicationWriterANDSubscriptionReader = true;
    PParam.rtps.builtin.discovery_config.leaseDuration = c_TimeInfinite;

    PParam.rtps.setName("Participant_pub");

    // SharedMem transport configuration
    PParam.rtps.useBuiltinTransports = false;

    auto shm_transport = std::make_shared<SharedMemTransportDescriptor>();
    shm_transport->segment_size(2 * 1024 * 1024);
    PParam.rtps.userTransports.push_back(shm_transport);

    // UDP
    auto udp_transport = std::make_shared<UDPv4TransportDescriptor>();
    //udp_transport->interfaceWhiteList.push_back("127.0.0.1");
    PParam.rtps.userTransports.push_back(udp_transport);

    mp_participant = Domain::createParticipant(PParam);

    if (mp_participant == nullptr)
    {
        return false;
    }
    //REGISTER THE TYPE

    Domain::registerType(mp_participant, &m_type);

    //CREATE THE PUBLISHER
    PublisherAttributes Wparam;
    Wparam.topic.topicKind = NO_KEY;
    Wparam.topic.topicDataType = "SHMDemo";
    Wparam.topic.topicName = "SHMDemoTopic";
    Wparam.topic.historyQos.kind = KEEP_LAST_HISTORY_QOS;
    Wparam.topic.historyQos.depth = 30;
    Wparam.topic.resourceLimitsQos.max_samples = 50000;
    Wparam.topic.resourceLimitsQos.allocated_samples = 2000;
    Wparam.times.heartbeatPeriod.seconds = 2;
    Wparam.times.heartbeatPeriod.nanosec = 200 * 1000 * 1000;
    Wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    Wparam.qos.m_publishMode.kind = ASYNCHRONOUS_PUBLISH_MODE;
    mp_publisher = Domain::createPublisher(mp_participant, Wparam, (PublisherListener*)&m_listener);
    if (mp_publisher == nullptr)
    {
        return false;
    }

    return true;
}

SHMPubPublisher::~SHMPubPublisher()
{
    // TODO Auto-generated destructor stub
    Domain::removeParticipant(mp_participant);
}

void SHMPubPublisher::PubListener::onPublicationMatched(
        Publisher* /*pub*/,
        MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        firstConnected = true;
        std::cout << "Publisher matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Publisher unmatched" << std::endl;
    }
}


bool SHMPubPublisher::publish(
        std::string message)
{
    //int pubind = 0;
    struct timespec pubtime;
    bool waitForListener = false;
    m_pub->message(message);
    
    if (m_listener.firstConnected || !waitForListener || m_listener.n_matched > 0)
    { 
        tag = 1;
        m_pub->index(m_pub->index() + 1);
        int pubind = m_pub->index();
	    clock_gettime(CLOCK_MONOTONIC, &pubtime);
	    int64_t diff = pubcalcdiff(pubtime);
	    m_pub->time(diff);
        mp_publisher->write((void*)m_pub.get());
	    tag = 2;
        return true;
    }
   
    return false;
}
