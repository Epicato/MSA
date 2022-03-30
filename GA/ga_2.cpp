#include <iostream>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <ctime>
#include <cmath>

using namespace std;

vector<string> query = {"KJXXJAJKPXKJJXJKPXKJXXJAJKPXKJJXJKPXKJXXJAJKPXKJXXJAJKHXKJXXJAJKPXKJXXJAJKHXKJXX",
                        "ILOTGJJLABWTSTGGONXJMUTUXSJHKWJHCTOQHWGAGIWLZHWPKZULJTZWAKBWHXMIKLZJGLXBPAHOHVOLZWOSJJLPO",
                        "IHKKKRKKKKKKXGWGKKKPKSKKKKKBKKKPKHKKXKKBSKKPKWKKLKSKRKKWXKPKKBKKKPKTSKHKKKKLADKKYPKKKOPHKKBWWLPPWKK",
                        "MPPPJPXPGPJPPPXPPPJPJPPPXPPPPSPPJJJPPXXPPPPPJPPPXPPXIPJMMMXPKPSVGULMHHZPAWHTHKAAHHUPAONAPJSWPPJGA",
                        "IPPVKBKXWXKHSAPHVXXVOJMRAKKPJVLLJBWKOLLJKXHGXLLCPAJOBKPGXBATGXMPOMCVZTAXVPAGKXGOMJQOLJGWGKXLQ"};



const int N = 100;

class Gene
{
    const int GAP = 2;
    const int MISMATCH = 3;
    const double CROSS_RATE = 0.8;
    const double MUTE_RATE = 0.8;
    const int ITERS = 100;
    int it = 0;
    string seq1;
    string seq2;
    vector<string> first_population[N];
    int first_cost[N];
    int last_cost[N];
    vector<string> select_population[N];
    vector<string> cross_population[N];
public:
    string res1;
    string res2;
    int bestcost;
    Gene(string s1,string s2):seq1(s1),seq2(s2){};
    void iteration();
    void init_population();
    void cost_eval(vector<string>* pop,int* costList);
    void selection();
    void crossover();
    void mutation();
    void mix();
    void print();
    void calculate_best();
};

void Gene::iteration()
{
    init_population();
    cost_eval(first_population,first_cost);
    for(it = 0;it<ITERS;it++)
    {
        selection();

        crossover();

        mutation();

        mix();
        
    }
    calculate_best();
    //print();
}

void Gene::init_population()
{
    for(int i =0;i<N;i++)
    {
        string s1,s2;
        s1 = seq1;
        s2 = seq2;
        if(s1.size()<=s2.size())    //seq2 is longer
        {
            while(s1.size()<s2.size())
            {
                s1.insert(rand()%(s1.size()+1),"-");
            }
        }
        else    //seq1 is longer
        {
            while(s2.size()<s1.size())
            {
                s2.insert(rand()%(s2.size()+1),"-");
            }
        }
        first_population[i].push_back(s1);
        first_population[i].push_back(s2);
        //cout<<first_population[i][0]<<' '<<first_population[i][1]<<endl;
    }
}

void Gene::cost_eval(vector<string>* pop,int* costList)
{
    for(int i =0;i<N;i++)
    {
        int cost=0;
        //cout<<pop[i][0].size()<<' '<<pop[i][1].size()<<endl;
        for(int idx = 0;idx<pop[i][0].size();idx++)
        {
            if(pop[i][0][idx] != pop[i][1][idx])
            {
                if(pop[i][0][idx] == '-' || pop[i][1][idx] == '-') cost += GAP;
                else cost += MISMATCH;
            }
            // else if(pop[i][0][idx] == '-')   //eliminate both'-'   Better not use!!! Bad performance!
            // {
            //     pop[i][0].erase(idx,1);
            //     pop[i][1].erase(idx,1);
            //     idx--;
            // }
        }
        
        costList[i] = cost;
        //cout<<costlist[i]<<endl;
    }
}

