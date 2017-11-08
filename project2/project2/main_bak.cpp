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
    queue<Packet> newly_created_packets;
    queue<Packet> buffer;
    queue<Packet> server;
    queue<Packet> completed_packets;
    int timer;
};


//universal hub
queue<Packet> local_wire;
queue<Packet> hub;
queue<Packet> finsih_pak;


// Two universal tick counts
double buffer_empty_ticks = 0;
double dropped_packets = 0;

// Universal signals
bool aborting = false;
int computersSending = 0;

// Buffer to keep track of the queue sizes at different ticks
queue<int> total_queue_size;

double random_gen(double i, double convertion){
    std::mt19937_64 rng;
    double uniform = 0;
    double randomGeneration = 0;
    double result = 0;
    
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
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
    int max = pow(2,i);
    double random = 0;
    
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_int_distribution<int> gen(min, max);
    
    // ready to generate random numbers
    random = gen(rng);
    random = random*512;
    return random;
}

int packet_gen_MD1(double time, double size, double arrival, Computer comp){
    // If this is not the first packet being generated
    if (!comp.newly_created_packets.empty()){
        // Only create packet if the start time has not already occured
        if (comp.newly_created_packets.front().starttime <= time){
            // Create a new packet and populate the attributes
            Packet new_packet;
            new_packet.length = size;
            new_packet.starttime = arrival;
            new_packet.i_value = 0;
            
            // Get the front value of the first queue. Push to the buffer and then pop the value
            Packet tmp = comp.newly_created_packets.front();
            comp.buffer.push(tmp);
            comp.newly_created_packets.pop();
            
            // Add the new packet the the queue
            comp.newly_created_packets.push(new_packet);
        }
    }
    return 0;
}

int buffer_detect(double time, double transmission_rate, Computer comp){
    // See if the buffer is empty

    
    // If the server is empty
    if (comp.server.size() == 0){
        // And the buffer is not empty
        if (!comp.buffer.empty()){
            // Get the value of the front packet and populate the attributes
            Packet tmp = comp.buffer.front();
            tmp.servertime = time;
            
            // TODO: where is the hardcoded vlaue of 1000000 coming from?
            tmp.endtime = time + ((tmp.length / transmission_rate) * 1000000);
            
            
            if (local_wire.empty()){

            // Push the value to the server
            comp.server.push(tmp);
            
            // Pop the value from the buffer
            comp.buffer.pop();
            }
            else{
                comp.buffer.front().i_value ++;
                tmp.i_value ++;
                comp.timer = exp_gen(tmp.i_value);
            }
        }
    }
    return 0;
}
int server_detect(double time, Computer comp){
    // Get the value of the front packet
    Packet tmp = comp.server.front();

    // If the server is not empty
    if (!comp.server.empty()){
        // Only push the packet if the end time has not already occured
        
        }
    else{
    if(comp.server.front().servertime + 26 <= time){
        if (local_wire.empty()){
            tmp.complettime = time + ((tmp.length / 1000000) * 1000000) - 26;
            computersSending++;
            local_wire.push(tmp); //push it to local_wire
            // Push it to completed_packets
            comp.completed_packets.push(tmp);
        }
        else{
            tmp.i_value ++;
            double new_time = exp_gen(tmp.i_value);
            comp.server.front().endtime = new_time;
            comp.server.front().i_value ++;

        }

    }
        if (comp.server.front().endtime <= time){
            finsih_pak.push(tmp);
            return 0;

        }
    return 0;

    
    return 0;
    }
    return 0;
}






