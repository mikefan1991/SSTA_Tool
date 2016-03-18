#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include "delay.h"
using namespace std;
#define pi 3.1415926535
#define random(x)(rand()%x)

void lib_init(vector<WIRE> &w,char *filename)
{
  GATE g;
  g={"NAND",1,1,10,1.1,2.2,3.3};
  gate_lib.push_back(g);
  g={"NAND",2,1.3,7,1.4,2.8,4.1};
  gate_lib.push_back(g);
  g={"NAND",3,1.5,3,1.8,2.5,3.9};
  gate_lib.push_back(g);
  //g={"NAND",4,0.5,50,1.9,2.3,3.6};
  //gate_lib.push_back(g);
  g={"AND",1,1.2,9,1.1,2.2,3.3};
  gate_lib.push_back(g);
  g={"AND",2,0.8,11,1.3,2.4,3.9};
  gate_lib.push_back(g);
  g={"NOR",1,1.1,10,1.3,2.0,4.1};
  gate_lib.push_back(g);
  g={"NOR",2,1.4,8,1.3,2.2,3.6};
  gate_lib.push_back(g);
  g={"OR",1,1,11.4,0.7,1.6,5.1};
  gate_lib.push_back(g);
  g={"OR",2,0.8,11,0.9,1.4,4.9};
  gate_lib.push_back(g);
  g={"OR",3,1.4,10,1.5,1.9,2.9};
  gate_lib.push_back(g);
  g={"XOR",1,1.3,18,1.2,1.3,2.6};
  gate_lib.push_back(g);
  g={"XOR",2,1.5,16,1.5,2.1,3.1};
  gate_lib.push_back(g);
  g={"XNOR",1,1.4,17,2.1,1.9,0.6};
  gate_lib.push_back(g);
  g={"XNOR",2,1.1,20,1.9,2.3,1.1};
  gate_lib.push_back(g);
  g={"BUFF",1,0.5,2,0.1,0.1,0.1};
  gate_lib.push_back(g);
  g={"BUFF",2,0.6,4,0.2,0.2,0.4};
  gate_lib.push_back(g);
  g={"BUFF",3,0.4,2,0.3,0.2,0.5};
  gate_lib.push_back(g);
  g={"NOT",1,0.8,3,0.3,0.2,0.4};
  gate_lib.push_back(g);
  g={"NOT",2,0.6,2,0.1,0.2,0.6};
  gate_lib.push_back(g);
  g={"NOT",3,0.4,4,0.2,0.3,1.0};
  gate_lib.push_back(g);

  ifstream tf(filename);
  string s;
  stringstream ss;
  for(int i=0;i<5;i++)
    getline(tf,s);
  while(getline(tf,s))
  {
    WIRE newwire;
    
    if(s.size()>0)
    {
      int i;
      //find start wire
      for(i=0;s[i]!='\t';i++);
      ss.clear();
      ss<<s.substr(1,i-1);
      ss>>newwire.start;
      //find stop wire
      int l;
      i=i+2;
      for(l=i;s[i]!='\t';i++);
      ss.clear();
      ss<<s.substr(l,i-l);
      ss>>newwire.stop;
      
      for(l=++i;s[i]!='\t';i++);
      ss.clear();
      ss<<s.substr(l,i-l);
      ss>>newwire.d0;
      for(l=++i;s[i]!='\t';i++);
      ss.clear();
      ss<<s.substr(l,i-l);
      ss>>newwire.d1;
      for(l=++i;s[i]!='\t';i++);
      ss.clear();
      ss<<s.substr(l,i-l);
      ss>>newwire.d2;
      l=i+1;
      ss.clear();
      ss<<s.substr(l);
      ss>>newwire.dR;
      
      w.push_back(newwire);
    }
  }
  tf.close();
}

int getindex(string s)
{
  int idx;
  int l,r;
  for(unsigned int i=0;i<s.size();i++)
  {
    if(s[i]=='G')
      l=i+1;
    if(s[i]==')')
      r=i;
  }
  stringstream ss;
  string t=s.substr(l,r-l);
  ss<<t;
  ss>>idx;
  return idx;
}

void swap(VERTEX &A,VERTEX &B)
{
  VERTEX temp=A;
  A=B;
  B=temp;
}

VERTEX ADD(VERTEX A,string OP,VERTEX &tar)
{
  for(unsigned int i=0;i<gate_lib.size()&&OP!="null";i++)
  {
    if(gate_lib[i].type==OP&&gate_lib[i].model==1)
    {
      tar.gate=&gate_lib[i];
      break;
    }
  }
  
  A.a0+=tar.gate->d0;
  A.a1+=tar.gate->d1;
  A.a2+=tar.gate->d2;
  A.aR=sqrt(A.aR*A.aR+tar.gate->dR*tar.gate->dR);
  return A;
}

