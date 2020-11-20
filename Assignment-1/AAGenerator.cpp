// Name - shubham gupta
//ID -  a1787223
//Semester - 1 
// distributed databases and management 
// acknowledgement - some  coding references have been taken from public forums like github and stackflow. 
//-------------------------------------------------
// calling different packages for our program
#include <iostream>
#include <vector>
#include <sstream>
#include <utility>
#include <fstream>
#include <cmath>
#include <string>
using namespace std;
//-------------------------------------------------
//defining the attribute class of type string 
class attribute {
public:
    attribute(string label, string name) {
        this->label = move(label);
        this->name = move(name);
    }
    string name;
    string label;
};
//-------------------------------------------------
//defining the query class of type string 
class query {
public:
    query(string label, string query) {
        this->label = move(label);
        this->query_s = move(query);
    }
    string query_s;
    string label;
};
//-------------------------------------------------
void removing_bsR_carriege_return(string& line) {
    while (*line.rbegin() == '\r')     {
        line.erase(line.length() - 1);
    }
}
//-------------------------------------------------
vector<attribute> read_attributes(const string &attribute_file_name) {
 //here we are getting the relation file in the form 
   // THE FOLLOWING CODE TO READ SQL QUERIES HAS BEEN DIRECTLY TAKEN FROM ONLINE SOURCES
 /* Label Name
    A1 PNO
    A2 PNAME
    A3 BUDGET
    A4 LOC
  */
    //initializing the class attribute as attributes
    vector<attribute> attributes;
    ifstream attribute_file(attribute_file_name);
    string line;
    while (getline(attribute_file, line)) {
        removing_bsR_carriege_return(line);
        istringstream iss(line);
        string label; string name;
        getline(iss, label, ' ');
        getline(iss, name, ' ');
        attributes.emplace_back(label, name);
    }
    attributes.erase(attributes.begin());
    return attributes;
    //returning the attributes 
}
//-------------------------------------------------
vector<query> read_queries(const string &queries_file_name) {
    // THE FOLLOWING CODE TO READ SQL QUERIES HAS BEEN DIRECTLY TAKEN FROM ONLINE SOURCES 
    //queries are of the form 
    /*
    q1: SELECT BUDGET FROM PROJ WHERE PNO=Value
    q2: SELECT PNAME, BUDGET FROM PROJ  
    q3: SELECT PNAME FROM PROJ WHERE LOC=Value
    q4: SELECT SUM(BUDGET) FROM PROJ WHERE LOC=Value
    */ //just for reference while coding 
    vector<query> queries;
    ifstream queries_file(queries_file_name);
    string line;
    while (getline(queries_file, line)) {
        removing_bsR_carriege_return(line);
        istringstream iss(line);
        string label;
        string query_s;
        getline(iss, label, ' ');
        getline(iss, query_s);
        queries.emplace_back(label, query_s);
    }
    return queries;
    //returning the queries after reading them
}
//-------------------------------------------------
vector<vector<int>> reading_access_frequency(const string &freq_file_name, unsigned long number_of_queries) {
    // THE FOLLOWING CODE TO READ ACCESS FREQUENCY HAS BEEN DIRECTLY TAKEN FROM ONLINE SOURCES 
    /*
          S1 S2 S3
        q1 15 20 10
        q2 5 0 0
        q3 25 25 25
        q4 5 0 0
     */
    vector<vector<int>> access;
    ifstream access_file(freq_file_name);
    string line;
    int queries = -1;
    int sites = 0;
    while (getline(access_file, line)) {
        removing_bsR_carriege_return(line);
        istringstream iss(line);
        string label; string acc_f_line;
        getline(iss, label, ' ');
        getline(iss, acc_f_line);
        //size_t is an unsigned integer data type 
        //which can assign only 0 and greater than 0 integer values
        size_t current_one;
        size_t next_one = -1;
        int site = 0;
        do {
            current_one = next_one + 1;
            next_one = acc_f_line.find_first_of(' ', current_one);
            if (queries < 0) {
                sites++;
                if (next_one == string::npos) {
                    access = vector<vector<int>>(number_of_queries, vector<int>(sites, 0));
                }
            } else {
                access[queries][site] = stoi(acc_f_line.substr(current_one, next_one - current_one));
                site++;
            }
            if (next_one == string::npos) {
                queries++;
            }
        }
        while (next_one != string::npos);
    }
    return access;
}
//-------------------------------------------------
//calculating affinity from Otsuka-Ochiai coefficient (https://en.wikipedia.org/wiki/Yanosuke Otsuka)
vector<vector<int>> affinity_matrix_calculation(vector<vector<int>> usage, vector<vector<int>> access, unsigned long number_of_attributes, unsigned long number_of_queries) {
    vector<vector<int>> affinity = vector<vector<int>>(number_of_attributes, vector<int>(number_of_attributes, 0));
    int execution = 1;
    /// The following code implemented directly from book .........
    int p =0; 
    while(p < number_of_attributes) 
    {
        for (int q = 0; q < number_of_attributes; q++) 
        { 
            affinity[p][q] = 0;
            int total_sum[number_of_queries];
            for (int r = 0; r < number_of_queries; r++)
            {
                total_sum[r] = 0;
                for (int s = 0; s < access[r].size(); s++) 
                { 
                    total_sum[r] += access[r][s];
                }
            }
            float numerator_one = 0,denominator_one = 0,denominator_two = 0;
            for (int t = 0; t < number_of_queries; t++) 
            { 
                denominator_one = denominator_one + (usage[t][p] * total_sum[t]) ;
                numerator_one = numerator_one + (usage[t][p] * total_sum[t]) * (usage[t][q] * total_sum[t]) ;
                denominator_two = denominator_two + (usage[t][q] * total_sum[t]) ;
            }
            //The static_cast<> operator can be used for operations such as: 
            //converting a pointer of a base class to a pointer of a non-virtual derived class; 
            //converting numeric data types such as enums to ints or ints to floats
            affinity[p][q] = static_cast<int>(ceil(numerator_one / sqrt(denominator_one * denominator_two)));
                if (affinity[p][q] < 0) 
                { 
                    affinity[p][q] = 0;
                }
        }
        p++;
    }
    return affinity;
    //returning the final affinity matrix which will also be passed as input for CA matrix calculation.
}
//-------------------------------------------------
vector<vector<int>> populating_usage(vector<attribute> attributes, vector<query> queries) {
    vector<vector<int>> usage = vector<vector<int>>(queries.size(), vector<int>(attributes.size(), 0));
    //calculating the populating usage ....................
    // taken from book , 
    
    int f =0 ;
    while( f < queries.size()){
        for (int j = 0; j < attributes.size(); j++) {
            (queries[f].query_s.find(attributes[j].name) != std::string::npos &&
                    !isalnum(queries[f].query_s.at(queries[f].query_s.find(attributes[j].name) - 1))) ? usage[f][j] = 1: usage[f][j] = 0;
        }
        f++;
    }
    return usage;
}
//-------------------------------------------------
int main (int argc, char *argv[]) {
  // intializing variable for attributes , queries and acccess frequenfies files 
    string access_frequency_matrix_file;
    string attribute_file;
    string queries_file;
    //taking the input argugement in the respective variables...
    // order taken as given in the assignment
    attribute_file = argv[1];
    queries_file = argv[2];
    access_frequency_matrix_file = argv[3];
    // calling the readAttributes function which reads the relation given such as 
    /* Label Name
    A1 PNO
    A2 PNAME
    A3 BUDGET
    A4 LOC
  */
    vector<attribute> attributes = read_attributes(attribute_file);
//-----------------------------------------------------------------
    //calling the read queries function to read the queries which are inputted 
    //they are of the type 
    /*
    q1: SELECT BUDGET FROM PROJ WHERE PNO=Value
    q2: SELECT PNAME, BUDGET FROM PROJ  
    q3: SELECT PNAME FROM PROJ WHERE LOC=Value
    q4: SELECT SUM(BUDGET) FROM PROJ WHERE LOC=Value
    */
    vector<query> queries = read_queries(queries_file);
//-----------------------------------------------------------------
    //reading access frequency matrix 
    // it is of the type 
    /*
        S1 S2 S3
        q1 15 20 10
        q2 5 0 0
        q3 25 25 25
        q4 5 0 0
     */
    vector<vector<int>> access = reading_access_frequency(access_frequency_matrix_file, queries.size());
//-----------------------------------------------------------------
    vector<vector<int>> usage = populating_usage(attributes, queries);
//-----------------------------------------------------------------
    vector<vector<int>> affinity = affinity_matrix_calculation(usage, access, attributes.size(), queries.size());
    //calculating the affinity matrix 
//-----------------------------------------------------------------
    //printing out the final affinity matrix 
    int p =0; 
    while (p < affinity.size()){
        for (int q = 0; q < affinity[p].size(); q++) {
            cout << affinity[p][q] << ' ';
        }
        p++;
        cout << endl;
    }
//-----------------------------------------------------------------
}