Computer computer_handler(Computer computer123,double time){
    double packet_start = random_gen(lambda, 1000000) + time;
    double size=8000;
    double arrival = 0;
    double transmissionrate=1000000;
    packet_gen_MD1(packet_start,size,arrival,computer123);
    buffer_detect(time,transmissionrate,computer123);
    server_detect(time,computer123);
    return tmp;
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
    cout << "The speed of the LAN is set to " << speed << " bits per second.";
    
    // Tell user the packet length
    cout << "The packet length is " << pktLength << " bits.";
    Computer *network = new Computer[numberComputers]; //right
    
    for (int i = 0; i <= ticks; i++){
        for (int b = 0; b<numberComputers; b++){
            computer_handler(network[b]);
        }
    }
    return 0;
    
    /*
     // Declarations
     double i = 0;
     double transmission_rate = 0;
     double packet_size = 0;
     double lambda = 0;
     double ticks = 0;
     double first_packet_start = 0;
     double packet_start = 0;
     double utilization_rate = 0;
     long total_packets_evaluated = 0;
     double total_ticks = 0;
     double E_T = 0;
     double total_size = 0;
     double sample_time = 0;
     double E_N = 0;
     
     // Empty all of the buffers
     newly_created_packets.empty();
     buffer.empty();
     server.empty();
     
     // Ask user for simulation type
     cout << "Please enter simulation type: 0 for (M/D/K) 1 for (M/D/K/1): ";
     
     // Store response in i
     cin >> i;
     
     // M/D/K
     if (i == 0){
     // Inform the user of their choice
     cout << "-------M/D/K Mode------ ";
     
     // Ask user for transmission rate
     cout << "Please enter the transmission rate: ";
     
     // Store response in transmission_rate
     cin >> transmission_rate;
     
     // Ask user for the packet size
     cout << "Please enter the desired size of the packets: ";
     
     // Store response in packet_size
     cin >> packet_size;
     
     // Ask user for lambda value (Packets per second)
     cout << "Please enter the desired packets per second: ";
     
     // Store response in lambda
     cin >> lambda;
     
     // Ask user for number of TCISK they wish to take place
     cout << "Please enter the nubmer of TICKS you wish to elapse: ";
     
     // Store response in time
     cin >> ticks;
     
     
     // Use the random number generator to decide when to load the first packet
     first_packet_start = random_gen(lambda, 1000000);
     
     // Create a new packet with the packet length specified by the user and start time dictated by the RNG
     Packet first_packet;
     first_packet.length = packet_size;
     first_packet.starttime = first_packet_start;
     
     // Push the initial packet into the queue
     newly_created_packets.push(first_packet);
     
     // Iterate until the specified amount of ticks have elpased
     for (int i = 0; i <= ticks; i++){
     // Find the start time of the new packet via RNG
     packet_start = random_gen(lambda, 1000000) + i;
     
     // Generate the new packet
     packet_gen_MDK(i, packet_size, packet_start);
     
     buffer_detect(i, transmission_rate);
     server_detect(i);
     }
     
     // Show to user the number of ticks where the buffer is empty
     cout << "Ticks elapsed with an empty buffer:" << buffer_empty_ticks << "\n";
     
     // Calculate utilization rate
     utilization_rate = (buffer_empty_ticks / ticks) * 100;
     
     // Show to user the percentage of time where the buffer was empty
     cout << "Utilization rate:" << round(utilization_rate) << "%\n";
     
     // Find the number of packets that passed through the queue
     total_packets_evaluated = completed_packets.size();
     
     // Show to user the number of packets that passed through the queue
     cout << "total_Packet:" << total_packets_evaluated << "\n";
     
     // While we haven't iterated through all of the packets that passed through the queue
     while (!completed_packets.empty())
     {
     // Add the difference between the end and start times of the front packet
     total_ticks += (completed_packets.front().endtime - completed_packets.front().starttime);
     
     // Pop the front packet
     completed_packets.pop();
     }
     
     // Calculate the average time a packet took to be evaluated
     E_T = total_ticks / total_packets_evaluated;
     
     // Show to user the average time a packet took to be evaluated
     cout << "Average soujorn time (E[T]):" << E_T << "\n";
     
     // Number of entries in the queue that is keeping track of the size of the queue
     sample_time = total_queue_size.size();
     cout << "Number fo entries in the total_queue_size queue:" << sample_time << "\n";
     
     // While we have not iterated across the entrie buffer
     while (!total_queue_size.empty())
     {
     // Add the first value to the total
     total_size += total_queue_size.front();
     
     // Pop the first value
     total_queue_size.pop();
     }
     
     // Calculate the average number of packets in the queue
     E_N = total_size / sample_time;
     
     // Show to user the average number of packets in the queue
     cout << "Average number of packets in the queue (E[N]):" << E_N << "\n";
     
     // Find out how many packets have not entered the queue
     long newly_created_packets_remaining = newly_created_packets.size();
     
     // Display to user the number of packets that have not entered the queue
     cout << "newly_created_packets_remaining:" << newly_created_packets_remaining << "\n";
     }
     else{
     double buffer_size = 0;
     double packet_loss = 0;
     
     // Inform the user of their choice
     cout << "-------M/D/K/1 Mode------ ";
     
     // Ask user for buffer size
     cout << "Please enter buffer size: ";
     
     // Store response in buffer_size
     cin >> buffer_size;
     
     // Ask user for transmission rate
     cout << "Please enter the transmission rate: ";
     
     // Store response in transmission_rate
     cin >> transmission_rate;
     
     // Ask user for the packet size
     cout << "Please enter the desired size of the packets: ";
     
     // Store response in packet_size
     cin >> packet_size;
     
     // Ask user for lambda value (Packets per second)
     cout << "Please enter the desired packets per second: ";
     
     // Store response in lambda
     cin >> lambda;
     
     // Ask user for number of TCISK they wish to take place
     cout << "Please enter the nubmer of TICKS you wish to elapse: ";
     
     // Store response in time
     cin >> ticks;
     
     for (int time_avg = 0; time_avg < 4; time_avg++){
     //initialze value
     packet_loss = 0;
     while (!newly_created_packets.empty())
     {
     newly_created_packets.pop();
     }
     while (!buffer.empty())
     {
     buffer.pop();
     }
     while (!server.empty())
     {
     server.pop();
     }
     while (!completed_packets.empty())
     {
     completed_packets.pop();
     }
     while (!total_queue_size.empty())
     {
     total_queue_size.pop();
     }
     total_packets_evaluated = 0;
     total_ticks = 0;
     E_T = 0;
     total_size = 0;
     sample_time = 0;
     E_N = 0;
     buffer_empty_ticks = 0;
     dropped_packets = 0;
     
     // Use the random number generator to decide when to load the first packet
     first_packet_start = random_gen(lambda, 1000000);
     
     // Create a new packet with the packet length specified by the user and start time dictated by the RNG
     Packet first_packet;
     first_packet.length = packet_size;
     first_packet.starttime = first_packet_start;
     
     // Push the initial packet into the queue
     newly_created_packets.push(first_packet);
     
     // Iterate until the specified amount of ticks have elpased
     for (int i = 0; i <= ticks; i++){
     // Find the start time of the new packet via RNG
     packet_start = random_gen(lambda, 1000000) + i;
     
     // Generate the new packet
     packet_gen_MDK1(i, packet_size, packet_start, buffer_size);
     
     
     buffer_detect(i, transmission_rate);
     server_detect(i);
     }
     // Show to user the number of ticks where the buffer is empty
     cout << "Ticks elapsed with an empty buffer:" << buffer_empty_ticks << "\n";
     
     // Calculate utilization rate
     utilization_rate = (buffer_empty_ticks / ticks) * 100;
     
     // Show to user the percentage of time where the buffer was empty
     cout << "Utilization rate:" << round(utilization_rate) << "%\n";
     
     // Find the number of packets that passed through the queue
     total_packets_evaluated = completed_packets.size();
     
     // Show to user the number of packets that passed through the queue
     cout << "total_Packet:" << total_packets_evaluated << "\n";
     
     // While we haven't iterated through all of the packets that passed through the queue
     while (!completed_packets.empty())
     {
     // Add the difference between the end and start times of the front packet
     total_ticks += (completed_packets.front().endtime - completed_packets.front().starttime);
     
     // Pop the front packet
     completed_packets.pop();
     }
     
     // Calculate the average time a packet took to be evaluated
     E_T = total_ticks / total_packets_evaluated;
     
     // Show to user the average time a packet took to be evaluated
     cout << "Average soujorn time (E[T]):" << E_T << "\n";
     
     // Number of entries in the queue that is keeping track of the size of the queue
     sample_time = total_queue_size.size();
     cout << "Number fo entries in the total_queue_size queue:" << sample_time << "\n";
     
     // While we have not iterated across the entrie buffer
     while (!total_queue_size.empty())
     {
     // Add the first value to the total
     total_size += total_queue_size.front();
     
     // Pop the first value
     total_queue_size.pop();
     }
     
     // Calculate the average number of packets in the queue
     E_N = total_size / sample_time;
     
     // Show to user the average number of packets in the queue
     cout << "Average number of packets in the queue (E[N]):" << E_N << "\n";
     
     // Calculate the number of packets that were lost
     packet_loss = dropped_packets / (total_packets_evaluated + dropped_packets) * 100;
     
     // Show to user the number of packets that were lost
     cout << "Number of packets that were lost:" << packet_loss << "\n";
     
     // Find out how many packets have not entered the queue
     long newly_created_packets_remaining = newly_created_packets.size();
     
     // Display to user the number of packets that have not entered the queue
     cout << "newly_created_packets_remaining:" << newly_created_packets_remaining << "\n";
     }
     }
     */
}


