//
//  main.cpp
//  Genetic Program Symbolic Regression
//
//  Created by Albert Go on 10/21/21.
//
//  This program implements maintaining genetic diversity through determinstic crowding and creates diversity through Hierarchical Fair Competition.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include<time.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstdlib>
#include <random>
using namespace std;

struct Equation{
    double error;
    vector<string> function;
    vector<int> locations;
};

vector<string> random_equation(vector<string> &operators, vector<string> &terminals);
double get_error(vector<double> &coord, int k, vector<string> &equation, vector<string> &operators, vector<string> &terminals);
double help_get_error(vector<double> &coord, int k, vector<string> &equation, vector<string> &operators, vector<string> &terminals);
vector<vector<double> > get_new_coords(vector<string> &equation, vector<vector<double>> &coords, vector<string> &operators, vector<string> &terminals);
vector<Equation> get_population(vector<vector<double> > &coordinates, vector<string> &operators, vector<string> &terminals);
vector<Equation> replenish_population(vector<vector<double> > &coordinates, vector<string> &operators, vector<string> &terminals);
void breed(vector<vector<double> > &coordinates, vector<string> &operations, vector<string> &terminals, Equation parent1, Equation parent2, vector<Equation> &population, vector<int> &cross1, vector<int> &cross2, vector<Equation> &new_population);
Equation mutate(vector<vector<double> > &coordinates, vector<string> &operations, vector<string> &terminals, vector<string> function, double &prev_error, vector<int> &locs);
bool compareByError(const Equation &func1, const Equation &func2);

