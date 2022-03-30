#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <ctime>

using namespace std;
bool PRINT_RES = true;


struct Node     //a node in the cubic grid
{
    int x,y,z; //coordinate
    int g;  //cost from start node to the current node
    int h;  //estimated cost from current node to the end
    int f;  //estimated whole cost(g+h)
    Node* father;
    Node(int x0,int y0,int z0):x(x0),y(y0),z(z0),f(0),g(0),h(0),father(NULL){}
    Node(int x0,int y0,int z0,Node* father0):x(x0),y(y0),z(z0),f(0),g(0),h(0),father(father0){}
};

class Astar
{
    string seq1;
    string seq2;
    string seq3;
    Node* start;
    Node* end;
    static const int GAP = 2;
    vector<Node*> openList;
    vector<Node*> closeList;
    //vector<Node*> allList;
public:
    vector<vector<int>> DPxy;//(seq1.size()+1,vector<int>(seq2.size()+1));
    vector<vector<int>> DPyz;//(seq1.size()+1,vector<int>(seq3.size()+1));
    vector<vector<int>> DPzx;//(seq3.size()+1,vector<int>(seq2.size()+1));
    string res1;
    string res2;
    string res3;
    int endCost;
    Astar(string s1,string s2,string s3);
    ~Astar();
    void nextstate(Node* current);
    void expandnode(int x,int y,int z,Node* father,int stepcost);
    int inList(vector<Node*>* Nodelist,int x,int y,int z);
    void calc_cost(Node* current,Node* end,int stepcost);
    static bool mycompare(Node* X,Node* Y);
    void search(Node* start,Node* end);
    void printRes();
    void DP2matrix_xy();
    void DP2matrix_yz();
    void DP2matrix_zx();
};

Astar::Astar(string s1,string s2,string s3):seq1(s1),seq2(s2),seq3(s3)
    {
        for(int i = 0;i<s1.size()+1;i++)
        {
            vector<int> temp;
            for(int j = 0;j<s2.size()+1;j++)
            {
                temp.push_back(0);
            }
            DPxy.push_back(temp);
        }
        for(int i = 0;i<s1.size()+1;i++)
        {
            vector<int> temp;
            for(int j = 0;j<s3.size()+1;j++)
            {
                temp.push_back(0);
            }
            DPyz.push_back(temp);
        }
        for(int i = 0;i<s3.size()+1;i++)
        {
            vector<int> temp;
            for(int j = 0;j<s2.size()+1;j++)
            {
                temp.push_back(0);
            }
            DPzx.push_back(temp);
        }
        DP2matrix_xy();
        DP2matrix_yz();
        DP2matrix_zx();
    }

Astar::~Astar()
{
    
    //for(int i=0;i<openList.size();i++) delete openList[i];
    //for(int i=0;i<closeList.size();i++) delete closeList[i];
    // delete allList[0];
    // cout<<allList[0]->f<<endl;
    openList.clear();
    closeList.clear();
}

void Astar::search(Node* startNode,Node* endNode)
{
    start = startNode;
    end = endNode;
    openList.push_back(start);

    ofstream outfile;
    outfile.open("out.txt",ios::out);
    if (!outfile.is_open())
	{
		cout << "read file error" << endl;
		return ;
	}

    while(openList.size()>0)
    {
        Node* current = openList[0];
        outfile<<current->x<<' '<<current->y<<' '<<current->z<<' '<<current->f<<' '<<current->g<<' '<<current->h<<endl;
        // cout<<current->f<<endl;
        if(current->x == end->x && current->y == end->y && current->z == end->z)//find the path
        {   
            end = current;
            endCost = end->g;
            //do the cleaning
            // openList.clear();
            // closeList.clear();
            break;
        }
        nextstate(current);
        closeList.push_back(current);
        openList.erase(openList.begin());
        sort(openList.begin(),openList.end(),mycompare); //find the best next node to expand
    }
    outfile.close();
}

void Astar::nextstate(Node* current)
{
    int MISMATCH_rd = 3*(seq2[current->x] != seq1[current->y]);
    int MISMATCH_db = 3*(seq3[current->z] != seq1[current->y]);
    int MISMATCH_br = 3*(seq2[current->x] != seq3[current->z]);
    // 7 situations in all
    expandnode(current->x+1,current->y,current->z,current,GAP*2);//right
    expandnode(current->x,current->y+1,current->z,current,GAP*2);//down
    expandnode(current->x,current->y,current->z+1,current,GAP*2);//back

    expandnode(current->x+1,current->y+1,current->z,current,MISMATCH_rd+GAP*2);//right-down
    expandnode(current->x,current->y+1,current->z+1,current,MISMATCH_db+GAP*2);//down-back
    expandnode(current->x+1,current->y,current->z+1,current,MISMATCH_br+GAP*2);//back-right

    expandnode(current->x+1,current->y+1,current->z+1,current,MISMATCH_rd+MISMATCH_db+MISMATCH_br);//right-down-back
}