VERTEX ADD_wire_delay(VERTEX A,WIRE w)
{
  A.a0+=w.d0;
  A.a1+=w.d1;
  A.a2+=w.d2;
  A.aR=sqrt(A.aR*A.aR+w.dR*w.dR);
  return A;
}

VERTEX MAX(VERTEX A,VERTEX B)
{
  if(A.a0==B.a0&&A.a1==B.a1&&A.a2==B.a2&&A.aR==B.aR)
    return A;
  double m1,m2,m3;
  m1=A.a1*A.a1+A.a2*A.a2+A.aR*A.aR;
  m2=A.a1*B.a1+A.a2*B.a2;
  m3=B.a1*B.a1+B.a2*B.a2+B.aR*B.aR;
  double theta=sqrt(m1+m3-2*m2);
  double T=0.5+0.5*erf((A.a0-B.a0)/theta/sqrt(2));
  VERTEX C;
  C.a0=A.a0*T+B.a0*(1-T)+theta/sqrt(2*pi)*exp(-(A.a0-B.a0)/theta*(A.a0-B.a0)/theta/2);
  C.a1=T*A.a1+(1-T)*B.a1;
  C.a2=T*A.a2+(1-T)*B.a2;
  double var=(m1+A.a0*A.a0)*T+(m3+B.a0*B.a0)*(1-T)+(A.a0+B.a0)*theta/sqrt(2*pi)*exp(-(A.a0-B.a0)/theta*(A.a0-B.a0)/theta/2)-C.a0*C.a0;
  C.aR=sqrt(var-C.a1*C.a1-C.a2*C.a2);
  return C;
}

void optimizationpath(map<int,VERTEX> &v_map,int idx,vector<int> path)
{
  bool inpath=false;
  for(unsigned int i=0;i<path.size();i++)
  {
    if(path[i]==idx)
    {
      inpath=true;
      break;
    }
  }
  if(inpath)
  {
    for(unsigned int i=0;i<gate_lib.size();i++)
    {
      if(gate_lib[i].type==v_map[idx].gate->type&&gate_lib[i].model!=v_map[idx].gate->model)
	if(gate_lib[i].d0<v_map[idx].gate->d0||(gate_lib[i].d0==v_map[idx].gate->d0&&gate_lib[i].cost<v_map[idx].gate->cost))
	  v_map[idx].gate=&gate_lib[i];
    }
  }
}

void optimizationall(map<int,VERTEX> &v_map,int idx)
{
  for(unsigned int i=0;i<gate_lib.size();i++)
  {
    if(gate_lib[i].type==v_map[idx].gate->type&&gate_lib[i].model!=v_map[idx].gate->model)
      if(v_map[idx].gate->cost>gate_lib[i].cost)
	v_map[idx].gate=&gate_lib[i];
  }
}

void rerun(map<int,VERTEX> &v_map,vector<int> &path,double &cost,vector<WIRE> wire_lib,vector<int> order,vector<int> o_idx)
{
  cost=0;
  for(unsigned int i=0;i<order.size();i++)
  {
    vector<VERTEX> input;
    int tout_idx=order[i];
    for(unsigned int j=0;j<v_map[tout_idx].in.size();j++)
    {
      int temp_idx=v_map[tout_idx].in[j];
      input.push_back(v_map[temp_idx]);
      for(unsigned int k=0;k<wire_lib.size();k++)
	if(wire_lib[k].start==temp_idx&&wire_lib[k].stop==tout_idx)
        {
	  input[j]=ADD_wire_delay(input[j],wire_lib[k]);
	}
      input[j]=ADD(input[j],"null",v_map[tout_idx]);
    }
    cost+=v_map[tout_idx].gate->cost;
    if(input.size()==1)
    {
      v_map[tout_idx].a0=input[0].a0;
      v_map[tout_idx].a1=input[0].a1;
      v_map[tout_idx].a2=input[0].a2;
      v_map[tout_idx].aR=input[0].aR;
      v_map[tout_idx].dad=input[0].index;
      v_map[tout_idx].cost=cost;
      continue;
    }
    unsigned int m,n;
    for(m=0;m<input.size()-1;m++)
      for(n=m+1;n<input.size();n++)
	if(input[m].a0>input[n].a0)
	  swap(input[m],input[n]);
    v_map[tout_idx].dad=input[m].index;
    for(m=0,n=1;n<input.size();m++,n++)
      input[n]=MAX(input[m],input[n]);
    v_map[tout_idx].a0=input[m].a0;
    v_map[tout_idx].a1=input[m].a1;
    v_map[tout_idx].a2=input[m].a2;
    v_map[tout_idx].aR=input[m].aR;
    v_map[tout_idx].cost=cost;
  }
  path.clear();
  double max=0;
  int max_outidx;
  for(unsigned int i=0;i<o_idx.size();i++)
  {
    if(v_map[o_idx[i]].a0>max)
    {
      max=v_map[o_idx[i]].a0;
      max_outidx=o_idx[i];
    }
    //cout<<"G"<<o_idx[i]<<" a0 a1 a2 ar "<<v_map[o_idx[i]].a0<<" "<<v_map[o_idx[i]].a1<<" "<<v_map[o_idx[i]].a2<<" "<<v_map[o_idx[i]].aR<<endl;
  }
  path.push_back(max_outidx);
  int curr=max_outidx;
  while(v_map[curr].dad)
  {
    curr=v_map[curr].dad;
    path.push_back(curr);
  }
  
}

