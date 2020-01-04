#include <iostream>
#include <fstream>
#include <vector>
#include<algorithm>
#include <chrono>
#include <string>
#include <queue>


using namespace std;
using namespace std::chrono;

struct item
{
    int id;
    int profit;
    int weight;
};

struct knapsack_problem
{
    vector<item> items;
    int capacity;
    int time_limit = 10; // seconds
};

struct Node
{
    int level, value, bound, weight, id;
};

knapsack_problem read_data(std::string &fn)
{
    ifstream fin(fn);
    if (!fin.good())
    {
        cerr << "Error opening file " << fn << endl;
        exit(-1);
    }
    knapsack_problem ks;
    int items_number;
    fin >> items_number;
    for (int i = 0; i < items_number; i++)
    {
        item an_item;
        fin >> an_item.id;
        fin >> an_item.profit;
        fin >> an_item.weight;
        ks.items.push_back(an_item);
    }
    fin >> ks.capacity;
    return ks;
}

void print_knapsack_problem_info(knapsack_problem &ks)
{
   std:: cout << "Items=" << ks.items.size() << std::endl;
    for (int i = 0; i < ks.items.size(); i++)
    {
        std::cout << "Item=" << ks.items[i].id << " value=" << ks.items[i].profit << " weight=" << ks.items[i].weight << std::endl;
    }
    std::cout << "Capacity=" << ks.capacity << std::endl;
}

int get_profit (knapsack_problem &ks, vector<item> &sol)
{
int total_profit =0;
int total_weight = 0;
for (item an_item:sol)
{
total_weight += an_item.weight;
if (total_weight>ks.capacity)
return -1;
total_profit += an_item.profit;
}
return total_profit;
}

void export_solution(knapsack_problem &ks, vector<item> sol, string fn)
{ 
std:: ofstream ofs(fn);
if (!ofs.good())
{
std:: cerr<< "Error acessing file "<< fn<< std::endl;
exit(-1);
}
std::string output;
int total_weight=0;
int total_profit=0;
for (int i=0; i<sol.size(); i++)
{
output=output + std:: to_string(sol[i].id) + " ";
total_weight += sol[i].weight;
total_profit +=sol[i].profit;
}
output=std:: to_string(total_profit) + " " + std:: to_string(total_weight) + " " + output;
ofs<< output;
ofs.close();
}

std::vector <item> greedy_solver(knapsack_problem &ks)
{
std:: sort(ks.items.begin(), ks.items.end(), [](item &item1, item &item2)
{
return (double) item1.profit/(double) item1.weight > (double) item2.profit/ (double) item2.weight;
});
int total_weight = 0;
std:: vector <item> solution;

for (int i=0; i<ks.items.size(); i++)
{
if (total_weight + ks.items[i].weight >ks.capacity)
break;
solution.push_back(ks.items[i]);
total_weight +=ks.items[i].weight;
}
std:: sort(solution.begin(), solution.end(), [](item &item1, item &item2)
{
return (item1.id < item2.id);
});
return solution;
}

 
vector <item> brute_force_solver(knapsack_problem &ks)
{
    high_resolution_clock :: time_point t1 =high_resolution_clock::now();
    vector <item> result;
    int max_profit=-1;
    vector <vector<item>>sets;
 for (item an_item:ks.items)
    {
        vector <vector<item>>new_sets;
        new_sets.push_back({an_item});
        for (vector<item> a_set : sets)
        {
            a_set.push_back(an_item);
            new_sets.push_back(a_set);
        }
        
    }
    int n=ks.items.size();
    int total=1<<n;
    if (n>=31)
    total=numeric_limits<int>::max();
    for (int i=0; i<total; i++)
    {
        vector<item> sol;
        for (int j=0; j<n; j++)
        {
            if ((i>>j) & 1)
            sol.push_back(ks.items[j]);
        }
        int profit=get_profit(ks,sol);
        if (profit> max_profit)
        {
        max_profit = profit;
        result=sol;
        }
    }
    
        high_resolution_clock :: time_point t2 =high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(t2-t1).count();
      if (duration>ks.time_limit*1000)
      {
        cout<<"Time elapsed:"<<duration<<"microseconds"<<duration/1000.0<<"seconds"<<endl;
      }
       return result;
    
}