int main(int argc, const char * argv[]) {
    // insert code here...
    srand( static_cast<unsigned int>(time(0)));
    std::cout << "Finding equation...\n";
    ifstream ifs("/Users/albertgo/Documents/MECS_4510/PSet_2/data.txt");
    
    if (!ifs) {
       cerr << "Could not open the file.\n";
       exit(1);
    }
    
    vector<vector<double> > coordinates;
    double x, y;
    char comma;
    while (ifs >> x >> comma >> y) {
        vector<double> coord;
        coord.push_back(x);
        coord.push_back(y);
        coordinates.push_back(coord);
    }
    ifs.close();
    
    vector<string> operations{"+", "-", "/", "*", "sin", "cos"};
    vector<string> terminal{"val", "x", "x", "T"};
    
    vector<int> cross1{6, 13, 14, 27, 28, 29, 30, 55, 56, 57, 58, 59, 60, 61, 62, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254};
    vector<int> cross2{1, 3, 4, 7, 8, 9, 10, 15, 16, 17, 18, 19, 20, 21, 22, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190};
    vector<int> cross3{5, 11, 12, 23, 24, 25, 26, 47, 48, 49, 50, 51, 52, 53, 54, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222};
    
    vector<Equation> population = get_population(coordinates, operations, terminal);
    vector<Equation> minor_league;
    vector<Equation> major_league;
    vector<Equation> update_minor;
    vector<Equation> update_major;
    sort(population.begin(), population.end(), compareByError);
    cout << "Established population" << endl;
    int evaluations = 0;
    
    while(evaluations < 1000){
        vector<Equation> new_population;
        vector<Equation> new_minor;
        vector<Equation> new_major;
        for (int i=0; i<population.size(); i++){
            int parent2 = rand() % 100;
            if(parent2 == i){
                bool same = true;
                while(same){
                    parent2 = rand() % 100;
                    if(parent2 != i){
                        same = false;
                    }
                }
            }
            breed(coordinates, operations, terminal, population[i], population[parent2], population, cross2, cross2, new_population);
            if (i < minor_league.size() && minor_league.size() > 0){
                int parent2_m = rand() % 50;
                if(parent2_m == i){
                    bool same = true;
                    while(same){
                        parent2_m = rand() % 50;
                        if(parent2_m != i){
                            same = false;
                        }
                    }
                }
                breed(coordinates, operations, terminal, minor_league[i], minor_league[parent2_m], minor_league, cross2, cross2, new_minor);
            }
            if (i < major_league.size() && major_league.size() > 0){
                int parent2_m2 = rand() % 50;
                if(parent2_m2 == i){
                    bool same = true;
                    while(same){
                        parent2_m2 = rand() % 50;
                        if(parent2_m2 != i){
                            same = false;
                        }
                    }
                }
                breed(coordinates, operations, terminal, major_league[i], major_league[parent2_m2], major_league, cross2, cross2, new_major);
            }
        }
        population = new_population;
        minor_league = new_minor;
        major_league = new_major;
//        cout << population.size();
//        cout << ", ";
//        cout << minor_league.size();
//        cout << ", ";
//        cout << major_league.size() << endl;
        
        sort(population.begin(), population.end(), compareByError);
        sort(minor_league.begin(), minor_league.end(), compareByError);
        sort(major_league.begin(), major_league.end(), compareByError);
        
        evaluations += 1;
        
        if (evaluations % 50 == 0){
            update_minor = {population.begin(), population.begin()+100};
            if (minor_league.size() > 0){
                update_major = {minor_league.begin(), minor_league.begin()+50};
                minor_league.erase(minor_league.begin(), minor_league.begin()+50);
                if (minor_league.size() == 100){
                    minor_league.erase(minor_league.begin()+50, minor_league.end());
                }
            }
            minor_league.insert(minor_league.end(), update_minor.begin(), update_minor.end());
            
            if (major_league.size() > 0){
                major_league.erase(major_league.begin()+25, major_league.end());
            }
            major_league.insert(major_league.end(), update_major.begin(), update_major.end());
            
            if (major_league.size() > 0){
                cout << "Major League update: ";
                cout << major_league[0].error << endl;
                cout << "Updating population..." << endl;
                
                vector<string> curr_best = major_league[0].function;
                cout << "[";
                for(int j=0; j < curr_best.size(); j++){
                    cout << "\"";
                    cout << curr_best[j];
                    cout << "\"";
                    if (j < curr_best.size()-1){
                        cout << ", ";
                    }
                    else{
                        cout << "]" << endl;
                    }
                }
                if (major_league[0].error == 0){
                    cout << evaluations << endl;
                    break;
                }
            }
            else if (major_league.size() == 0 && minor_league.size() > 0){
                cout << "Minor League update: ";
                cout << minor_league[0].error << endl;
                cout << "Updating population..." << endl;
                
                vector<string> curr_best = minor_league[0].function;
                cout << "[";
                for(int j=0; j < curr_best.size(); j++){
                    cout << "\"";
                    cout << curr_best[j];
                    cout << "\"";
                    if (j < curr_best.size()-1){
                        cout << ", ";
                    }
                    else{
                        cout << "]" << endl;
                    }
                }
                if (minor_league[0].error == 0){
                    cout << evaluations << endl;
                    break;
                }
            }
            else{
                cout << "Status update: ";
                cout << population[0].error << endl;
                cout << "Updating population..." << endl;
                
                vector<string> curr_best = minor_league[0].function;
                cout << "[";
                for(int j=0; j < curr_best.size(); j++){
                    cout << "\"";
                    cout << curr_best[j];
                    cout << "\"";
                    if (j < curr_best.size()-1){
                        cout << ", ";
                    }
                    else{
                        cout << "]" << endl;
                    }
                }
                if (population[0].error == 0){
                    cout << evaluations << endl;
                    break;
                }
            }
            
            population.erase(population.begin(), population.begin()+100);
            
            vector<Equation> new_set = replenish_population(coordinates, operations, terminal);
            population.insert(population.end(), new_set.begin(), new_set.end());
            
            sort(population.begin(), population.end(), compareByError);
            sort(minor_league.begin(), minor_league.end(), compareByError);
            sort(major_league.begin(), major_league.end(), compareByError);
        }
    
        cout << evaluations << endl;
    }
    
    vector<string> best_eqn = population[0].function;
    double min_error = population[0].error;
    
    cout << "[";
    for(int i=0; i < best_eqn.size(); i++){
        cout << best_eqn[i];
        if (i < best_eqn.size()-1){
            cout << ", ";
        }
        else{
            cout << "]" << endl;
        }
    }
    
    vector<vector<double> > eqn_coords = get_new_coords(best_eqn, coordinates, operations, terminal);
    
    for (int j=0; j<eqn_coords.size(); j++){
        cout << eqn_coords[j][0];
        cout << ", ";
        cout << eqn_coords[j][1] << endl;
    }
    
    cout << min_error << endl;
    
    return 0;
}

vector<Equation> get_population(vector<vector<double> > &coordinates, vector<string> &operators, vector<string> &terminals){
    int iterations = 0;
    vector<Equation> population;
    
    while (iterations < 200) {
        vector<string> eqn = random_equation(operators, terminals);
        
        vector<int> l;
        for (int i=0; i<eqn.size(); i++){
            if (eqn[i] != "T" && eqn[i] != "x"){
                l.push_back(i);
            }
        }
        
        double total_error = 0;
        for (int j=0; j<coordinates.size(); j++){
            total_error += get_error(coordinates[j], 0, eqn, operators, terminals);
        }
        
        if (isnan(total_error)){
            total_error = 10000;
        }
        
        Equation func;
        func.error = total_error;
        func.function = eqn;
        func.locations = l;
        
        population.push_back(func);
        
        iterations += 1;
    }
    
    return population;
}