void Gene::selection()
{
    double all_weight = 0;
    double weight[N];
    double accumulated_weight = 0;
    double probability[N];
    for(int i = 0;i<N;i++)
    {
        weight[i] = 1/(double)first_cost[i];
        all_weight += weight[i];
    }
    for(int i=0;i<N;i++)
    {
        accumulated_weight += weight[i];
        probability[i] = accumulated_weight / all_weight;
        //cout<<weight[i]<<' ';
    }
    
    for(int i=0;i<N;i++)
    {
        double threshold = (double)rand()/RAND_MAX;
        for(int idx=0;idx<N;idx++)
        {
            if(threshold <= probability[idx])   //pick idx-th individual
            {
                select_population[i] = first_population[idx];
                //cout<<endl<<idx;
                break;
            }
        }
    }
}

void Gene::crossover()
{
    int child_idx = 0;
    for(int i=0;i<N/2;i++)
    {
        //pick parent individuals
        int father_idx,mother_idx;
        father_idx = rand()%N;
        mother_idx = rand()%N;
        while(mother_idx == father_idx) mother_idx = rand()%N;
        cross_population[child_idx] = select_population[father_idx];
        cross_population[child_idx+1] = select_population[mother_idx];
        double rate = (double)rand()/RAND_MAX;
        if(rate<=CROSS_RATE)
        {
            //exchange one of the sequences
            string temp = cross_population[child_idx][0];
            cross_population[child_idx][0] = cross_population[child_idx+1][0];
            cross_population[child_idx+1][0] = temp;
            while(cross_population[child_idx][0].size()<cross_population[child_idx][1].size())
            {
                cross_population[child_idx][0] += "-";
            }
            while(cross_population[child_idx][1].size()<cross_population[child_idx][0].size())
            {
                cross_population[child_idx][1] += "-";
            }

            while(cross_population[child_idx+1][0].size()<cross_population[child_idx+1][1].size())
            {
                cross_population[child_idx+1][0] += "-";
            }
            while(cross_population[child_idx+1][1].size()<cross_population[child_idx+1][0].size())
            {
                cross_population[child_idx+1][1] += "-";
            }
        }
        child_idx += 2;
    }
}

void Gene::mutation()
{
    for(int i=0;i<N;i++)
    {
        double rate = (double)rand()/RAND_MAX;
        if(rate<=MUTE_RATE) //mutation:insert a gap to both sequences
        {
            //int minSize = min(cross_population[i][0].size(),cross_population[i][1].size());
            int mute_idx = rand()%(cross_population[i][0].size()+1);
            cross_population[i][0].insert(mute_idx,"-");
            cross_population[i][1].insert(mute_idx,"-");
        }
        //cout<<cross_population[i][0]<<endl<<cross_population[i][1]<<endl<<endl;
    }
}

void Gene::mix()
{
    vector<string> next_population[N];
    int next_cost[N];
    int next_idx = 0;
    cost_eval(cross_population,last_cost);
    int temp[N],index[N];
    //pick 70% from the cross_population
    int cross_num = round(0.7*N);
    copy(last_cost,last_cost+N,temp);
    sort(temp,temp+N);
    for(int i=0;i<N;i++)
    {
        index[i] = find(last_cost,last_cost+N,temp[i])-last_cost;
        temp[i] = -1;
    }
    for(int i=0;i<cross_num;i++)
    {
        next_cost[next_idx] = last_cost[index[i]];
        next_population[next_idx] = cross_population[index[i]];
        next_idx++;
    }

    //pick 20% from the first_population
    int first_num = round(0.2*N);
    copy(first_cost,first_cost+N,temp);
    sort(temp,temp+N);
    for(int i=0;i<N;i++)
    {
        index[i] = find(first_cost,first_cost+N,temp[i])-first_cost;
        temp[i] = -1;
    }
    for(int i=0;i<first_num;i++)
    {
        next_cost[next_idx] = first_cost[index[i]];
        next_population[next_idx] = first_population[index[i]];
        next_idx++;
    }

    //pick 10% from the new_population
    int new_num = N-first_num-cross_num;
    for(int i =0;i<new_num;i++)
    {
        string s1,s2;
        s1 = seq1;
        s2 = seq2;
        if(s1.size()<=s2.size())    //seq2 is longer
        {
            while(s1.size()<s2.size())
            {
                s1.insert(rand()%(s1.size()+1),"-");
            }
        }
        else    //seq1 is longer
        {
            while(s2.size()<s1.size())
            {
                s2.insert(rand()%(s2.size()+1),"-");
            }
        }
        next_population[next_idx].push_back(s1);
        next_population[next_idx].push_back(s2);
        int cost=0;
        for(int idx = 0;idx<s1.size();idx++)
        {
            if(s1[idx] != s2[idx])
            {
                if(s1[idx] == '-' || s2[idx] == '-') cost += GAP;
                else cost += MISMATCH;
            }
        }
        next_cost[next_idx] = cost;
        
        next_idx ++;
    }
    

    //pass to the next iteration
    copy(next_cost,next_cost+N,first_cost);
    copy(next_population,next_population+N,first_population);
}

