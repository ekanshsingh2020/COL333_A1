#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
mt19937 gen(steady_clock::now().time_since_epoch().count());
uniform_int_distribution<long long> rnd(0,INT_MAX);

atomic<bool> exit_flag = false;

#include "SL.h"

    SportsLayout::SportsLayout(string inputfilename)
    {
          
        readInInputFile(inputfilename);
        mapping= new int[z];

    }

    bool SportsLayout::check_output_format()
    {

        vector<bool> visited(l,false);
        for(int i=0;i<z;i++)
        {
            if((mapping[i]>=1 && mapping[i]<=l))
            {
                if(!visited[mapping[i]-1])
                visited[mapping[i]-1]=true;
                else
                {
                    cout<<"Repeated locations, check format\n";
                    return false;
                }
            }
            else
            {
                cout<<"Invalid location, check format\n";
                return false;
            }
        }

        return true;

    }


    long long SportsLayout::cost_fn(int* mp){
        long long cost=0;
        for(int i=0;i<z;i++)
        {
           for(int j=i+1;j<z;j++) 
           {
                cost+=((long long)N[i][j]+(long long)N[j][i])*(long long)T[mp[i]-1][mp[j]-1];
           }
        }
        return cost;
    }

    void SportsLayout::readInInputFile(string inputfilename)
    {
            fstream ipfile;
	        ipfile.open(inputfilename, ios::in);
            if (!ipfile) {

                cout << "No such file\n";
                exit( 0 );
            }
            else {
            
                ipfile>> time;
                ipfile >> z;
                ipfile >> l;

                if(z>l)
                {
                    cout<<"Number of zones more than locations, check format of input file\n";
                    exit(0);
                }

                int **tempT;
                int **tempN;

                tempT = new int*[l];
                for (int i = 0; i < l; ++i)
                    tempT[i] = new int[l];
                
                tempN = new int*[z];
                for (int i = 0; i < z; ++i)
                    tempN[i] = new int[z];

                for(int i=0;i<z;i++)
                {
                    for(int j=0;j<z;j++)
                    ipfile>>tempN[i][j];
                }

                for(int i=0;i<l;i++)
                {
                    for(int j=0;j<l;j++)
                    ipfile>>tempT[i][j];
                }

                ipfile.close();

                T= tempT;
                N= tempN;
            }

    }


    void SportsLayout::write_to_file(string outputfilename){

         // Open the file for writing
        ofstream outputFile(outputfilename);

        // Check if the file is opened successfully
        if (!outputFile.is_open()) {
            cerr << "Failed to open the file for writing." << std::endl;
            exit(0);
        }

        for(int i=0;i<z;i++)
        outputFile<<mapping[i]<<" ";

        // Close the file
        outputFile.close();

        cout << "Allocation written to the file successfully." << endl;

    }
    
    int* SportsLayout::generate_random_mapping(){
        int* locations = new int[l];

        for(int i=0;i<l;i++) locations[i]=i+1;

        for(int i=0;i<l;i++) swap(locations[i],locations[rnd(gen)%l]);

        int* init_mp = new int[z];

        for(int i=0;i<z;i++) init_mp[i]=locations[i];

        return init_mp;
    }

    void SportsLayout::compute_allocation(int* mp)
    {
        for(int i=0;i<z;i++)
        mapping[i]=mp[i];
    }

    void SportsLayout::greedy_with_restarts(int* best_mp,int &best_cost){

        int* curr_mp= new int[z];
        int curr_cost,curr_best_cost=INT_MAX;
        int* curr_best_mp= new int[z];
        
        auto helper = [&](set<int> &used_locations){
            for(int i=0;i<z;i++){
                int curr_location = curr_mp[i];
                int best_location = curr_location;
                int best_location_cost = INT_MAX;

                for(int j=0;j<l;j++){
                    if(used_locations.find(j+1) == used_locations.end()){
                        curr_mp[i] = j+1;
                        curr_cost = cost_fn(curr_mp);
                        if(curr_cost < best_location_cost){
                            best_location_cost = curr_cost;
                            best_location = j+1;
                        }
                    }
                }

                curr_mp[i] = best_location;
                used_locations.erase(curr_location);
                used_locations.insert(best_location);
            }

            curr_cost = cost_fn(curr_mp);

            if(curr_cost < curr_best_cost){
                curr_best_cost = curr_cost;
                for(int i=0;i<z;i++)
                    curr_best_mp[i] = curr_mp[i];
            }

        };

        int restarts = 1000;

        while(restarts--){
            if(exit_flag) goto return_point_label;
            int* temp = generate_random_mapping();

            for(int i=0;i<z;i++){
                curr_mp[i] = temp[i];
                curr_best_mp[i] = temp[i];
            }

            curr_cost = cost_fn(curr_mp);
            curr_best_cost=cost_fn(curr_best_mp);
            set<int>used_locations(curr_mp,curr_mp+z);
            
            int iterations = 200;
            while(iterations--){
                helper(used_locations);
            }
            if(curr_best_cost<best_cost){
                best_cost = curr_best_cost;
                for(int i=0;i<z;i++)
                    best_mp[i] = curr_best_mp[i];
            }
        }

        return_point_label:
        return;
    }

    int* SportsLayout::find_best_mapping(){
        vector<int*> arr_mp;
        vector<int> arr_costs;
        
        // greedy hill climbing with restarts
        arr_mp.push_back(new int[z]);
        arr_costs.push_back(INT_MAX);
        thread t1(&SportsLayout::greedy_with_restarts,this,arr_mp[0],ref(arr_costs[0])); 
        
        // signalling all the processes to exit by returning after the time limit
        this_thread::sleep_for(chrono::seconds(time*60 - 1));
        exit_flag = true;
        t1.join();

        // now computing the best mapping among all algorithms
        int best_cost = arr_costs[0];
        int* best_mp = arr_mp[0];
        for(int i=1;i<(int)(arr_mp).size();i++){
            if(best_cost<arr_costs[i]){
                best_cost = arr_costs[i];
                best_mp = arr_mp[i];
            }
        }
        return best_mp;
    }