vector<Equation> replenish_population(vector<vector<double> > &coordinates, vector<string> &operators, vector<string> &terminals){
    int iterations = 0;
    vector<Equation> population;
    
    while (iterations < 100) {
        vector<string> eqn = random_equation(operators, terminals);
        
        vector<int> l;
        for (int i=0; i<eqn.size(); i++){
            if (eqn[i] != "T" && eqn[i] != "x"){
                l.push_back(i);
            }
        }
        
        double total_error = 0;
        for (int j=0; j<coordinates.size(); j++){
            total_error += get_error(coordinates[j], 0, eqn, operators, terminals);
        }
        
        if (isnan(total_error)){
            total_error = 10000;
        }
        
        Equation func;
        func.error = total_error;
        func.function = eqn;
        func.locations = l;
        
        population.push_back(func);
        
        iterations += 1;
    }
    
    return population;
}

vector<string> random_equation(vector<string> &operators, vector<string> &terminals){
    
    vector<string> func;
    int formula_size = 255;
    int half_size = (formula_size-1)/2;
    
    for (int i=0; i < formula_size; i++){
        int rand_num = rand() % 15;

        if (i == 0){
            int rand_num2 = rand() % 4;
            func.push_back(operators[rand_num2]);
        }
        else if ((i-1) >= 0 && (i-1) % 2 == 0){
            int parent = (i-1)/2;
            if (count(operators.begin(), operators.end(), func[parent])){
                if (i >= half_size){
                    int rand_num2 = rand() % 3;
                    if (terminals[rand_num2] == "val"){
                        int rand_num3 = rand() % 20;
                        string s = to_string(rand_num3+1);
                        func.push_back(s);
                    }
                    else{
                        func.push_back(terminals[rand_num2]);
                    }
                }
                else{
                    if (rand_num < 14){
                        int rand_num2 = rand() % 6;
                        func.push_back(operators[rand_num2]);
                    }
                    else{
                        int rand_num2 = rand() % 3;
                        if (terminals[rand_num2] == "val"){
                            int rand_num3 = rand() % 20;
                            string s = to_string(rand_num3+1);
                            func.push_back(s);
                        }
                        else{
                            func.push_back(terminals[rand_num2]);
                        }
                    }
                }
            }
            else{
                func.push_back(terminals[3]);
            }
        }
        else if ((i-2) >= 0 && (i-2) % 2 == 0){
            int parent = (i-2)/2;
            if (count(operators.begin(), operators.end(), func[parent])){
                if (func[parent] == "sin" || func[parent] == "cos"){
                    func.push_back(terminals[3]);
                }
                else{
                    if (i >= half_size){
                        int rand_num2 = rand() % 3;
                        if (terminals[rand_num2] == "val"){
                            int rand_num3 = rand() % 20;
                            string s = to_string(rand_num3+1);
                            func.push_back(s);
                        }
                        else{
                            func.push_back(terminals[rand_num2]);
                        }
                    }
                    else{
                        if (rand_num < 14){
                            int rand_num2 = rand() % 6;
                            func.push_back(operators[rand_num2]);
                        }
                        else{
                            int rand_num2 = rand() % 3;
                            if (terminals[rand_num2] == "val"){
                                int rand_num3 = rand() % 20;
                                string s = to_string(rand_num3+1);
                                func.push_back(s);
                            }
                            else{
                                func.push_back(terminals[rand_num2]);
                            }
                        }
                    }
                }
            }
            else{
                func.push_back(terminals[3]);
            }
        }
    }
    
    return func;
}


double get_error(vector<double> &coord, int k, vector<string> &equation, vector<string> &operators, vector<string> &terminals){
    double error;
    double func_val = help_get_error(coord, k, equation, operators, terminals);
    error = abs(func_val-coord[1]);
    return error;
}

