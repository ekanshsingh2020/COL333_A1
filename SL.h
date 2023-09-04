
#ifndef SPORTSLAYOUT_H
#define	SPORTSLAYOUT_H

#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

struct SportsLayout{
    int z,l;
    int** T;
    int **N;
    double time;
    int *mapping;
    
    SportsLayout(string inputfilename);

    bool check_output_format();
    
    long long cost_fn(int* mp);

    void write_to_file(string outputfilename);

    void readInInputFile(string inputfilename);

    void compute_allocation(int* mp);
    
    long long find_contribution(int* mp,int idx);

    int* generate_random_mapping();

    void greedy_with_restarts(int* best_mp,int &best_cost);

    int* find_best_mapping();
    
    void simulated_annealing(int* best_mp,int &best_cost);

};
#endif