//
//  main.cpp
//  project1
//
//  Created by Wei Du on 15/01/2017.
//  Copyright © 2017 Wei Du. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <queue>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <list>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until

using namespace std::chrono; // nanoseconds, system_clock, seconds

class Packet {
public:
    double length;   //Packet Size
    double starttime;  //TICK when the packet is generated
    double servertime;  //TICK when the packet start processed by server
    double endtime;   //TICK when the packet finish serving by server
    double complettime; //TICK when packet finish hub process
    double i_value; //i in lab2
};

class Computer {
public:
    queue<Packet*> newly_created_packets;
    queue<Packet*> buffer;
    queue<Packet*> server;
    queue<Packet*> completed_packets;
    int timer;
    int id;
};


//universal hub
queue<Packet*> local_wire;
queue<Packet*> hub;
queue<Packet*> finsih_pak;


// Two universal tick counts
double buffer_empty_ticks = 0;
double dropped_packets = 0;
double Waittime =0;

// Buffer to keep track of the queue sizes at different ticks
queue<int> total_queue_size;

double random_gen(double i, double convertion){
    std::mt19937_64 rng;
    double uniform = 0;
    double randomGeneration = 0;
    double result = 0;
    
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
    rng.seed(ss);
    
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
    
    // ready to generate random numbers
    uniform = unif(rng);
    
    //std::cout << uniform << "\n";
    randomGeneration = (-1) / i*log(1 - uniform);
    result = randomGeneration * convertion;
    
    return result;
}
double exp_gen(double i){
    std::mt19937_64 rng;
    int min = 0;
    int max = pow(2, i);
    double random = 0;
    
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_int_distribution<int> gen(min, max);
    
    // ready to generate random numbers
    random = gen(rng);
    random = random * 512;
    return random;
}

int packet_gen_MD1(double ticks, double size, double packet_start, Computer* comp){
    if (!comp->newly_created_packets.empty()){
        if (comp->newly_created_packets.front()->starttime <= ticks){
            Packet *new_packet = new Packet;
            new_packet->length = size;
            new_packet->starttime = packet_start;
            new_packet->i_value = 0;
            
            Packet *tmp = comp->newly_created_packets.front();
            //cout << "comp->newly_created_packets.front().starttime: " << comp->newly_created_packets.front()->starttime << "ticks: " << ticks << "\n";
            comp->buffer.push(tmp);
            //cout << comp->buffer.size();

            comp->newly_created_packets.pop();
            
            comp->newly_created_packets.push(new_packet);
            //cout << "comp->buffer.front().starttime: " << comp->buffer.front()->starttime << "ticks: " << ticks << "\n";

        }
    }
    return 0;
}

int buffer_detect(double ticks, double transmission_rate, Computer* comp){
    if (comp->server.empty()){
        if (!comp->buffer.empty()){
            //cout << "working";
            Packet *tmp = comp->buffer.front();
            tmp->servertime = ticks;
            
            tmp->endtime = ticks + ((tmp->length / transmission_rate) * 100000); // =ticks + 800
            //--TODO for non-presistent remove timer thing.
            if (local_wire.empty() && comp->timer <= 0){
            //if (local_wire.empty() ){
                //cout  << "this";
                comp->server.push(tmp);
                local_wire.push(tmp);
               // cout << "local_wire pushed \n";
                comp->buffer.pop();
            }
            else{
                if (!local_wire.empty()){
                    //cout << "local_wire no empty \n";
 
                    /*
                    if (comp->buffer.front()->i_value > 10){
                        comp->buffer.pop();
                        //cout << "10 attempt limit reached \n";
                    }
                    else{
                        comp->buffer.front()->i_value++;
                        tmp->i_value++;
                        
                        // for non-presistent remove this
                        //comp->timer = exp_gen(tmp->i_value);
                        // -- TODO
                    }
                     */
                    //comp->buffer.front()->i_value++;
                    //tmp->i_value++;
                    
                    // for non-presistent remove this
                    comp->timer = Waittime;
                    // -- TODO
                }
                else{
                    comp->timer--;
                }
            }
        }
    }
    return 0;
}
int server_detect(double ticks, Computer* comp){
    if (comp->server.empty()){

    }
    else{
        //cout << "server not empty";

        Packet *tmp = comp->server.front();
        
        //cout << "comp->server.front().servertime: " << comp->server.front()->servertime << "ticks: " << ticks << "\n";
        //cout << "comp->server.front().endtime: " << comp->server.front()->endtime << "ticks: " << ticks << "\n";
    
        if (comp->server.front()->servertime + 2.6 <= ticks){
            
            if (local_wire.size() > 1){
                tmp->i_value++;
                double new_time = exp_gen(tmp->i_value);
                //asdasdasd
                Waittime = new_time;
                comp->server.front()->endtime += new_time;
                comp->server.front()->servertime += new_time;
                if (comp->server.front()->i_value > 10){
                    comp->server.pop();
                    cout << "10 attempt limit reached";
                }
                else{
                comp->server.front()->i_value++;
                }
                while (!local_wire.empty())
                {
                    // Pop the front packet
                    local_wire.pop();
                }
                //cout << "not working \n";
            }
            else{
               // cout << "comp->server.front().endtime: " << comp->server.front()->endtime << "\n";
                if (comp->server.front()->endtime <= ticks){
                    finsih_pak.push(tmp);
                    //cout << "finished" << "\n";
                    comp->server.pop();
                    while (!local_wire.empty())
                    {
                        // Pop the front packet
                        local_wire.pop();
                    }
                    return 0;
                    
                }
            }
        }
        return 0;
    }
    return 0;
}

