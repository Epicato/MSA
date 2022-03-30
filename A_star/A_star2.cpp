#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <string>
#include <ctime>

using namespace std;
bool PRINT_RES = true;

struct Node     //a node in the grid
{
    int x,y; //coordinate
    int g;  //cost from start node to the current node
    int h;  //estimated cost from current node to the end
    int f;  //estimated whole cost(g+h)
    Node* father;
    Node(int x0,int y0):x(x0),y(y0),f(0),g(0),h(0),father(NULL){}
    Node(int x0,int y0,Node* father0):x(x0),y(y0),f(0),g(0),h(0),father(father0){}
};

class Astar
{
    string seq1;
    string seq2;
    Node* start;
    Node* end;
    static const int GAP = 2;
    vector<Node*> openList;
    vector<Node*> closeList;
public:
    string res1;
    string res2;
    int endCost;
    Astar(string s1,string s2):seq1(s1),seq2(s2){};
    ~Astar(){};
    void nextstate(Node* current);
    void expandnode(int x,int y,Node* father,int stepcost);
    int inList(vector<Node*>* Nodelist,int x,int y);
    void calc_cost(Node* current,Node* end,int stepcost);
    static bool mycompare(Node* X,Node* Y);
    void search(Node* start,Node* end);
    void printRes();
};

void Astar::search(Node* startNode,Node* endNode)
{
    Node* current;
    start = startNode;
    end = endNode;
    openList.push_back(start);
    while(openList.size()>0)
    {
        current = openList[0];
        //cout<<current->x<<' '<<current->y<<endl;
        if(current->x == end->x && current->y == end->y)//find the path
        {   
            end = current;
            endCost = end->g;
            
            //do the cleaning
            // for(int i=0;i<openList.size();i++)  
            // {
            //     delete openList[i];
            //     openList[i]=NULL;
            // }
            // for(int i=0;i<closeList.size();i++)
            // {
            //     delete closeList[i];
            //     closeList[i]=NULL;
            // }
            openList.clear();
            closeList.clear();
            break;
        }
        nextstate(current);
        closeList.push_back(current);
        openList.erase(openList.begin());
        sort(openList.begin(),openList.end(),mycompare); //find the best next node to expand
    }
}

void Astar::nextstate(Node* current)
{
    int MISMATCH = 3*(seq2[current->x] != seq1[current->y]);
    expandnode(current->x+1,current->y,current,GAP);//right
    expandnode(current->x,current->y+1,current,GAP);//down
    expandnode(current->x+1,current->y+1,current,MISMATCH);//right-down
}

void Astar::expandnode(int x,int y,Node* father,int stepcost)
{
    if(x>seq2.size() || y>seq1.size())  return;//end of the road
    if(inList(&closeList,x,y) != -1)    return;//already expanded
    int idx = inList(&openList,x,y);
    if(idx == -1)   //add it to openlist
    {
        Node* next_one = new Node(x,y,father);
        calc_cost(next_one,end,stepcost);
        openList.push_back(next_one);
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

int Astar::inList(vector<Node*>* Nodelist,int x,int y)
{
    for(int i=0;i<Nodelist->size();i++)
    {
        if(Nodelist->at(i)->x == x && Nodelist->at(i)->y == y)  return i;
    }
    return -1; //nonexist
}

void Astar::calc_cost(Node* current,Node* end,int stepcost)
{
    int h = GAP*abs((end->x-current->x)-(end->y-current->y)); //heuristic function
    int g = current->father->g + stepcost;
    int f = g + h;
    current->f = f;
    current->g = g;
    current->h = h;
}

bool Astar::mycompare(Node* X,Node* Y)
{
    return X->f < Y->f;
}

void Astar::printRes()
{
    int idx1 = seq1.size()-1;
    int idx2 = seq2.size()-1;
    Node* current = end;
    while(current->father)
    {
        if(current->x == current->father->x)    //down
        {
            res2 = '-'+res2;
            res1 = seq1[idx1]+res1;
            idx1--;
            current = current->father;
            continue;
        }
        if(current->y == current->father->y)    //right
        {
            res1 = '-'+res1;
            res2 = seq2[idx2]+res2;
            idx2--;
            current = current->father;
            continue;
        }
        //right-down
        res1 = seq1[idx1]+res1;
        res2 = seq2[idx2]+res2;
        idx1--;
        idx2--;
        current = current->father;
    }
}

vector<string> query = {"KJXXJAJKPXKJJXJKPXKJXXJAJKPXKJJXJKPXKJXXJAJKPXKJXXJAJKHXKJXXJAJKPXKJXXJAJKHXKJXX",
                        "ILOTGJJLABWTSTGGONXJMUTUXSJHKWJHCTOQHWGAGIWLZHWPKZULJTZWAKBWHXMIKLZJGLXBPAHOHVOLZWOSJJLPO",
                        "IHKKKRKKKKKKXGWGKKKPKSKKKKKBKKKPKHKKXKKBSKKPKWKKLKSKRKKWXKPKKBKKKPKTSKHKKKKLADKKYPKKKOPHKKBWWLPPWKK",
                        "MPPPJPXPGPJPPPXPPPJPJPPPXPPPPSPPJJJPPXXPPPPPJPPPXPPXIPJMMMXPKPSVGULMHHZPAWHTHKAAHHUPAONAPJSWPPJGA",
                        "IPPVKBKXWXKHSAPHVXXVOJMRAKKPJVLLJBWKOLLJKXHGXLLCPAJOBKPGXBATGXMPOMCVZTAXVPAGKXGOMJQOLJGWGKXLQ"};

void findBestseq(string seq1,vector<string>& database,int &best_cost,int &idx,string &res1,string &res2)
{
    for(int k = 0;k<database.size();k++)
    {
        Astar astar_2(seq1,database[k]);
        Node* startPos = new Node(0,0);
        Node* endPos = new Node(database[k].size(),seq1.size());
        astar_2.search(startPos,endPos);
        if (PRINT_RES)  astar_2.printRes();
        int current_cost = astar_2.endCost;
        if (current_cost<best_cost)
        {
            best_cost = current_cost;
            idx = k;
            res1 = astar_2.res1;
            res2 = astar_2.res2;
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

    //一共5条query
    for(int i = 0;i<query.size();i++)
    {
        int best_cost = INT_MAX;
        int idx = -1;
        string res1,res2;
        findBestseq(query[i],database,best_cost,idx,res1,res2);
        cout<<"best"<<i<<':'<<database[idx]<<endl;
        if(PRINT_RES)
        {
            cout<<"res"<<i<<"_1:"<<res1<<endl;
            cout<<"res"<<i<<"_2:"<<res2<<endl;
        }
        cout<<"cost"<<i<<':'<<best_cost<<endl;
    }
 

    end = clock();
    cout<<"time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}