vector<item> dynamic_programming_solver(knapsack_problem &ks)
 { 
   int i,w;
   int n=ks.items.size();
   int W= ks.capacity;
   int maxValue;
  
	vector<vector<int>>table(n+1, vector<int>(W+1,0));
    
	    for (w = 0; w <= W; w++) 
        {
            table[0][w]=0;
        }
			for (i=1; i<=n; i++) 
        {
                for (w = 0; w <= W; w++) 
            {
                    if(ks.items[i].weight> w)
				{
                table[i][w] = table[i-1][w];
                
                }
               
                else 
                {
				table[i][w] = max( table[i-1][w], ks.items[i].profit + table[i-1][w-ks.items[i].weight] );
		               }
            }
        }
         
          vector <item> solution;
          
      int j= W;
      for(i=n; i>0; i--)
      
     {
     if (ks.items[i].weight<=j && ks.items[i].profit + table[i-1][j-ks.items[i].weight] >=  table[i-1][j])
      {
        
      solution.push_back(ks.items[i]);
         j = j-ks.items[i].weight;       
      } 
      }
       maxValue = table[n][W];
       return solution;                
}

 int bound(Node u, knapsack_problem &ks)
{  
  int j=0;
  int k=0;
  int value_bound=0;
  int totweight=0;
    if (u.weight >= ks.capacity)
    {
    return 0;
    }
    else
    {
    value_bound=u.value;
    j = u.level +1;
    totweight=u.weight;
    
    while ((j<ks.items.size()) && (totweight + ks.items[j].weight <=ks.capacity))
    {
        totweight +=ks.items[j].weight;
        value_bound += ks.items[j].profit;
        j++;
    }
    k=j;
    if (k < ks.items.size())
    {
    value_bound += (ks.capacity - totweight) * ks.items[j].profit / ks.items[j].weight;
    }
    return value_bound;
    }
}

vector<item> branch_and_bound_solver(knapsack_problem &ks)
{

vector<item> solution;
int n=ks.items.size();
int total_weight=0;
 
    std:: sort(ks.items.begin(), ks.items.end(), [](item &item1, item &item2)
{
return (double) item1.profit/(double) item1.weight > (double) item2.profit/ (double) item2.weight;
});

    for (int i=0; i<n; i++)

{
if (total_weight + ks.items[i].weight <=ks.capacity)
{

solution.push_back(ks.items[i]);
total_weight +=ks.items[i].weight;
}
}
    queue<Node> Q;
    Node u,v;
    Q.empty();
    u.level=-1;
    u.value=u.weight=0;
    Q.push(u);
    int maxProfit=0;
    
    while (!Q.empty())
    {
        u=Q.front();
        Q.pop();
        if (u.level == -1)
        {
        v.level =0;
        }
        else if (u.level != ks.items.size()-1)
        {
         
        v.level = u.level + 1;
        }
        v.weight = u.weight + ks.items[v.level].weight;
        v.value = u.value + ks.items[v.level].profit;
        v.id=ks.items[v.level].id;
        
        if (v.weight <=ks.capacity && v.value > maxProfit)
        {
          maxProfit = v.value;
          total_weight=v.weight;
        }
        v.bound = bound(v, ks);
          if (v.bound > maxProfit)
          {
          Q.push(v);
          }
          v.weight = u.weight;
          v.value = u.value;
          v.id=u.id;
          v.bound = bound(v,ks);
          if (v.bound > maxProfit)
          {
          Q.push(v);
          }
    }  
 return solution;  
}


