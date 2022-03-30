#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
//#include <limits>

using namespace std;

int dp_3(string seq1,string seq2,string seq3)
{
    int len1 = seq1.size() + 1;
    int len2 = seq2.size() + 1;
    int len3 = seq3.size() + 1;
    vector<vector<vector<int>>> dp(len1,vector<vector<int>>(len2,vector<int>(len3)));
    //constructing the cubic grid
    //1d
    for(int i =0;i<len1;i++) dp[i][0][0] = 4*i; // a straight step means two gaps, which cost 4
    for(int j =0;j<len2;j++) dp[0][j][0] = 4*j;
    for(int k =0;k<len3;k++) dp[0][0][k] = 4*k;
    //2d
    for(int i = 1;i<len1;i++)
    {
        for(int j=1;j<len2;j++)
        {
            int cost_y = dp[i][j-1][0] + 4;
            int cost_z = dp[i-1][j][0] + 4;
            int cost_yz = dp[i-1][j-1][0] + 4 + 3*(seq1[i-1] != seq2[j-1]); //a gap for x, a mismatch for y&z
            dp[i][j][0] = min({cost_y,cost_z,cost_yz});
        }
    }
    
    for(int j = 1;j<len2;j++)
    {
        for(int k=1;k<len3;k++)
        {
            int cost_x = dp[0][j][k-1] + 4;
            int cost_y = dp[0][j-1][k] + 4;
            int cost_xy = dp[0][j-1][k-1] + 4 + 3*(seq2[j-1] != seq3[k-1]);
            dp[0][j][k] = min({cost_x,cost_y,cost_xy});
        }
    }

    for(int k=1;k<len3;k++)
    {
        for(int i=1;i<len1;i++)
        {
            int cost_z = dp[i-1][0][k] + 4;
            int cost_x = dp[i][0][k-1] + 4;
            int cost_zx = dp[i-1][0][k-1] + 4 + 3*(seq3[k-1] != seq1[i-1]);
            dp[i][0][k] = min({cost_z,cost_x,cost_zx});
        }
    }
    //3d
    for(int i=1;i<len1;i++)
    {
        for(int j=1;j<len2;j++)
        {
            for(int k=1;k<len3;k++)
            {
                //7moves in all
                int cost_x = dp[i][j][k-1] + 4;
                int cost_y = dp[i][j-1][k] + 4;
                int cost_z = dp[i-1][j][k] + 4;
                int cost_xy = dp[i][j-1][k-1] + 4 + 3*(seq2[j-1] != seq3[k-1]);
                int cost_yz = dp[i-1][j-1][k] + 4 + 3*(seq1[i-1] != seq2[j-1]);
                int cost_zx = dp[i-1][j][k-1] + 4 + 3*(seq3[k-1] != seq1[i-1]);
                int cost_xyz = dp[i-1][j-1][k-1] + 3*(seq2[j-1] != seq3[k-1]) + 3*(seq1[i-1] != seq2[j-1]) + 3*(seq3[k-1] != seq1[i-1]);
                dp[i][j][k] = min({cost_x,cost_y,cost_z,cost_xy,cost_yz,cost_zx,cost_xyz});
            }
        }
    }
    
    return dp[len1-1][len2-1][len3-1];
}

vector<string> query = {"IPZJJLMLTKJULOSTKTJOGLKJOBLTXGKTPLUWWKOMOYJBGALJUKLGLOSVHWBPGWSLUKOBSOPLOOKUKSARPPJ",
                        "IWTJBGTJGJTWGBJTPKHAXHAGJJSJJPPJAPJHJHJHJHJHJHJHJHJPKSTJJUWXHGPHGALKLPJTPJPGVXPLBJHHJPKWPPDJSG"};

void findBestseq(string seq1,vector<string>& database,int &best_cost,int &idx1,int &idx2)
{
    for(int m = 0;m<database.size();m++)
    {
        for(int n = m+1;n<database.size();n++)
        {
            int current_cost = dp_3(seq1,database[m],database[n]);
            if (current_cost<best_cost)
            {
                best_cost = current_cost;
                idx1 = m;
                idx2 = n;
            }
        }
    }
}


int main()
{
    clock_t start,end;
    start = clock();

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

    //2 queries in all
    for(int i = 0;i<query.size();i++)
    {
        int best_cost = INT_MAX;
        int idx1 = -1;
        int idx2 = -1;
        findBestseq(query[i],database,best_cost,idx1,idx2);
        cout<<"best"<<i<<"_1"<<':'<<database[idx1]<<endl;
        cout<<"best"<<i<<"_2"<<':'<<database[idx2]<<endl;
        cout<<"cost"<<i<<':'<<best_cost<<endl;
    }

    end = clock();
    cout<<"time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}