void Astar::expandnode(int x,int y,int z,Node* father,int stepcost)
{
    if(x>seq2.size() || y>seq1.size() || z>seq3.size())  return;//end of the road
    if(inList(&closeList,x,y,z) != -1)    return;//already expanded
    int idx = inList(&openList,x,y,z);
    if(idx == -1)   //add it to openlist
    {
        Node* next_one = new Node(x,y,z,father);
        calc_cost(next_one,end,stepcost);
        openList.push_back(next_one);
        //allList.push_back(next_one);    //record all the nodes for cleaning
    }
    else
    {
        Node *old_one = openList[idx];
        if(stepcost+father->g < old_one->g)
        {
            old_one->father = father;
            old_one->g = stepcost+father->g;
            old_one->f = old_one->g + old_one->h;
        }
    }
}

int Astar::inList(vector<Node*>* Nodelist,int x,int y,int z)
{
    for(int i=0;i<Nodelist->size();i++)
    {
        if(Nodelist->at(i)->x == x && Nodelist->at(i)->y == y && Nodelist->at(i)->z == z)  return i;
    }
    return -1; //nonexist
}

void Astar::DP2matrix_xy()
{
    int len1 = seq1.size() + 1;
    int len2 = seq2.size() + 1;

    //constructing the backforward-dp matrix grid
    for(int i =len1-1;i>=0;i--) DPxy[i][len2-1] = 2*(len1-1-i); // a gap cost 2
    for(int j =len2-1;j>=0;j--) DPxy[len1-1][j] = 2*(len2-1-j);

    for(int i=len1-2;i>=0;i--)
    {
        for(int j=len2-2;j>=0;j--)
        {
            int cost_x = DPxy[i][j+1] + 2;
            int cost_y = DPxy[i+1][j] + 2;
            int cost_xy= DPxy[i+1][j+1] + 3*(seq1[i] != seq2[j]); // a mismatch cost 3
            DPxy[i][j] = min({cost_x,cost_y,cost_xy});
        }
    }
    
}

void Astar::DP2matrix_yz()
{
    int len1 = seq1.size() + 1;
    int len2 = seq3.size() + 1;

    //constructing the backforward-dp matrix grid
    for(int i =len1-1;i>=0;i--) DPyz[i][len2-1] = 2*(len1-1-i); // a gap cost 2
    for(int j =len2-1;j>=0;j--) DPyz[len1-1][j] = 2*(len2-1-j);

    for(int i=len1-2;i>=0;i--)
    {
        for(int j=len2-2;j>=0;j--)
        {
            int cost_z = DPyz[i][j+1] + 2;
            int cost_y = DPyz[i+1][j] + 2;
            int cost_yz= DPyz[i+1][j+1] + 3*(seq1[i] != seq3[j]); // a mismatch cost 3
            DPyz[i][j] = min({cost_z,cost_y,cost_yz});
        }
    }
    
}

void Astar::DP2matrix_zx()
{
    int len1 = seq3.size() + 1;
    int len2 = seq2.size() + 1;

    //constructing the backforward-dp matrix grid
    for(int i =len1-1;i>=0;i--) DPzx[i][len2-1] = 2*(len1-1-i); // a gap cost 2
    for(int j =len2-1;j>=0;j--) DPzx[len1-1][j] = 2*(len2-1-j);

    for(int i=len1-2;i>=0;i--)
    {
        for(int j=len2-2;j>=0;j--)
        {
            int cost_x = DPzx[i][j+1] + 2;
            int cost_z = DPzx[i+1][j] + 2;
            int cost_zx= DPzx[i+1][j+1] + 3*(seq3[i] != seq2[j]); // a mismatch cost 3
            DPzx[i][j] = min({cost_z,cost_x,cost_zx});
        }
    }

}

void Astar::calc_cost(Node* current,Node* end,int stepcost)
{
    int h = DPxy[current->y][current->x] + DPyz[current->y][current->z] + DPzx[current->z][current->x]; //3d heuristic function
    int g = current->father->g + stepcost;
    int f = g + h;
    current->f = f;
    current->g = g;
    current->h = h;
}

bool Astar::mycompare(Node* X,Node* Y)
{
    //if(X->f == Y->f)    return X->h < Y->h;
    return X->f < Y->f;
}

