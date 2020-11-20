// Name - shubham gupta
//ID -  a1787223
//Semester - 1 
// distributed databases and management 
//Assignment 2 - 
// acknowledgement -- a lot of this has been done through the discussions forums from Guanhua Wang and Dr. Wei
//Thank you to them and the tutors for helping me getting through this subject...
// acknowledgement - some  coding references have been taken from public forums like github and stackflow. 
//-------------------------------------------------
// calling different packages for our program
#include <math.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>
#include <iomanip>
using namespace std;
//-------------------------------------------------
//creating a class of type packet with flow-key and arrival time and lenght of the packet 
class packet{
public:
    string flow_key;
    int arrival_time;
    int packet_length;
};
//Some reference has been taken from online sources for making these class functions 
// namely StackOverflow, Geeks for Geeks
class cluster{
public:
    int modiodIdx;
    vector<int> nesrestPoints;
    void addPoint(int pointIdx)
    {
        nesrestPoints.push_back(pointIdx);
    }
    void clearPoints()
    {
        nesrestPoints.clear();
    }
    void printPoints()
    {
        int i =0; 
        while( i< nesrestPoints.size() ) 
        {
            cout<<nesrestPoints[i]<<" ";
            i++;
        }
        cout<<endl;
    }
    bool isBelong (int pointIdx)
    {    
        int j=0 ; 
        while( j< nesrestPoints.size() )
        {
            if(pointIdx==nesrestPoints[j])
            {
                return true;
            }
            j++;    
        }
        return false;
    }
};
//-------------------------------------------------
vector<vector<double>>  map_building(vector<vector<double>> flowResult)
{// building the map required 
    vector<vector<double>> map;
    int flow = 0 ; 
    while( flow < flowResult.size() )
    {
        vector<double> entity;
        for (int y=0; y<flowResult.size(); y++)
        {
            entity.push_back(abs(flowResult[flow][0]-flowResult[y][0])+abs(flowResult[flow][1]-flowResult[y][1]));
        }
        map.push_back(entity);
        flow = flow + 1 ;
    }
    return map;
}
//--------------------------------------------------------------------------------------------------------------
int find_nearest_modoid_map(int pointIdx,vector<vector<double>> map, vector<int> modoidIdx)
{
    double min_distance = 9999999.0;
    double distance;
    int near_map_temp = 0; 
    int min_indx;
    while( near_map_temp < modoidIdx.size() )
    {
        distance = map[pointIdx][modoidIdx[near_map_temp]];
        if (distance<min_distance)
        {
            min_distance = distance;
            min_indx=modoidIdx[near_map_temp];
        }
        near_map_temp = near_map_temp + 1 ; 
    }
    return min_indx;
}
//--------------------------------------------------------------------------------------------------------------
void assigning_points(vector<vector<double>>& flowResult, vector<int>& modoidIdx, vector<cluster>& clusters)
{// assigning the point 
    int cluster = 0 ; 
    while( cluster < clusters.size())
    {
        clusters[cluster].clearPoints();
        clusters[cluster].modiodIdx=modoidIdx[cluster];
        cluster = cluster + 1 ; 
    }
    int flow_result = 0; 
    while( flow_result < flowResult.size() )
    {
        int minimum_index;
        minimum_index=find_nearest_modoid_map(flow_result, flowResult, modoidIdx);
        if (minimum_index!=-1)
        {
            for(int h=0;h<clusters.size();h++)
            {
                if(clusters[h].modiodIdx==minimum_index)
                {
                    clusters[h].addPoint(flow_result);
                    break;
                }
            }
        }
        flow_result = flow_result + 1 ; 
    }
}
//--------------------------------------------------------------------------------------------------------------
void remove_flow(vector<int> resultIdx, vector<packet>& packets)
{// removing the flow from the packets 
    vector <packet>::iterator iteration;
    int temp = resultIdx.size()-1;
    while( temp >=0 )
    {
         iteration = packets.begin()+resultIdx[temp];
         packets.erase(iteration);
         iteration = packets.begin();
         temp = temp - 1 ; 
    }
}
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
double calculate_tc(int previousModoidIdx, int potientalModoidIdx, vector<cluster>& clusters,  vector<vector<double>>& map, vector<int>& newModiodIdx,vector<int>& oldModiodIdx)
{// calculating the TOTAL SWAPPING COST ERROR  
   //place holder variable 
    int place_holder = 0 ; 
    //calculating tc 
    double tc_ih=0;
    while( place_holder < map.size() )
    {   
        //variable of type boolean for true false usage i.e boolean 
        bool belong_potential=false;
        bool belong_previous =false;
        int new_cluster_index;
        int previous_cluster;
        double cost=0;
        //finding the nearest modoid map for cluster for tc calculation
        previous_cluster=find_nearest_modoid_map(place_holder, map, oldModiodIdx);

        //conditional operator on previous_cluster
        (previous_cluster == previousModoidIdx ? belong_previous = true : belong_previous=false);
        //new cluster index and finding nearest modoid map
        new_cluster_index=find_nearest_modoid_map(place_holder, map, newModiodIdx);
        //condition operator on new cluster index 
        (new_cluster_index == potientalModoidIdx ? belong_potential = true : belong_potential=false);        
       
        if (belong_previous==true&&belong_potential==true)
        {   
            double temp1 =0 , temp2 = 0; 
            temp1 = map[place_holder][potientalModoidIdx];
            temp2 = map[place_holder][previousModoidIdx];
            cost = temp1 - temp2 ;
        }
        else if (belong_previous==true&&belong_potential==false)
        {
            double temp3 =0 , temp4 = 0; 
            temp3 = map[place_holder][new_cluster_index];
            temp4 = map[place_holder][previousModoidIdx];
            cost = temp3 - temp4 ;
        }
        else if (belong_previous==false&&belong_potential==true)
        {
            double temp5 =0 , temp6 = 0; 
            temp5 = map[place_holder][potientalModoidIdx];
            temp6 = map[place_holder][previous_cluster];
            cost = temp5 - temp6 ;
        }
        else
        {
            cost=0;
        }
        tc_ih = tc_ih + cost;
        place_holder = place_holder + 1;
    }
    return tc_ih;
}
//--------------------------------------------------------------------------------------------------------------
double calculating_error(vector<cluster>& clusters, vector<vector<double>>& map)
{ // calculating the error   
    int new_temp = 0 ; 
    double calculated_error=0;

    while( new_temp < clusters.size() )
    {
        for (int j=0; j<clusters[new_temp].nesrestPoints.size(); j++) 
        {
            calculated_error = calculated_error + map[clusters[new_temp].modiodIdx][ clusters[new_temp].nesrestPoints[j]];
        }
        new_temp = new_temp + 1 ; 
    }
    return calculated_error;
}
//--------------------------------------------------------------------------------------------------------------
vector<int> finding_flow(string key,vector<packet> packets)
{//finding the flow of packets 
    vector<int> result_index;
    int q=0;
    while( q < packets.size() )
    {
        if(packets[q].flow_key==key)
        {
            result_index.push_back(q);
        }
        q = q+ 1 ; 
    }
    return result_index;
}
//--------------------------------------------------------------------------------------------------------------
bool is_medoid(vector<int> medoid_idx, int key)
{   //
    int index = 0; 
    while(index < medoid_idx.size() )
    {
         if(key==medoid_idx[index])
        {
            return true;
        }
        index = index + 1 ; 
    }
    return false;
}
//--------------------------------------------------------------------------------------------------------------
//The main function
int main(int argc, const char * argv[]) {
//* Taking input directly as an argumnet as given in the handout 
    string network_packets_txt=argv[1];
   // load k and init medoids index
    string initial_medoids_txt=argv[2];
    // some reference has been taken on how to analyse the input arguments using vectors and fstreams and ifstreams  
    // like to acknowledge work on github, stackoverflow.
    //initializing a vector 'packets' of type packet,
    //--------------------------------------------------------------------------------------------------------------
    //Following code is just for reading the FILE1 (Netowrk packets) and FILE 2  (Initial_medoids_txt)
    vector<packet> packets;
    //initializing a vector 'flow_results' of type double
    vector<vector<double>> flow_result;
    //vector type int
    vector<int> flow_index;
    int medoid_num;
    string data;
    string new_data; 
    string place_holder_data ; 
    vector<int> medoid_index;
    //opeining network_packets_txt
    ifstream infile;
    infile.open(network_packets_txt,ios::in);
    getline(infile,data);
    while (getline(infile, data)) 
    {
        string tmp;
        vector<string> vec;
        stringstream ss(data);
        while (ss>>tmp) 
        {// storing the contents as a vector 
            vec.push_back(tmp);
        }
        packet pac;
        pac.flow_key=vec[0]+vec[1]+vec[2]+vec[3]+vec[4];
        pac.arrival_time=std::stoi(vec[5]);
        pac.packet_length=std::stoi(vec[6]);
        packets.push_back(pac);
    }
    infile.close();
    //closing the network_packets_txt file 
//--------------------------------------------------------------------------------------------------------------    
    // analyzing the contents of network_packets_txt file 
    while (packets.size()!=0) {     
        flow_index=finding_flow(packets[0].flow_key,packets);
        if (flow_index.size()<=1) 
        {
            remove_flow(flow_index, packets);
        }
        else
        {
            vector<double> result;
            double length=0;
            int g =flow_index.size()-1 ; 
            double time=0;
            while( g > 0 )
            {
                length=length+packets[flow_index[g]].packet_length;
                time=time+packets[flow_index[g]].arrival_time-packets[flow_index[g-1]].arrival_time;
                g = g - 1 ; 
            }
            length=length+packets[0].packet_length;
            length=length/flow_index.size();
            time=time/(flow_index.size()-1);
            result.push_back(time);
            result.push_back(length);
            flow_result.push_back(result);
            remove_flow(flow_index, packets);            
        }
    }
//--------------------------------------------------------------------------------------------------------------
    //Output 1 
    ofstream fout;
    fout.open ("Flow.txt", ios::out | ios::trunc);// creating and opening a file name Flow.txt
    fout.flags(ios::fixed);
//--------------------------------------------------------------------------------------------------------------
    int precision_interation = 0 ; 
    while( precision_interation < flow_result.size() )
    {   // set precision used as told by professor in discussions 
        fout<<precision_interation<<" ";
        // for printing indexes 
        fout<<setprecision(2)<<flow_result[precision_interation][0];
        // set precision is basically rounding out to the 2nd decical value 
        fout<<" ";// space for next value 
        fout<<setprecision(2)<<flow_result[precision_interation][1]<<endl;//second value 
        precision_interation = precision_interation + 1 ; 
    }
    fout.close();//closing the file 
//-----------------------------------------------------------
    ifstream infile1;
    infile1.open(initial_medoids_txt,ios::in);
    getline(infile1,data);
    medoid_num=stoi(data);
    while (getline(infile1, data)) 
    {
        int num;
        stringstream ss(data);
        while (ss>>num) 
        {
            medoid_index.push_back(num);
        }
    }
    infile1.close();
//---------------------------------------------------
    vector<cluster> clusters;
    int cl_id = 0 ; 
    while ( cl_id < medoid_num)
    {
        cluster clust;
        cl_id = cl_id + 1 ; 
        clusters.push_back(clust);
    }
//---------------------------------------------------    
    vector<vector<double>> map=map_building(flow_result);
    bool change=true;
    double error;
    while(change)
    {
        assigning_points(map, medoid_index, clusters);
        bool is_change=false;
        int flow = 0 ; 
        while(flow < flow_result.size() )
        {       // checking if its a medoid 
             if (is_medoid(medoid_index, flow)==false) 
            {
                bool is_exchanging=false;
                for (int g=0; g<medoid_num; g++) 
                {
                    vector<int> newModiodIdx(medoid_index);
                    newModiodIdx[g]=flow;
                    double total_cost;
                    total_cost=calculate_tc(medoid_index[g], newModiodIdx[g], clusters, map,newModiodIdx,medoid_index);
                    if (total_cost<0)
                    {// used from discussions on my uni webpage by prof Guanhua Wang and Dr. Wei.
                        is_exchanging=true;
                        medoid_index=newModiodIdx;
                        is_change=true;
                        break;
                    }
                }
                if(is_exchanging)
                {
                    break;
                }
            }
            flow = flow + 1 ;      
        }
        //conditional operator for boolean variables and to write efficient code.. 
        (is_change == true? change=true : change=false);
    }
//--------------------------------------------------- 
    // OUTPUT 2 
    error=calculating_error(clusters, map);
    //fout_kmc is just a fstream variable to make a file and write on it. 
    ofstream fout_kmc;
    //making and opening a KMedoidsClusters.txt file 
    fout_kmc.open ("KMedoidsClusters.txt",ios::out | ios::trunc);
    fout_kmc.flags(ios::fixed);
    //again set precision of 2 is used to round of decimal places to 2nd position. Used as asked in the assignment 
    fout_kmc<<setprecision(2)<<error<<endl;
//---------------------------------------------------
    int kmc = 0; 
    while( kmc < medoid_index.size() )
    {
        fout_kmc<<setprecision(2)<<medoid_index[kmc]<<" ";
        kmc = kmc + 1; 
    }
    fout_kmc<<endl;
//---------------------------------------------------  
    int cluster_variable = 0 ; 
    while( cluster_variable < clusters.size())
    {
        for (int j=0; j<clusters[cluster_variable].nesrestPoints.size(); j++)
        {   
            //again set precision is used here for final output 
            fout_kmc<<setprecision(2)<<clusters[cluster_variable].nesrestPoints[j]<<" ";//space given for right structure for the output 
        }
        cluster_variable = cluster_variable + 1; 
        fout_kmc<<endl;
    }   
    fout_kmc.close();
    //closing the file 
//---------------------------------------------------    
    return 0;
}