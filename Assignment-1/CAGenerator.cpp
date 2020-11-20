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
void removing_bsR_carriege_return(std::string& line) {
    // THE FOLLOWING CODE TO READ SQL QUERIES HAS BEEN DIRECTLY TAKEN FROM ONLINE SOURCES 
    while (*line.rbegin() == '\r')     {
        line.erase(line.length() - 1);
    }
}
//Affnity matrix is of the form //
/*
45 0 41 0
0 71 1 71
41 1 38 1
0 71 1 71
*/
// we have saved this output from AAGenerator and taking it as a input for CAGenerator...........
// this function ---->reading_affinity_matrix_from_aa reads the contents of AAmatrix 
//Some of the code has been taken from online forums and necessary changes have been made (NOT COPIED) .... 
vector<vector<int>> reading_affinity_matrix_from_aa(const std::string &affinity_matrix_file) {
    // THE FOLLOWING CODE TO READ SQL QUERIES HAS BEEN DIRECTLY TAKEN FROM ONLINE SOURCES 
    vector<vector<int>> affinity;
    ifstream affinity_file(affinity_matrix_file);
    string line;
    // calculating number of attributes.
    int number_of_attributes = 0;
    while (getline(affinity_file, line))
        ++number_of_attributes;
    // Reset file
    affinity_file.clear();                 // clear fail and eof bits
    affinity_file.seekg(0, std::ios::beg); // back to the start!
    affinity = vector<vector<int>>(number_of_attributes, vector<int>(number_of_attributes, 0));
    int row = 0;
    while (getline(affinity_file, line)) {
        removing_bsR_carriege_return(line);
        istringstream iss(line);
        string affinity_line;
        getline(iss, affinity_line);
        //size_t is an unsigned integer data type which can assign only 0 and greater than 0 integer values
        size_t current;
        size_t next = -1;
        int column = 0;
        do {
            current = next + 1;
            next = affinity_line.find_first_of(' ', current);
            if (next != string::npos) {
                affinity[row][column] = stoi(affinity_line.substr(current, next - current));
            }
            column++;
        } while (next != string::npos);
        row++;
    }
    return affinity;
}
//-------------------------------------------------
int calculating_bond (int attribute1, int attribute2, int index, const vector<vector<int>> &affinity, vector<int> &order) {
    if (attribute1 < 0 || attribute2 < 0) {
        return 0;
    }
    if (attribute1 > index || attribute2 > index) {
        return 0;
    }
    int sum = 0,z =0, place_holder = 0 ; 
    while (z < affinity[0].size()){
        //algorithm taken directly from the book 
        place_holder = affinity[z][order[attribute1]] * affinity[z][order[attribute2]];
        sum = sum + place_holder ; 
        z++;
        }
        return sum; 
}
//-------------------------------------------------
int calculating_contribution (int attribute1, int attribute2, int attribute3, const vector<vector<int>> &affinity, vector<int> &order) {
    int index = attribute2;
    // algorithm copied directly from the book 
    int sum1=0,sum2=0,sum3 =0 ; 
    sum1 = 2*calculating_bond(attribute1, attribute2, index, affinity, order);
    sum2 = 2*calculating_bond(attribute2, attribute3, index, affinity, order);
    sum3 = 2*calculating_bond(attribute1, attribute3, index, affinity, order);
    return sum1 + sum2 - sum3 ; 
}
//-------------------------------------------------
vector<vector<int>> calculating_clustered_affinity(vector<vector<int>> affinity) {
    int number_of_attributes = affinity[0].size();
    vector<vector<int>> cluster_affinity = vector<vector<int>>(number_of_attributes, vector<int>(number_of_attributes, 0));
//-------------------------------------------------
    vector<int> order; 
    int ord =0 ; 
    while ( ord < number_of_attributes ){
        order.push_back(ord);
        ord++;
    }
//-------------------------------------------------
    cluster_affinity[0] = affinity[0];
    cluster_affinity[1] = affinity[1];
    int index = 2;
//-------------------------------------------------
    while (index < number_of_attributes) { // REFERENCE FROM BOOK AND ONLINE SOURCES 
        int cont = 0;
        int loc = -1;
        int max = -1;
        int k = 0 ; 
        while(k < index){
            cont = calculating_contribution(k-1, index, k, affinity, order);
            if (cont > max) {
                max = cont;
                loc = k;
            }
            k++;
        }
        cont = calculating_contribution(index - 1, index, index + 1, affinity, order);
        if (cont > max) {
            loc = index;
        }
        for (int j = index; j > loc; j--) {
            cluster_affinity[j] = cluster_affinity[j-1];
            order[j] = order[j-1];
        }
        cluster_affinity[loc] = affinity[index];
        order[loc] = index;
        index++;
    }
//-------------------------------------------------
    // Shuffle the given columns
    int k =0 ;
    while ( k < number_of_attributes){
        for (int g =0; g < number_of_attributes; g++) {
            cluster_affinity[k][g] = affinity[order[k]][order[g]];
        }
        k++;
    }
    int j = 0; 
    while(j<number_of_attributes){
        cout << "A" << order[j] + 1 << "\t"; 
        j++;
    }
    cout << endl;
    //  Print the final Clustered Affinity Matrix in the required format.........
    int p =0;
    while( p < cluster_affinity.size()){
        cout << "A" << order[p] + 1 << "\t";
        for (int q =0 ; q < cluster_affinity[p].size(); q++){
            cout << cluster_affinity[p][q] << ' '; 
        }
        p++;
        cout << endl;
    }
    return cluster_affinity;
}
//-------------------------------------------------
//main function 
int main (int argc, char *argv[]) {
    // variable for storing affinity matrix 
    string affinity_matrix_file;
    //reading the input which is affinity matrix file
    // we are taking argv 1 as our direct input assuming affinity matrix is entered.....
    affinity_matrix_file = argv[1];
    // calling our function to read the affinity matrix from AAGenerator.
    vector<vector<int>> affinity_from_AAGenerator = reading_affinity_matrix_from_aa(affinity_matrix_file);
    //calling our function to calculate clustered affinity matrix from affinity matrix 
    vector<vector<int>> cluster_affinity = calculating_clustered_affinity(affinity_from_AAGenerator);
}
//-------------------------------------------------