void Astar::printRes()
{
    int idx1 = seq1.size()-1;
    int idx2 = seq2.size()-1;
    int idx3 = seq3.size()-1;
    Node* current = end;
    while(current->father)
    {
        if(current->x == current->father->x && current->z == current->father->z)    //down
        {
            res2 = '-'+res2;
            res3 = '-'+res3;
            res1 = seq1[idx1]+res1;
            idx1--;
            current = current->father;
            continue;
        }
        if(current->y == current->father->y && current->z == current->father->z)    //right
        {
            res1 = '-'+res1;
            res3 = '-'+res3;
            res2 = seq2[idx2]+res2;
            idx2--;
            current = current->father;
            continue;
        }
        if(current->y == current->father->y && current->x == current->father->x)    //back
        {
            res1 = '-'+res1;
            res2 = '-'+res2;
            res3 = seq3[idx3]+res3;
            idx3--;
            current = current->father;
            continue;
        }
        if(current->y != current->father->y && current->z == current->father->z)    //right-down
        {
            res3 = '-'+res3;
            res2 = seq2[idx2]+res2;
            res1 = seq1[idx1]+res1;
            idx1--;
            idx2--;
            current = current->father;
            continue;
        }
        if(current->y != current->father->y && current->x == current->father->x)    //down-back
        {
            res2 = '-'+res2;
            res1 = seq1[idx1]+res1;
            res3 = seq3[idx3]+res3;
            idx3--;
            idx1--;
            current = current->father;
            continue;
        }
        if(current->y == current->father->y && current->z != current->father->z)    //back-right
        {
            res1 = '-'+res1;
            res2 = seq2[idx2]+res2;
            res3 = seq3[idx3]+res3;
            idx3--;
            idx2--;
            current = current->father;
            continue;
        }
        //right-down-back
        res1 = seq1[idx1]+res1;
        res2 = seq2[idx2]+res2;
        res3 = seq3[idx3]+res3;
        idx1--;
        idx2--;
        idx3--;
        current = current->father;
    }
}

vector<string> query = {"IPZJJLMLTKJULOSTKTJOGLKJOBLTXGKTPLUWWKOMOYJBGALJUKLGLOSVHWBPGWSLUKOBSOPLOOKUKSARPPJ",
                        "IWTJBGTJGJTWGBJTPKHAXHAGJJSJJPPJAPJHJHJHJHJHJHJHJHJPKSTJJUWXHGPHGALKLPJTPJPGVXPLBJHHJPKWPPDJSG"};

void findBestseq(string seq1,vector<string>& database,int &best_cost,int &idx1,int &idx2,string &res1,string &res2,string &res3)
{
    
    for(int m = 0;m<database.size();m++)
    {
        
        for(int k = m+1;k<database.size();k++)
        {
            
            Astar astar_3(seq1,database[m],database[k]);
            
            //if(astar_3.DPxy[0][0]+astar_3.DPyz[0][0]+astar_3.DPzx[0][0]>best_cost)  continue;

            //cout<<m<<' '<<k<<endl;
            Node* startPos = new Node(0,0,0);
            Node* endPos = new Node(database[m].size(),seq1.size(),database[k].size());
            astar_3.search(startPos,endPos);
            if (PRINT_RES)  astar_3.printRes();
            int current_cost = astar_3.endCost;
            if (current_cost<best_cost)
            {
                best_cost = current_cost;
                idx1 = m;
                idx2 = k;
                res1 = astar_3.res1;
                res2 = astar_3.res2;
                res3 = astar_3.res3;
            }
            delete startPos;
            delete endPos;
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



    //一共2条query
    for(int i = 0;i<query.size();i++)
    {
        int best_cost = INT_MAX;
        int idx1 = -1;
        int idx2 = -1;
        string res1,res2,res3;
        //int threshold = 0;
        findBestseq(query[i],database,best_cost,idx1,idx2,res1,res2,res3);
        cout<<"best"<<i<<"_1"<<':'<<database[idx1]<<endl;
        cout<<"best"<<i<<"_2"<<':'<<database[idx2]<<endl;
        if(PRINT_RES)
        {
            cout<<"res"<<i<<"_1:"<<res1<<endl;
            cout<<"res"<<i<<"_2:"<<res2<<endl;
            cout<<"res"<<i<<"_3:"<<res3<<endl;
        }
        cout<<"cost"<<i<<':'<<best_cost<<endl<<endl;
    }


    // string s1,s2,s3;
    // s1 = query[0];
    // s2 = "IPZJJPLLTHUULOSTXTJOGLKJGBLLMMPJPLUWGKOMOYJBZAYUKOFLOSZHGBPHXPLXKJBXKJLAUUOJHWTWWPQ";
    // s3 = "IPMJJLLLTHOULOSTMAJIGLKJPVLLXGKTPLTWWKOMOYJBZPYUKLILOSZHGBPGWXLZKJBSWJLPJUUMHKTRAP";
    // // s2 = database[0];
    // // s3 = database[20];
    // Astar astar_3(s1,s2,s3);
    // Node* startPos = new Node(0,0,0);
    // Node* endPos = new Node(s2.size(),s1.size(),s3.size());
    // astar_3.search(startPos,endPos);
    // cout<<astar_3.endCost<<endl;
    // astar_3.printRes();
    // cout<<astar_3.res1<<endl;
    // cout<<astar_3.res2<<endl;
    // cout<<astar_3.res3<<endl;

    end = clock();
    cout<<"time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}