void solve_GR()
{ 
string csv_path = "../helloworld/resultsGREEDY.csv";
remove(csv_path.c_str());
std::ofstream file;
file.open(csv_path, std::ofstream::out);
string header="INSTANCE";
header.append(";VALUE_GR;WEIGHT_GR;EXECUTION_TIME_GR");
header.append("\n");
file<<header;
for (int n :{10, 50, 100, 500})
    for (int r: {50, 100, 500, 1000})
        for (int type: {1,2,3,4})
           for (int instance_id=1; instance_id<=5; instance_id++)
  { string problem_instance="problem_" + to_string(n) + "_" + to_string(r) + "_" + to_string(type) +"_" + to_string(instance_id);
    string out= problem_instance;
    out.append(";");
file<<out;
string path_fn="../helloworld/";
    path_fn.append(problem_instance);
    path_fn.append("_5.txt");
    knapsack_problem ks =read_data(path_fn);  
    std::vector<item> solution;
    auto t1 =chrono::high_resolution_clock::now();
    solution=greedy_solver(ks);
    string output;
      int total_weight=0;
    int total_profit=0;
    for (int i=0; i<solution.size(); i++)
{
total_weight += solution[i].weight;
total_profit +=solution[i].profit;
}
output=std:: to_string(total_profit) + ";" + std:: to_string(total_weight) +";";
   auto t2 =chrono::high_resolution_clock::now();
chrono::duration<double,milli>duration = t2-t1;
   string x= to_string(duration.count() / 1000.0);
  output.append(x);
   file<<output<<endl;
  }
  file.close();
}

 void solve_DP()
{ 
string csv_path = "../helloworld/resultsDYNAMICPROG.csv";
remove(csv_path.c_str());
std::ofstream file;
file.open(csv_path, std::ofstream::out);
string header="INSTANCE";
header.append(";VALUE_DP;WEIGHT_DP;EXECUTION_TIME_DP");
header.append("\n");
file<<header;
for (int n :{10, 50, 100, 500})
    for (int r: {50, 100, 500, 1000})
        for (int type: {1,2,3,4})
           for (int instance_id=1; instance_id<=5; instance_id++)
  { string problem_instance="problem_" + to_string(n) + "_" + to_string(r) + "_" + to_string(type) +"_" + to_string(instance_id);
    string out= problem_instance;
    out.append(";");
file<<out;

 string path_fn="../helloworld/";
    path_fn.append(problem_instance);
    path_fn.append("_5.txt");
    knapsack_problem ks =read_data(path_fn);  
    std::vector<item> solution;
  auto t1 =chrono::high_resolution_clock::now();
   solution=dynamic_programming_solver(ks);
    string output; 
      int total_weight=0;
    int total_profit=0;
    for (int i=0; i<solution.size(); i++)
{
total_weight += solution[i].weight;
total_profit +=solution[i].profit;
}

output=std:: to_string(total_profit) + ";" + std:: to_string(total_weight) +";";
 auto t2 =chrono::high_resolution_clock::now();
chrono::duration<double,milli>duration = t2-t1;
   string x= to_string(duration.count() / 1000.0);
  output.append(x);
   file<<output<<endl;
  }
  file.close();
}

void solve_BF()
{  
string csv_path = "../helloworld/resultsBRUTEFORCE.csv";
remove(csv_path.c_str());
std::ofstream file;
file.open(csv_path, std::ofstream::out);
string header="INSTANCE";
header.append(";VALUE_BF;WEIGHT_BF;EXECUTION_TIME_BF");
header.append("\n");
file<<header;
for (int n :{10, 50, 100, 500})
    for (int r: {50, 100, 500, 1000})
        for (int type: {1,2,3,4})
           for (int instance_id=1; instance_id<=5; instance_id++)
  { string problem_instance="problem_" + to_string(n) + "_" + to_string(r) + "_" + to_string(type) +"_" + to_string(instance_id);
    string out= problem_instance;
    out.append(";");
file<<out;

 string path_fn="../helloworld/";
    path_fn.append(problem_instance);
    path_fn.append("_5.txt");
    knapsack_problem ks =read_data(path_fn);  
    std::vector<item> solution;
  auto t1 =chrono::high_resolution_clock::now();
   solution=brute_force_solver(ks);
    string output; 
      int total_weight=0;
    int total_profit=0;
    for (int i=0; i<solution.size(); i++)
{
total_weight += solution[i].weight;
total_profit +=solution[i].profit;
}

output=std:: to_string(total_profit) + ";" + std:: to_string(total_weight) +";";
 auto t2 =chrono::high_resolution_clock::now();
chrono::duration<double,milli>duration = t2-t1;
   string x= to_string(duration.count() / 1000.0);
  output.append(x);
   file<<output<<endl;
  }
  file.close();
}

void solve_BNB()
{ 
string csv_path = "../helloworld/resultsBRANCHBOUND.csv";
remove(csv_path.c_str());
std::ofstream file;
file.open(csv_path, std::ofstream::out);
string header="INSTANCE";
header.append(";VALUE_BNB;WEIGHT_BNB;EXECUTION_TIME_BNB");
header.append("\n");
file<<header;
for (int n :{10, 50, 100, 500})
    for (int r: {50, 100, 500, 1000})
        for (int type: {1,2,3,4})
           for (int instance_id=1; instance_id<=5; instance_id++)
  { string problem_instance="problem_" + to_string(n) + "_" + to_string(r) + "_" + to_string(type) +"_" + to_string(instance_id);
    string out= problem_instance;
    out.append(";");
file<<out;

 string path_fn="../helloworld/";
    path_fn.append(problem_instance);
    path_fn.append("_5.txt");
    knapsack_problem ks =read_data(path_fn);  
    std::vector<item> solution;
  auto t1 =chrono::high_resolution_clock::now();
   solution=branch_and_bound_solver(ks);
    string output; 
      int total_weight=0;
    int total_profit=0;
    for (int i=0; i<solution.size(); i++)
{
total_weight += solution[i].weight;
total_profit +=solution[i].profit;
}

output=std:: to_string(total_profit) + ";" + std:: to_string(total_weight) +";";
 auto t2 =chrono::high_resolution_clock::now();
chrono::duration<double,milli>duration = t2-t1;
   string x= to_string(duration.count() / 1000.0);
  output.append(x);
   file<<output<<endl;
  }
  file.close();
}