double computer_handler(Computer* computer123, double ticks, double lambda, double speed, double pktLength){
    double packet_start = random_gen(lambda, 100000) + ticks;
    packet_gen_MD1(ticks, pktLength, packet_start, computer123);
    buffer_detect(ticks, speed, computer123);
    server_detect(ticks, computer123);
    return 0;
}


int main(int argc, char* argv[]){
    //lab2 code start here
    
    // Declarations
    int numberComputers = 0;
    double avgArrivalRate = 0;
    double collisionDetected = 0;
    double speed = 1000000;
    double pktLength = 8000;
    double pktLengthBytes = pktLength / 8;
    double i = 0;
    
    double bitTime = 1 / speed;
    bool busy = true;
    
    // Ask user for number of computers on the LAN
    cout << "Please enter the number of computers on the LAN: ";
    
    // Store response in numberComputers
    cin >> numberComputers;
    
    // Ask user for average arrival rate of packets
    cout << "Please enter the desired average arrival rate of the packets: ";
    
    // Store response in avgArrivalRate
    cin >> avgArrivalRate;
    
    // Tell user the speed of the connection
    cout << "The speed of the LAN is set to " << speed << " bits per second.\n";
    
    // Tell user the packet length
    cout << "The packet length is " << pktLength << " bits.\n";
    vector<Computer*> computers;
    
    for (int b = 0; b < numberComputers; b++){
        Computer* computer = new Computer;
        computer->timer = 0;
        computer->id = b;
        double first_packet_start = random_gen(avgArrivalRate, 100000);
        //cout << "first packet: " << first_packet_start << "\n";
        Packet *first_packet = new Packet;
        first_packet->length = pktLength;
        first_packet->starttime = first_packet_start;
        
        computer->newly_created_packets.push(first_packet);
        computers.push_back(computer);
    }
    
    cout << "Number of computers: " << computers.size();
    
    
    for (int ticks = 0; ticks <= 100000; ticks++){
        for(auto it = computers.begin(); it != computers.end(); it++)
        {
            computer_handler(*it, ticks, avgArrivalRate, speed, pktLength);
           // cout << local_wire.size() << "backoff here \n";

        }
    }
    double total_ticks = 0;
    int total = finsih_pak.size();
    // While we haven't iterated through all of the packets that passed through the queue
    while (!finsih_pak.empty())
    {
        // Add the difference between the end and start times of the front packet
        total_ticks += (finsih_pak.front()->endtime - finsih_pak.front()->starttime);
        
        // Pop the front packet
        finsih_pak.pop();
    }
    
    // Calculate the average time a packet took to be evaluated
    double E_T = total_ticks / total;
    
    cout << "total:" << total << "\n";
    cout << "average_delay:" << E_T << "\n";

    return 0;
}




