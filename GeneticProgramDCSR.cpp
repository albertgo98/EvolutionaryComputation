//
//  main.cpp
//  Genetic Program Symbolic Regression
//
//  Created by Albert Go on 10/21/21.
//
//  This program implements maintaining genetic diversity through determinstic crowding.

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
void breed(vector<vector<double> > &coordinates, vector<string> &operations, vector<string> &terminals, Equation parent1, Equation parent2, vector<Equation> &population, vector<int> &cross1, vector<int> &cross2, int index1, int index2);
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
    
    vector<int> cross1{14, 29, 30, 59, 60, 61, 62, 119, 120, 121, 122, 123, 124, 125, 126, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254};
    vector<int> cross2{7, 15, 16, 31, 32, 33, 34, 63, 64, 65, 66, 67, 68, 69, 70, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142};
    
    vector<Equation> population = get_population(coordinates, operations, terminal);
    sort(population.begin(), population.end(), compareByError);
    cout << "Established population" << endl;
    int evaluations = 0;
    
    while(evaluations < 10000){
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
            breed(coordinates, operations, terminal, population[i], population[parent2], population, cross1, cross1, i, parent2);
        }

        sort(population.begin(), population.end(), compareByError);
        population.erase(population.begin()+200, population.end());
        
        evaluations += 1;
        
        if (evaluations % 50 == 0){
            cout << "Status update: ";
            cout << population[0].error << endl;
            cout << "Updating population..." << endl;
            vector<Equation> new_set = get_population(coordinates, operations, terminal);
            population.insert(population.end(), new_set.begin(), new_set.end());
            sort(population.begin(), population.end(), compareByError);
            vector<string> curr_best = population[0].function;
            cout << "[";
            for(int j=0; j < curr_best.size(); j++){
                cout << curr_best[j];
                if (j < curr_best.size()-1){
                    cout << ", ";
                }
                else{
                    cout << "]" << endl;
                }
            }
        }
        
        if (population[0].error == 0){
            cout << evaluations << endl;
            break;
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
        int rand_num = rand() % 10;

        if (i == 0){
            if (rand_num < 9){
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
                    if (rand_num < 9){
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
                        if (rand_num < 3){
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
    
//    cout << item << endl;
    
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
            if (item == "T"){
                int rand_num2 = rand() % 3;
                if (terminals[rand_num2] == "val"){
                    int rand_num3 = rand() % 20;
                    return rand_num3;
                }
                else{
                    return coord[0];
                }
            }
            double constant = stod(item);
            return constant;
        }
    }
}

vector<vector<double> > get_new_coords(vector<string> &equation, vector<vector<double>> &coords, vector<string> &operators, vector<string> &terminals){
    vector<vector<double> > eqn_coords;
    
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

void breed(vector<vector<double> > &coordinates, vector<string> &operations, vector<string> &terminals, Equation parent1, Equation parent2, vector<Equation> &population, vector<int> &cross1, vector<int> &cross2, int index1, int index2){
    Equation offspring1;
    Equation offspring2;
    
    offspring1.function = parent1.function;
    offspring2.function = parent2.function;
    
    for (int i=0; i<cross1.size(); i++){
        offspring1.function[cross1[i]] = parent2.function[cross2[i]];
        offspring2.function[cross2[i]] = parent1.function[cross1[i]];
    }
    
    double total_error1 = 0;
    double total_error2 = 0;
    for (int j=0; j<coordinates.size(); j++){
        total_error1 += get_error(coordinates[j], 0, offspring1.function, operations, terminals);
        total_error2 += get_error(coordinates[j], 0, offspring2.function, operations, terminals);
    }
    offspring1.error = total_error1;
    offspring2.error = total_error2;
    
    vector<int> l1;
    vector<int> l2;
    for (int k=0; k<offspring1.function.size(); k++){
        if (offspring1.function[k] != "T" && offspring1.function[k] != "x"){
            l1.push_back(k);
        }
        if (offspring2.function[k] != "T" && offspring2.function[k] != "x"){
            l2.push_back(k);
        }
    }
    offspring1.locations = l1;
    offspring2.locations = l2;
    
    int rand_num1 = rand() % 100;
    int rand_num2 = rand() % 100;
    
    if(rand_num1 < 50){
        offspring1 = mutate(coordinates, operations, terminals, offspring1.function, offspring1.error, offspring1.locations);
    }
    if(rand_num2 < 50){
        offspring2 = mutate(coordinates, operations, terminals, offspring2.function, offspring2.error, offspring2.locations);
    }
    
    if (offspring1.error < parent1.error || offspring1.error < parent2.error || offspring2.error < parent1.error || offspring2.error < parent2.error){
        double diff1 = 0;
        double diff2 = 0;
        double diff3 = 0;
        double diff4 = 0;
        
        for (int n=0; n<50; n++){
            diff1 += abs(help_get_error(coordinates[n], 0, offspring1.function, operations, terminals)-help_get_error(coordinates[n], 0, parent1.function, operations, terminals));
            diff2 += abs(help_get_error(coordinates[n], 0, offspring2.function, operations, terminals)-help_get_error(coordinates[n], 0, parent2.function, operations, terminals));
            
            diff3 += abs(help_get_error(coordinates[n], 0, offspring1.function, operations, terminals)-help_get_error(coordinates[n], 0, parent2.function, operations, terminals));
            diff4 += abs(help_get_error(coordinates[n], 0, offspring2.function, operations, terminals)-help_get_error(coordinates[n], 0, parent1.function, operations, terminals));
        }
        if (diff1+diff2 < diff3+diff4){
            if (offspring1.error < parent1.error){
                population[index1] = offspring1;
            }
            if (offspring2.error < parent2.error){
                population[index2] = offspring2;
            }
        }
        else{
            if (offspring1.error < parent2.error){
                population[index2] = offspring1;
            }
            if (offspring2.error < parent1.error){
                population[index1] = offspring2;
            }
        }
            
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
    
    mutation.error = total_error;
    mutation.function = function;
    mutation.locations = locs;
    
    return mutation;
}

bool compareByError(const Equation &func1, const Equation &func2){
    return func1.error < func2.error;
}
