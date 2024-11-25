#include <bits/stdc++.h>
using namespace std;
#define endl '\n'
vector<string> dictionary;//存储每种属性 
unordered_map<string,int> swt;//属性to下标 
unordered_map<string,unordered_set<string> > range_set;//每一种属性取值范围 
struct Data{//数据点 
	int id;//样本点编号 
	vector<string> qualities;//存储每个属性值 
};
struct TreeNode{//树节点 
	string name;//该层结点的划分属性 
	vector<TreeNode*> children;//树的子节点 
	vector<string> classify;//每个分支对应的属性值 
};
vector<Data> training_set;//训练集 
TreeNode *root;//决策树的根节点 

void input(){//输入训练集信息 
	int n,m,k,id;
	string a,s;
	cin>>n;//n个属性
	for(int i=0;i<n;i++){
		cin>>a>>m;//该属性名和属性取值范围大小
		dictionary.push_back(a);
		swt[a]=i;
		for(int j=0;j<m;j++){
			cin>>s;//各属性值
			range_set[a].insert(s);
		}
	}
	vector<string> cur(dictionary.size());
	cin>>k;//k个样本点
	for(int i=0;i<k;i++){
		cin>>id;//此样本点编号
		for(int j=0;j<cur.size();j++){
			cin>>cur[j];//样本各属性值
		}
		training_set.push_back({id,cur});//压入训练集
	}
}

void output(){//输出训练集信息 
	queue<TreeNode*> que;
	vector<TreeNode*> vec;
	que.push(root);
	while(!que.empty()){
		int size=que.size();
		vec.clear();
		for(int i=0;i<size;i++){
			TreeNode* cur=que.front();
			que.pop();
			vec.push_back(cur);
			if(cur->children.size()==0)cout<<"O:";//如果为叶子节点则在前方加上"O:"以区分 
			cout<<cur->name<<" ";
		}
		cout<<endl;
		for(int i=0;i<size;i++){
			int cnt=0;
			for(int j=0;j<vec[i]->children.size();j++){
				cnt++;
				cout<<"branch"<<cnt<<":"<<vec[i]->classify[j]<<" ";//如果输出分支则在前方加上"branch X:"以区分
				que.push(vec[i]->children[j]);
			}
		}
		cout<<endl;
	} 
}

double Ent(const vector<Data> &D){//计算该训练子集的信息熵 
	unordered_map<string,int> mp;
	int n=D.size();
	double result=0,p;
	for(int i=0;i<n;i++){//将样本分类计数 
		mp[D[i].qualities.back()]++;
	}
	for(auto it=mp.begin();it!=mp.end();it++){
		p=it->second*1.0/n;
		result+=p*(log(p)/log(2));
	}
	return -1*result;
}

double Gain(const vector<Data> &D,const string &qua){//计算训练子集信息增益 
	double result=Ent(D);
	int k=swt[qua];
	unordered_map<string,int> mpa;//该属性的各属性值数据点计数 
	unordered_map<string,vector<Data> > mpb;//该属性的各属性值所分割的训练子集 
	int n=D.size();
	for(int i=0;i<n;i++){
		mpa[D[i].qualities[k]]++;
		mpb[D[i].qualities[k]].push_back(D[i]);//分类数据集 
	} 
	for(auto it=mpb.begin();it!=mpb.end();it++){
		double p=mpa[it->first]*1.0/n;
		result-=p*Ent(it->second);
	}
	return result;
}

bool case1(const vector<Data> &D){//遇到情况1 所有样本点属于同一类别
	bool flag=true;
	for(int i=1;i<D.size();i++){
		if(D[i].qualities.back()!=D[i-1].qualities.back()){
			flag=false;
			break;
		}
	}
	return flag;
}

bool case2(const vector<Data> &D,const vector<string> &A){//遇到情况2 没有更多的属性可以用于进一步划分
	if(A.size()==0)return true;
	bool flag=true;
	for(int i=1;i<D.size();i++){
		for(int j=0;j<A.size();j++){
			if(D[i].qualities[swt[A[j]]]!=D[i-1].qualities[swt[A[j]]]){
				flag=false;
				break;
			}
		}
		if(!flag)break;
	}
	return flag;
}

TreeNode* ID3(const vector<Data> &D,vector<string> A){//递归创建决策树 
	TreeNode* node=new TreeNode();
	//情形1||情形2 
	if(case1(D)||case2(D,A)){
		node->name=D[0].qualities.back();
		return node;
	}
	int k=0;
	double maxx=0,t;
	for(int i=0;i<A.size();i++){//找到最优划分属性 
		t=Gain(D,A[i]);
		if(maxx<t){
			maxx=t;
			k=i;
		}
	}
	node->name=A[k]; 
	unordered_map<string,vector<Data> > mp;//依据该属性分割训练集 
	for(int i=0;i<D.size();i++){
		mp[D[i].qualities[swt[A[k]]]].push_back(D[i]);
	}
	A.erase(A.begin()+k,A.begin()+k+1);//移除该属性
	for(auto it=mp.begin();it!=mp.end();it++){
		node->classify.push_back(it->first);//记录分支属性值 
		node->children.push_back(ID3(it->second,A));//递归该训练子集返回子结点 
	}
	return node;//返回该层结点 
}

int main() {
	input();
	vector<string> A=dictionary;
	A.pop_back(); 
	root=ID3(training_set,A);
	output();
	return 0;
}

/*
5
年龄 3 老年 青年 中年
发长 3 短发 中发 长发
鞋跟 2 平底 高跟
服装 3 深色 花色 浅色
性别 2 男 女
30
1       老年    短发    平底    深色    男性
2       老年    短发    平底    浅色    男性
3       老年    中发    平底    花色    女性
4       老年    长发    高跟    浅色    女性
5       老年    短发    平底    深色    男性
6       中年    短发    平底    浅色    男性
7       中年    短发    平底    浅色    男性
8       中年    长发    高跟    花色    女性
9       中年    中发    高跟    深色    女性
10      中年    中发    平底    深色    男性
11      青年    长发    高跟    浅色    女性
12      青年    短发    平底    浅色    女性
13      青年    长发    平底    深色    男性
14      青年    短发    平底    花色    男性
15      青年    中发    高跟    深色    女性
16      中年    短发    平底    深色    男性
17      老年    短发    平底    浅色    男性
18      中年    中发    平底    花色    女性
19      青年    长发    高跟    浅色    女性
20      青年    短发    平底    深色    男性
21      老年    短发    平底    浅色    男性
22      中年    短发    平底    浅色    男性
23      中年    长发    高跟    花色    女性
24      青年    中发    高跟    深色    女性
25      老年    中发    平底    深色    男性
26      中年    长发    高跟    浅色    女性
27      中年    短发    平底    浅色    女性
28      中年    长发    平底    深色    男性
29      中年    短发    平底    花色    男性
30      老年    中发    高跟    深色    女性
*/
