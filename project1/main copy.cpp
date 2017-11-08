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


using namespace std;

class Packet {
public:
    double length;   //Packet Size
    double starttime;  //TICK when the packet is generated
    double servertime;  //TICK when the packet start processed by server
    double endtime;   //TICK when the packet finish serving by server
};
queue<Packet> level1;
queue<Packet> buffer;
queue<Packet> server;
queue<Packet> log123;
int queue_emp = 0;
int drop_pak = 0;
int total_packet_gen = 0;
queue<int> total_queue_size;;

double random_gen(double i, double convertion){
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    std::uniform_real_distribution<double> unif(0, 1);
    // ready to generate random numbers
    double uniform = unif(rng);
    //std::cout << uniform << "\n";
    double r = (-1)/i*log(1-uniform);
    double result = r * convertion;
    //cout << result << "\n";
    return result;
    
}



int packet_gen(double time, double size, double arrival){
    if(!level1.empty()){
        if(level1.front().starttime <= time){
            Packet abc;
            abc.length = size;
            abc.starttime = arrival;
            //cout << arrival << "\n";
            Packet tmp = level1.front();
            buffer.push(tmp);
            level1.pop();
            level1.push(abc);
        }
    }
    return 0;
}

int packet_gen_MDK1(double time, double size, double arrival, double buffer_size){
    if(!level1.empty()){
        if(level1.front().starttime <= time){
            Packet abc;
            abc.length = size;
            abc.starttime = arrival;
            //cout << arrival << "\n";
            Packet tmp = level1.front();
            if (buffer.size() < buffer_size){
            buffer.push(tmp);
            level1.pop();
            level1.push(abc);
                total_packet_gen++;
            }
            else{
               // cout << "doped" ;
                drop_pak++;
            }
        }
    }
    return 0;
}

/*
int level1_detect(double time){
    if (!level1.empty()){
    if (level1.front().starttime <= time){
        Packet tmp = level1.front();
        buffer.push(tmp);
        level1.pop();
    }
    }
    return 0;
}
 */
int buffer_detect(double time, double transmission_rate){
    if(buffer.empty()){
        queue_emp ++;
    }
    total_queue_size.push(double(buffer.size()));
    if (server.size()==0){
        if(!buffer.empty()){
        Packet tmp = buffer.front();
        tmp.servertime = time;
        tmp.endtime = time+((tmp.length/transmission_rate)*1000000);
        server.push(tmp);
        buffer.pop();
        }
        else{
        }
    }
    return 0;
}
int server_detect(double time){
    if (!server.empty()){
    if (server.front().endtime <= time){
        Packet tmp = server.front();
        log123.push(tmp);
        server.pop();
    }
    }
    else{
    }
    return 0;
}
int main(int argc, char* argv[]){
    double i;
    cout << "Please enter simulation type: 0 for (M/D/K) 1 for (M/D/K/1): ";
    cin >> i;
    if(i==0){
    cout << "-------M/D/K Mode------ ";
    cout << "Please enter transmission_rate: ";
    cin >> i;
    //double transmission_rate = 1000000;
    double transmission_rate = i;
    cout << "Please enter packet size: ";
    cin >> i;
    int packet_size = i;
    cout << "Please enter namda: ";
    cin >> i;
    double namda = i;
    cout << "Please enter TICKS: ";
    cin >> i;
    int time = i;
    //int packet_size = 2000;
    // insert code here...
    level1.empty();
    buffer.empty();
    server.empty();
    double firstone = random_gen(namda,1000000);
    Packet abc;
    abc.length = packet_size;
    abc.starttime = firstone;
    level1.push(abc);
    //packet_gen(0,packet_size,firstone);
    for (int i = 0 ;i<=time ;i++){
        double j = random_gen(namda,1000000) + i;
        //cout << j << "\n";
        packet_gen(i,packet_size,j);
        //level1_detect(i);
        buffer_detect(i,transmission_rate);
        server_detect(i);
        //long size = log123.size();
        //cout << size << "\n";
    }
    cout << "em:" << queue_emp << "\n";
    double ulti_rate = double(queue_emp)/(time)*100;
    cout << "uti:" << ulti_rate << "\n";
    long total_packet = log123.size();
    cout << "total_Packet:" << total_packet << "\n";
    double total_time = 0;
    while (!log123.empty())
    {
        total_time += (log123.front().endtime - log123.front().starttime);
        //cout  << total_time << "\n";
        log123.pop();
    }
    double E_T = total_time/total_packet;
    cout << "E[T]:" << E_T << "\n";
    double total_size = 0;
    double sample_time = total_queue_size.size();
    cout  << sample_time << "\n";
    while (!total_queue_size.empty())
    {
        total_size = total_size + total_queue_size.front();
       // cout  << total_size << "\n";
        total_queue_size.pop();
    }
     //cout  << total_size << "\n";
    double E_N = total_size/sample_time;
    cout << "E[N]:" << E_N << "\n";
    long level1_left = level1.size();
    cout << "level1_left:" << level1_left << "\n";
    }
    else{
        cout << "-------M/D/K/1 Mode------ ";
        cout << "Please enter buffer size: ";
        cin >> i;
        double buffer_size = i;
        //double transmission_rate = 1000000;
        //double transmission_rate = i;
        cout << "Please enter transmission_rate: ";
        cin >> i;
        //double transmission_rate = 1000000;
        double transmission_rate = i;
        cout << "Please enter packet size: ";
        cin >> i;
        int packet_size = i;
        cout << "Please enter namda: ";
        cin >> i;
        double namda = i;
        cout << "Please enter TICKS: ";
        cin >> i;
        int time = i;
        for (int runtime = 0; runtime <=5; runtime++){
        level1.empty();
        buffer.empty();
        server.empty();
        log123.empty();
        queue_emp = 0;
         drop_pak = 0;
         total_packet_gen = 0;
       total_queue_size.empty();
        double firstone = random_gen(namda,1000000);
            cout << firstone;
        Packet abc;
        abc.length = packet_size;
        abc.starttime = firstone;
        level1.push(abc);
        //packet_gen(0,packet_size,firstone);
        for (int i = 0 ;i<=time ;i++){
            double j = random_gen(namda,1000000) + i;
            //cout << j << "\n";
            packet_gen_MDK1(i,packet_size,j,buffer_size);
            //level1_detect(i);
            buffer_detect(i,transmission_rate);
            server_detect(i);
            //long size = log123.size();
            //cout << size << "\n";
        }
        cout << "em:" << queue_emp << "\n";
        double ulti_rate = double(queue_emp)/(time)*100;
        cout << "uti:" << ulti_rate << "\n";
        long total_packet = log123.size();
        cout << "total_Packet:" << total_packet << "\n";
        double total_time = 0;
        while (!log123.empty())
        {
            total_time += (log123.front().endtime - log123.front().starttime);
            //cout  << total_time << "\n";
            log123.pop();
        }
        double E_T = total_time/total_packet;
        cout << "E[T]:" << E_T << "\n";
        double total_size = 0;
        double sample_time = total_queue_size.size();
        cout  << sample_time << "\n";
        while (!total_queue_size.empty())
        {
            total_size = total_size + total_queue_size.front();
            // cout  << total_size << "\n";
            total_queue_size.pop();
        }
        cout  << total_size << "\n";
        double E_N = total_size/sample_time;
        cout << "E[N]:" << E_N << "\n";
        double ploss = double(drop_pak)/double(total_packet_gen+drop_pak);
        cout << "Ploss:" << ploss << "\n";
        long level1_left = level1.size();
        cout << "level1_left:" << level1_left << "\n";
        }
    }
    return 0;
}