double help_get_error(vector<double> &coord, int k, vector<string> &equation, vector<string> &operators, vector<string> &terminals){
    string item = equation[k];
    
    if (count(operators.begin(), operators.end(), item)){
        if (item == "*"){
            double result = help_get_error(coord, 2*k+1, equation, operators, terminals)*help_get_error(coord, 2*k+2, equation, operators, terminals);
            return result;
        }
        else if (item == "/"){
            double result = help_get_error(coord, 2*k+1, equation, operators, terminals)/help_get_error(coord, 2*k+2, equation, operators, terminals);
            return result;
        }
        else if (item == "+"){
            double result = help_get_error(coord, 2*k+1, equation, operators, terminals)+help_get_error(coord, 2*k+2, equation, operators, terminals);
            return result;
        }
        else if (item == "-"){
            double result = help_get_error(coord, 2*k+1, equation, operators, terminals)-help_get_error(coord, 2*k+2, equation, operators, terminals);
            return result;
        }
        else if (item == "sin"){
            double result = sin(help_get_error(coord, 2*k+1, equation, operators, terminals));
            return result;
        }
        else{
            double result = cos(help_get_error(coord, 2*k+1, equation, operators, terminals));
            return result;
        }
    }
    else{
        if (item == "x"){
            return coord[0];
        }
        else{
            double constant = stod(item);
            return constant;
        }
    }
}

void breed(vector<vector<double> > &coordinates, vector<string> &operations, vector<string> &terminals, Equation parent1, Equation parent2, vector<Equation> &population, vector<int> &cross1, vector<int> &cross2, vector<Equation> &new_population){
    Equation offspring1;
    
    offspring1.function = parent1.function;
    
    for (int i=0; i<cross1.size(); i++){
        offspring1.function[cross1[i]] = parent2.function[cross2[i]];
    }
    
    if (offspring1.function[3] == "T" && count(operations.begin(), operations.end(), offspring1.function[1])){
        int rand_num3 = rand() % 3;
        if (terminals[rand_num3] == "val"){
            int rand_num4 = rand() % 20;
            string s = to_string(rand_num4+1);
            offspring1.function[3] = s;
        }
        else{
            offspring1.function[3] = "x";
        }
    }
    
    double total_error1 = 0;
    for (int j=0; j<coordinates.size(); j++){
        total_error1 += get_error(coordinates[j], 0, offspring1.function, operations, terminals);
    }
    
    if (isnan(total_error1)){
        total_error1 = 10000;
    }
    
    offspring1.error = total_error1;
    
    vector<int> l1;
    for (int k=0; k<offspring1.function.size(); k++){
        if (offspring1.function[k] != "T" && offspring1.function[k] != "x"){
            l1.push_back(k);
        }
    }
    offspring1.locations = l1;
    
    int rand_num1 = rand() % 100;
    
    if(rand_num1 < 50){
        offspring1 = mutate(coordinates, operations, terminals, offspring1.function, offspring1.error, offspring1.locations);
    }
    
    if (offspring1.error < parent1.error){
        new_population.push_back(offspring1);
    }
    else{
        new_population.push_back(parent1);
    }
}

Equation mutate(vector<vector<double> > &coordinates, vector<string> &operations, vector<string> &terminals, vector<string> function, double &prev_error, vector<int> &locs){
    Equation mutation;
    
    if (locs.size() == 0){
        mutation.error = prev_error;
        mutation.function = function;
        return mutation;
    }
    
    int rand_num = rand() % locs.size();
    
    string gene = function[locs[rand_num]];
    
    if (gene == "sin"){
        function[locs[rand_num]] = "cos";
    }
    else if (gene == "cos"){
        function[locs[rand_num]] = "sin";
    }
    else if (count(operations.begin(), operations.end(), gene)){
        int rand_num2 = rand() % 4;
        while (operations[rand_num2] == gene){
            rand_num2 = rand() % 4;
        }
        
        function[locs[rand_num]] = operations[rand_num2];
    }
    else{
        double rand_num3 = rand() % 100;
        double add = (rand_num3+1.0)/100.0;
        double constant = stod(gene)+add;
        
        function[locs[rand_num]] =to_string(constant);
    }
    
    double total_error = 0;
    for (int i=0; i<coordinates.size(); i++){
        total_error += get_error(coordinates[i], 0, function, operations, terminals);
    }
    
    if (isnan(total_error)){
        total_error = 10000;
    }
    
    mutation.error = total_error;
    mutation.function = function;
    mutation.locations = locs;
    
    return mutation;
}

bool compareByError(const Equation &func1, const Equation &func2){
    return func1.error < func2.error;
}

vector<vector<double> > get_new_coords(vector<string> &equation, vector<vector<double>> &coords, vector<string> &operators, vector<string> &terminals){
    vector<vector<double>> eqn_coords;
    
    for (int i=0; i<coords.size(); i++){
        vector<double> coord;
        double x = coords[i][0];
        double y = help_get_error(coords[i], 0, equation, operators, terminals);
        coord.push_back(x);
        coord.push_back(y);
        eqn_coords.push_back(coord);
    }
    
    return eqn_coords;
}
