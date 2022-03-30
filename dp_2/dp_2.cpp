#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
//#include <limits>

using namespace std;

int dp_2(string seq1,string seq2)
{
    int len1 = seq1.size() + 1;
    int len2 = seq2.size() + 1;
    vector<vector<int>> dp(len1,vector<int>(len2));
    //constructing the matrix grid
    for(int i =0;i<len1;i++) dp[i][0] = 2*i; // a gap cost 2
    for(int j =0;j<len2;j++) dp[0][j] = 2*j;

    for(int i=1;i<len1;i++)
    {
        for(int j=1;j<len2;j++)
        {
            int cost_x = dp[i][j-1] + 2;
            int cost_y = dp[i-1][j] + 2;
            int cost_xy= dp[i-1][j-1] + 3*(seq1[i-1] != seq2[j-1]); // a mismatch cost 3
            dp[i][j] = min({cost_x,cost_y,cost_xy});
        }
    }
    return dp[len1-1][len2-1];
}

vector<string> query = {"KJXXJAJKPXKJJXJKPXKJXXJAJKPXKJJXJKPXKJXXJAJKPXKJXXJAJKHXKJXXJAJKPXKJXXJAJKHXKJXX",
                        "ILOTGJJLABWTSTGGONXJMUTUXSJHKWJHCTOQHWGAGIWLZHWPKZULJTZWAKBWHXMIKLZJGLXBPAHOHVOLZWOSJJLPO",
                        "IHKKKRKKKKKKXGWGKKKPKSKKKKKBKKKPKHKKXKKBSKKPKWKKLKSKRKKWXKPKKBKKKPKTSKHKKKKLADKKYPKKKOPHKKBWWLPPWKK",
                        "MPPPJPXPGPJPPPXPPPJPJPPPXPPPPSPPJJJPPXXPPPPPJPPPXPPXIPJMMMXPKPSVGULMHHZPAWHTHKAAHHUPAONAPJSWPPJGA",
                        "IPPVKBKXWXKHSAPHVXXVOJMRAKKPJVLLJBWKOLLJKXHGXLLCPAJOBKPGXBATGXMPOMCVZTAXVPAGKXGOMJQOLJGWGKXLQ"};

void findBestseq(string seq1,vector<string>& database,int &best_cost,int &idx)
{
    for(int k = 0;k<database.size();k++)
    {
        int current_cost = dp_2(seq1,database[k]);
        if (current_cost<best_cost)
        {
            best_cost = current_cost;
            idx = k;
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

    //5 queries in all
    for(int i = 0;i<query.size();i++)
    {
        int best_cost = INT_MAX;
        int idx = -1;
        findBestseq(query[i],database,best_cost,idx);
        cout<<"best"<<i<<':'<<database[idx]<<endl;
        cout<<"cost"<<i<<':'<<best_cost<<endl;
    }

    end = clock();
    cout<<"time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}