#include <bits/stdc++.h>
using namespace std;
#define endl '\n'
vector<string> dictionary;//�洢ÿ������ 
unordered_map<string,int> swt;//����to�±� 
unordered_map<string,unordered_set<string> > range_set;//ÿһ������ȡֵ��Χ 
struct Data{//���ݵ� 
	int id;//�������� 
	vector<string> qualities;//�洢ÿ������ֵ 
};
struct TreeNode{//���ڵ� 
	string name;//�ò���Ļ������� 
	vector<TreeNode*> children;//�����ӽڵ� 
	vector<string> classify;//ÿ����֧��Ӧ������ֵ 
};
vector<Data> training_set;//ѵ���� 
TreeNode *root;//�������ĸ��ڵ� 

void input(){//����ѵ������Ϣ 
	int n,m,k,id;
	string a,s;
	cin>>n;//n������
	for(int i=0;i<n;i++){
		cin>>a>>m;//��������������ȡֵ��Χ��С
		dictionary.push_back(a);
		swt[a]=i;
		for(int j=0;j<m;j++){
			cin>>s;//������ֵ
			range_set[a].insert(s);
		}
	}
	vector<string> cur(dictionary.size());
	cin>>k;//k��������
	for(int i=0;i<k;i++){
		cin>>id;//����������
		for(int j=0;j<cur.size();j++){
			cin>>cur[j];//����������ֵ
		}
		training_set.push_back({id,cur});//ѹ��ѵ����
	}
}

void output(){//���ѵ������Ϣ 
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
			if(cur->children.size()==0)cout<<"O:";//���ΪҶ�ӽڵ�����ǰ������"O:"������ 
			cout<<cur->name<<" ";
		}
		cout<<endl;
		for(int i=0;i<size;i++){
			int cnt=0;
			for(int j=0;j<vec[i]->children.size();j++){
				cnt++;
				cout<<"branch"<<cnt<<":"<<vec[i]->classify[j]<<" ";//��������֧����ǰ������"branch X:"������
				que.push(vec[i]->children[j]);
			}
		}
		cout<<endl;
	} 
}

double Ent(const vector<Data> &D){//�����ѵ���Ӽ�����Ϣ�� 
	unordered_map<string,int> mp;
	int n=D.size();
	double result=0,p;
	for(int i=0;i<n;i++){//������������� 
		mp[D[i].qualities.back()]++;
	}
	for(auto it=mp.begin();it!=mp.end();it++){
		p=it->second*1.0/n;
		result+=p*(log(p)/log(2));
	}
	return -1*result;
}

double Gain(const vector<Data> &D,const string &qua){//����ѵ���Ӽ���Ϣ���� 
	double result=Ent(D);
	int k=swt[qua];
	unordered_map<string,int> mpa;//�����Եĸ�����ֵ���ݵ���� 
	unordered_map<string,vector<Data> > mpb;//�����Եĸ�����ֵ���ָ��ѵ���Ӽ� 
	int n=D.size();
	for(int i=0;i<n;i++){
		mpa[D[i].qualities[k]]++;
		mpb[D[i].qualities[k]].push_back(D[i]);//�������ݼ� 
	} 
	for(auto it=mpb.begin();it!=mpb.end();it++){
		double p=mpa[it->first]*1.0/n;
		result-=p*Ent(it->second);
	}
	return result;
}

bool case1(const vector<Data> &D){//�������1 ��������������ͬһ���
	bool flag=true;
	for(int i=1;i<D.size();i++){
		if(D[i].qualities.back()!=D[i-1].qualities.back()){
			flag=false;
			break;
		}
	}
	return flag;
}

bool case2(const vector<Data> &D,const vector<string> &A){//�������2 û�и�������Կ������ڽ�һ������
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

TreeNode* ID3(const vector<Data> &D,vector<string> A){//�ݹ鴴�������� 
	TreeNode* node=new TreeNode();
	//����1||����2 
	if(case1(D)||case2(D,A)){
		node->name=D[0].qualities.back();
		return node;
	}
	int k=0;
	double maxx=0,t;
	for(int i=0;i<A.size();i++){//�ҵ����Ż������� 
		t=Gain(D,A[i]);
		if(maxx<t){
			maxx=t;
			k=i;
		}
	}
	node->name=A[k]; 
	unordered_map<string,vector<Data> > mp;//���ݸ����Էָ�ѵ���� 
	for(int i=0;i<D.size();i++){
		mp[D[i].qualities[swt[A[k]]]].push_back(D[i]);
	}
	A.erase(A.begin()+k,A.begin()+k+1);//�Ƴ�������
	for(auto it=mp.begin();it!=mp.end();it++){
		node->classify.push_back(it->first);//��¼��֧����ֵ 
		node->children.push_back(ID3(it->second,A));//�ݹ��ѵ���Ӽ������ӽ�� 
	}
	return node;//���ظò��� 
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
���� 3 ���� ���� ����
���� 3 �̷� �з� ����
Ь�� 2 ƽ�� �߸�
��װ 3 ��ɫ ��ɫ ǳɫ
�Ա� 2 �� Ů
30
1       ����    �̷�    ƽ��    ��ɫ    ����
2       ����    �̷�    ƽ��    ǳɫ    ����
3       ����    �з�    ƽ��    ��ɫ    Ů��
4       ����    ����    �߸�    ǳɫ    Ů��
5       ����    �̷�    ƽ��    ��ɫ    ����
6       ����    �̷�    ƽ��    ǳɫ    ����
7       ����    �̷�    ƽ��    ǳɫ    ����
8       ����    ����    �߸�    ��ɫ    Ů��
9       ����    �з�    �߸�    ��ɫ    Ů��
10      ����    �з�    ƽ��    ��ɫ    ����
11      ����    ����    �߸�    ǳɫ    Ů��
12      ����    �̷�    ƽ��    ǳɫ    Ů��
13      ����    ����    ƽ��    ��ɫ    ����
14      ����    �̷�    ƽ��    ��ɫ    ����
15      ����    �з�    �߸�    ��ɫ    Ů��
16      ����    �̷�    ƽ��    ��ɫ    ����
17      ����    �̷�    ƽ��    ǳɫ    ����
18      ����    �з�    ƽ��    ��ɫ    Ů��
19      ����    ����    �߸�    ǳɫ    Ů��
20      ����    �̷�    ƽ��    ��ɫ    ����
21      ����    �̷�    ƽ��    ǳɫ    ����
22      ����    �̷�    ƽ��    ǳɫ    ����
23      ����    ����    �߸�    ��ɫ    Ů��
24      ����    �з�    �߸�    ��ɫ    Ů��
25      ����    �з�    ƽ��    ��ɫ    ����
26      ����    ����    �߸�    ǳɫ    Ů��
27      ����    �̷�    ƽ��    ǳɫ    Ů��
28      ����    ����    ƽ��    ��ɫ    ����
29      ����    �̷�    ƽ��    ��ɫ    ����
30      ����    �з�    �߸�    ��ɫ    Ů��
*/