int main(int args, char **argv)
{
  ifstream cf;
  map<int,VERTEX> v_map;
  vector<WIRE> wire_lib;
  vector<int> i_idx;//save all input indexes
  vector<int> o_idx;//save all output indexes
  vector<int> order;//rerun the SSTA
  int max_idx=0;
  double cost=0;
  cf.open(argv[1]);
  lib_init(wire_lib,argv[2]);//initialize all gates and wires

  string s;
  for(int i=0;i<6;i++)
    getline(cf,s);
  int inputnum=0,outputnum=0;
  while(getline(cf,s))
  {
    if(s[0]=='I')//the line is an input format
    {
      inputnum++;
      VERTEX v;
      int idx=getindex(s);
      v.a0=0;
      v.a1=0;
      v.a2=0;
      v.aR=0;
      v.cost=0;
      v.index=idx;
      v.dad=0;
      v.gate=NULL;
      v_map.insert(pair<int,VERTEX>(idx,v));
      i_idx.push_back(idx);
    }
    else if(s[0]=='O')//the line is an output format
    {
      outputnum++;
      int idx=getindex(s);
      if(idx>max_idx)
	max_idx=idx;
      o_idx.push_back(idx);
    }
    else if(s[0]=='G')//the line is a gate format
    {
      string OP;
      int t_idx;
      VERTEX tar;
      unsigned int i,j;
      for(i=0;s[i]!=' ';i++);//find out output index of the gate
      stringstream ss;
      ss<<s.substr(1,i-1);
      ss>>t_idx;
      tar.index=t_idx;
      order.push_back(t_idx);

      int flag;
      for(flag=i+3;s[i]!='(';i++);//find out operation in the string
      OP=s.substr(flag,i-flag);
      vector<VERTEX> input;//used for each gate
      for(flag=0;i<s.size();i++)//find out all input indexes of the gate
      {
	if(s[i]=='G')
	  flag=i;
	else if(s[i]==','||s[i]==')')
	{
	  ss.clear();
	  ss<<s.substr(flag+1,i-flag-1);
	  int temp_idx;
	  ss>>temp_idx;
	  input.push_back(v_map[temp_idx]);
	  tar.in.push_back(temp_idx);
	  v_map[temp_idx].out.push_back(t_idx);
	}
      }
      //add wire delay and gate delay to vertex
      for(i=0,tar.cost=0;i<input.size();i++)
      {
	for(unsigned int j=0;j<wire_lib.size();j++)
	{
	  if(wire_lib[j].start==input[i].index&&wire_lib[j].stop==tar.index)
	  {
	    input[i]=ADD_wire_delay(input[i],wire_lib[j]);
	    break;
	  }
	}
	input[i]=ADD(input[i],OP,tar);
      }
      cost+=tar.gate->cost;
      if(input.size()==1)//the input of a gate contains only one vertex
      {
	tar.a0=input[0].a0;
	tar.a1=input[0].a1;
	tar.a2=input[0].a2;
	tar.aR=input[0].aR;
	tar.dad=input[0].index;
	tar.cost=cost;
	v_map[t_idx]=tar;
	continue;
      }
      //sort the inputs respect to a0
      for(i=0;i<input.size()-1;i++)
	for(j=i+1;j<input.size();j++)
	  if(input[i].a0>input[j].a0)
	    swap(input[i],input[j]);
      tar.dad=input[i].index;
      //find the max distribution in the sorted inputs
      for(i=0,j=1;j<input.size();i++,j++)
      {
	input[j]=MAX(input[i],input[j]);
      }
      tar.a0=input[i].a0;
      tar.a1=input[i].a1;
      tar.a2=input[i].a2;
      tar.aR=input[i].aR;
      tar.cost=cost;
      v_map[t_idx]=tar;
    }
  }

  vector<int> path;
  double max=0;
  int max_outidx;
  //find the output with biggest AT and print out all ATs of outputs
  for(unsigned int i=0;i<o_idx.size();i++)
  {
    if(v_map[o_idx[i]].a0>max)
    {
      max=v_map[o_idx[i]].a0;
      max_outidx=o_idx[i];
    }
    cout<<"G"<<o_idx[i]<<" a0 a1 a2 ar "<<v_map[o_idx[i]].a0<<" "<<v_map[o_idx[i]].a1<<" "<<v_map[o_idx[i]].a2<<" "<<v_map[o_idx[i]].aR<<endl;
  }
  //derive the critical path
  path.push_back(max_outidx);
  int curr=max_outidx;
  while(v_map[curr].dad)
  {
    curr=v_map[curr].dad;
    path.push_back(curr);
  }
  cout<<"Critical Path: ";
  for(unsigned int i=path.size()-1;i>0;i--)
    cout<<"G"<<path[i]<<"->";
  cout<<"G"<<path[0]<<endl;
  cout<<"cost = "<<cost<<endl;

  cout<<endl<<"Do you wanna do optimization?(y/n)";
  char ch=getchar();
  switch(ch)
  {
  case 'y':
    cout<<"After optimization:"<<endl;
    if(max_idx>500)//it is a large circuit
    {
      //reduce all gates' cost
      for(unsigned int i=0;i<order.size();i++)
	optimizationall(v_map,order[i]);
      rerun(v_map,path,cost,wire_lib,order,o_idx);
      //reduce delay of critical path
      while(1)
      {
	for(unsigned int i=0;i<order.size();i++)
	  optimizationpath(v_map,order[i],path);
	double old_cost=cost;
	rerun(v_map,path,cost,wire_lib,order,o_idx);
	if(old_cost==cost)
	  break;
      }
    }
    else//it is a small circuit
    {
      double T=1.00;//temperature
      double T_min=0.0000001;
      double alpha=0.95;
      while(T>T_min)
      {
	int random_idx=random(order.size());
	map<int,VERTEX> old_map=v_map;
	double old_cost=cost;
	double old_max=max;
	unsigned int gate_idx;
	//find the gate index of the current gate model
	for(unsigned int i=0;i<gate_lib.size();i++)
	  if(gate_lib[i].type==v_map[order[random_idx]].gate->type&&gate_lib[i].model==v_map[order[random_idx]].gate->model)
	  {
	    gate_idx=i;
	    break;
	  }
	//change the gate model
	for(unsigned int i=(gate_idx+1)%gate_lib.size();i!=gate_idx;i++,i%=gate_lib.size())
	  if(gate_lib[i].type==v_map[order[random_idx]].gate->type&&gate_lib[i].model!=v_map[order[random_idx]].gate->model)
	  {
	    v_map[order[random_idx]].gate=&gate_lib[i];
	    rerun(v_map,path,cost,wire_lib,order,o_idx);
	    break;
	  }
	max=0;
	for(unsigned int i=0;i<o_idx.size();i++)
	  if(v_map[o_idx[i]].a0>max)
	    max=v_map[o_idx[i]].a0;
	double p=exp((old_cost-cost)/T);//accept probability for cost
	double q=exp((old_max-max)/T);//accept probability for delay
	if(p>1) p=1;
	if(q>1) q=1;
	double x=random(100);
	double y=random(100);

	if((p<1&&(p*100<x))||(q<1&&(q*100<y)))//new solution not accepted
	{
	  v_map=old_map;
	  rerun(v_map,path,cost,wire_lib,order,o_idx);
	}
	if(cost-old_cost<2&&max-old_max<5)//temperature cool down only if cost and delay both do not increase too much
	  T*=alpha;
      }
    }

    for(unsigned int i=0;i<o_idx.size();i++)
      cout<<"G"<<o_idx[i]<<" a0 a1 a2 ar "<<v_map[o_idx[i]].a0<<" "<<v_map[o_idx[i]].a1<<" "<<v_map[o_idx[i]].a2<<" "<<v_map[o_idx[i]].aR<<endl;
    cout<<"Critical Path: ";
    for(unsigned int i=path.size()-1;i>0;i--)
      cout<<"G"<<path[i]<<"->";
    cout<<"G"<<path[0]<<endl;
    cout<<"cost = "<<cost<<endl;
    break;
  case 'n':
    cout<<"Goodbye!"<<endl;
    break;
  }
  cf.close();
  return 0;
}