int main(int argc, char **argv)
{
      if(argc!=2) 
    {
		cerr<<" wrong number of arguments"<<endl;
		return 0;
	}
 int c=atoi(argv[1]);
 switch (c)
  {    

 case 1:
   
     for (int n :{10, 50, 100, 500})
   for (int r: {50, 100, 500, 1000})
   for (int type: {1,2,3,4})
   for (int instance_id=1; instance_id<=5; instance_id++)
  { string problem_instance="problem_" + to_string(n) + "_" + to_string(r) + "_" + to_string(type) +"_" + to_string(instance_id);
    string out= problem_instance;
    out.append("_GR");
    string path_fn="../helloworld/";
    path_fn.append(problem_instance);
    path_fn.append("_5.txt");
    knapsack_problem ks =read_data(path_fn); 
    print_knapsack_problem_info(ks);
    std::vector<item> solution;
    solution=greedy_solver(ks);
    get_profit(ks,solution);
    export_solution(ks,solution,out);  
    
   }
break;

 case 2:
   
   for (int n :{10, 50, 100, 500})
   for (int r: {50, 100, 500, 1000})
   for (int type: {1,2,3,4})
   for (int instance_id=1; instance_id<=5; instance_id++)
  { string problem_instance="problem_" + to_string(n) + "_" + to_string(r) + "_" + to_string(type) +"_" + to_string(instance_id);
    string out= problem_instance;
    out.append("_BF");
    string path_fn="../helloworld/";
    path_fn.append(problem_instance);
    path_fn.append("_5.txt");
    knapsack_problem ks =read_data(path_fn); 
    print_knapsack_problem_info(ks);
    std::vector<item> solution;
    solution=brute_force_solver(ks);
    get_profit(ks,solution);
    export_solution(ks,solution,out);  
    }  
    break;

case 3:

   for (int n :{10, 50, 100, 500})
   for (int r: {50, 100, 500, 1000})
   for (int type: {1,2,3,4})
   for (int instance_id=1; instance_id<=5; instance_id++)
  { string problem_instance="problem_" + to_string(n) + "_" + to_string(r) + "_" + to_string(type) +"_" + to_string(instance_id);
   string out= problem_instance;
    out.append("_DP");
    string path_fn="../helloworld/";
    path_fn.append(problem_instance);
    path_fn.append("_5.txt");
    knapsack_problem ks =read_data(path_fn); 
    print_knapsack_problem_info(ks);
    std::vector<item> solution;
    solution=dynamic_programming_solver(ks);
    get_profit(ks,solution);
    export_solution(ks,solution,out);  
  }
    break;

 case 4 :

   for (int n :{10, 50, 100, 500})
   for (int r: {50, 100, 500, 1000})
   for (int type: {1,2,3,4})
   for (int instance_id=1; instance_id<=5; instance_id++)
  { string problem_instance="problem_" + to_string(n) + "_" + to_string(r) + "_" + to_string(type) +"_" + to_string(instance_id);
 string out= problem_instance;
    out.append("_BNB");
    string path_fn="../helloworld/";
    path_fn.append(problem_instance);
    path_fn.append("_5.txt");
    knapsack_problem ks =read_data(path_fn); 
    print_knapsack_problem_info(ks);
    std::vector<item> solution;
    solution=branch_and_bound_solver(ks);
    get_profit(ks,solution);
    export_solution(ks,solution,out);  

  }
break;

 case 5:

   solve_GR();
break;

case 6:

   solve_DP();
  break;

case 7:
    
    solve_BF();

break;

case 8:
 
    solve_BNB();
break;

    default:
cerr<<"solver should be a number in [1,8]"<<endl;
exit(-1);
 }
}
       
 