void Gene::print()
{
    //cout<<"The best of ITERATION "<<it<<" is: "<<endl;
    cout<<"The best alignment is:"<<endl;
    int idx = 0;
    int minCost = INT_MAX;
    for(int i=0;i<N;i++)
    {
        
        if(first_cost[i]<minCost)
        {
            minCost = first_cost[i];
            idx = i;
        }
    }
    for(int i=0;i<first_population[idx][0].size();i++)  //eliminate both'-'
    {
        while(first_population[idx][0][i] == first_population[idx][1][i] && first_population[idx][0][i] == '-')
        {
            first_population[idx][0].erase(i,1);
            first_population[idx][1].erase(i,1);
        }
    }
    cout<<first_population[idx][0]<<endl<<first_population[idx][1]<<endl;
    cout<<"The minCost is: "<<minCost<<endl;
}

void Gene::calculate_best()
{
    int idx = 0;
    int minCost = INT_MAX;
    for(int i=0;i<N;i++)
    {
        if(first_cost[i]<minCost)
        {
            minCost = first_cost[i];
            idx = i;
        }
    }
    for(int i=0;i<first_population[idx][0].size();i++)  //eliminate both'-'
    {
        while(first_population[idx][0][i] == first_population[idx][1][i] && first_population[idx][0][i] == '-')
        {
            first_population[idx][0].erase(i,1);
            first_population[idx][1].erase(i,1);
        }
    }
    bestcost = minCost;
    res1 = first_population[idx][0];
    res2 = first_population[idx][1];
}


void findBestseq(string seq1,vector<string>& database,int &best_cost,int &idx,string &res1, string &res2)
{
    for(int i = 0;i<database.size();i++)
    {
        Gene new_gene(seq1,database[i]);
        new_gene.iteration();
        if(new_gene.bestcost<best_cost)
        {
            best_cost = new_gene.bestcost;
            idx = i;
            res1 = new_gene.res1;
            res2 = new_gene.res2;
        }
    }
}

int main()
{
    //srand(time(NULL));
    clock_t start,end,mid1,mid2;
    start = clock();
    mid1 = start;


    ifstream infile;
    infile.open("../data/MSA_database.txt",ios::in);
    if (!infile.is_open())
	{
		cout << "read file error" << endl;
		return 0;
	}
    string data_seq;
    vector<string> database;
	while (getline(infile,data_seq))
	{
		database.push_back(data_seq);
	}
    infile.close();

    //5 queries in all
    for(int i=0;i<query.size();i++)
    {
        srand(time(NULL));
        int best_cost = INT_MAX;
        int idx = -1;
        string res1,res2;
        findBestseq(query[i],database,best_cost,idx,res1,res2);
        cout<<"Best cost for query "<<i<<":  "<<best_cost<<endl;
        cout<<"Best alignment:"<<endl;
        cout<<res1<<endl<<res2<<endl;
        mid2 = clock();
        cout<<"time"<<i<<" = "<<double(mid2-mid1)/CLOCKS_PER_SEC<<"s"<<endl;
        mid1 = clock();
    }


    // int cost = INT_MAX;
    // int idx = -1;
    // string res1,res2;
    // for(int i = 0;i<database.size();i++)
    // {
    //     Gene new_gene(query[4],database[i]);
    //     new_gene.iteration();
    //     if(new_gene.bestcost<cost)
    //     {
    //         cost = new_gene.bestcost;
    //         idx = i;
    //         res1 = new_gene.res1;
    //         res2 = new_gene.res2;
    //     }
    // }
    



    end = clock();
    cout<<"whole time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}
