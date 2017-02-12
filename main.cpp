#include <bits/stdc++.h>
using namespace std;


#define BUCKET_SIZE 2
#define INITIAL_GLOBAL_DEPTH 1


class Bucket {
        int depth,size;
        std::map<int, string> values;
    public:
        Bucket(int depth, int size);
        int insert(int key,string value);
        int remove(int key);
        int update(int key, string value);
        void search(int key);
        int isFull(void);
        int isEmpty(void);
        int getDepth(void);
        int increaseDepth(void);
        int decreaseDepth(void);
        std::map<int, string> copy(void);
        void clear(void);
        void display(void);
};

class Directory {
        int global_depth, bucket_size;
        std::vector<Bucket*> buckets;
        int hash(int n);
        int pairIndex(int bucket_no, int depth);
        void grow(void);
        void shrink(void);
        void split(int bucket_no);
        void merge(int bucket_no);
        string bucket_id(int n);
    public:
        Directory(int depth, int bucket_size);
        void insert(int key,string value,bool reinserted);
        void remove(int key,int mode);
        void update(int key, string value);
        void search(int key);
        void display(void);
};


void input_process(Directory d,bool show_messages);
int menu(bool show_messages);



/* Main function */

int main()
{
    Directory d(INITIAL_GLOBAL_DEPTH,BUCKET_SIZE);
    input_process(d,1);
    return 0;
}

/* Utility function to take input and call function accordingly */

void input_process(Directory d,bool show_messages)
{
    int choice, key, mode;
    string value;
    do
    {
        choice = menu(show_messages);
        cout<<endl;
        switch(choice)
        {
            case 1:
                if(show_messages) { cout<<"Key : "; }
                cin>>key;
                if(show_messages) { cout<<"Value : "; }
                cin>>value;
                if(show_messages) { cout<<endl; }
                d.insert(key,value,0);
                break;
            case 2:
                if(show_messages) { cout<<"Key : "; }
                cin>>key;
                if(show_messages) { cout<<"Mode ( 0-Lazy / 1-Merge empty / 2-Shrink directory / 3-Both merge and shrink ) : "; }
                cin>>mode;
                if(show_messages) { cout<<endl; }
                d.remove(key,mode);
                break;
            case 3:
                if(show_messages) { cout<<"Key : "; }
                cin>>key;
                if(show_messages) { cout<<"Value : "; }
                cin>>value;
                if(show_messages) { cout<<endl; }
                d.update(key,value);
                break;
            case 4:
                if(show_messages) { cout<<"Key : "; }
                cin>>key;
                if(show_messages) { cout<<endl; }
                d.search(key);
                break;
            case 5:
                d.display();
                break;
        }
    } while(choice!=0);
}

/* Input choice in main */

int menu(bool show_messages)
{
    int choice;
    if(show_messages)
    {
        cout<<"\n----------"<<endl;
        cout<<"0) Exit"<<endl;
        cout<<"1) Insert data"<<endl;
        cout<<"2) Delete data"<<endl;
        cout<<"3) Update data"<<endl;
        cout<<"4) Search data"<<endl;
        cout<<"5) Display directory"<<endl;
        cout<<"Insert choice : ";
    }
    cin>>choice;
    return choice;
}



/* Directory class functions */

Directory::Directory(int depth, int bucket_size)
{
    this->global_depth = depth;
    this->bucket_size = bucket_size;
    for(int i = 0 ; i < 1<<depth ; i++ )
    {
        buckets.push_back(new Bucket(depth,bucket_size));
    }
}

int Directory::hash(int n)
{
    return n&((1<<global_depth)-1);
}

int Directory::pairIndex(int bucket_no, int depth)
{
    return bucket_no^(1<<(depth-1));
}

void Directory::grow(void)
{
    for(int i = 0 ; i < 1<<global_depth ; i++ )
        buckets.push_back(buckets[i]);
    global_depth++;
}

void Directory::shrink(void)
{
    int i,flag=1;
    for( i=0 ; i<buckets.size() ; i++ )
    {
        if(buckets[i]->getDepth()==global_depth)
        {
            flag=0;
            return;
        }
    }
    global_depth--;
    for(i = 0 ; i < 1<<global_depth ; i++ )
        buckets.pop_back();
}

void Directory::split(int bucket_no)
{
    int local_depth,pair_index;
    map<int, string> temp;
    map<int, string>::iterator it;

    local_depth = buckets[bucket_no]->increaseDepth();
    if(local_depth>global_depth)
        grow();
    pair_index = pairIndex(bucket_no,local_depth);
    buckets[pair_index] = new Bucket(local_depth,bucket_size);
    temp = buckets[bucket_no]->copy();
    buckets[bucket_no]->clear();
    for(it=temp.begin();it!=temp.end();it++)
        insert((*it).first,(*it).second,1);
}

void Directory::merge(int bucket_no)
{
    int local_depth,pair_index,index_diff,dir_size,i;

    local_depth = buckets[bucket_no]->getDepth();
    pair_index = pairIndex(bucket_no,local_depth);
    index_diff = 1<<local_depth;
    dir_size = 1<<global_depth;

    if( buckets[pair_index]->getDepth() == local_depth )
    {
        buckets[pair_index]->decreaseDepth();
        delete(buckets[bucket_no]);
        buckets[bucket_no] = buckets[pair_index];
        for( i=bucket_no-index_diff ; i>=0 ; i-=index_diff )
            buckets[i] = buckets[pair_index];
        for( i=bucket_no+index_diff ; i<dir_size ; i+=index_diff )
            buckets[i] = buckets[pair_index];
    }
}

string Directory::bucket_id(int n)
{
    int d;
    string s;
    d = buckets[n]->getDepth();
    s = "";
    while(n>0 && d>0)
    {
        s = (n%2==0?"0":"1")+s;
        n/=2;
        d--;
    }
    while(d>0)
    {
        s = "0"+s;
        d--;
    }
    return s;
}

void Directory::insert(int key,string value,bool reinserted)
{
    int bucket_no = hash(key);
    if(buckets[bucket_no]->insert(key,value))
    {
        if(!reinserted)
            cout<<"Inserted key "<<key<<" in bucket "<<bucket_id(bucket_no)<<endl;
        else
            cout<<"Moved key "<<key<<" to bucket "<<bucket_id(bucket_no)<<endl;
    }
    else
    {
        split(bucket_no);
        insert(key,value,reinserted);
    }
}

void Directory::remove(int key,int mode)
{
    int bucket_no = hash(key);
    buckets[bucket_no]->remove(key);
    cout<<"Deleted key "<<key<<" from bucket "<<bucket_id(bucket_no)<<endl;
    if(mode==1||mode==3)
    {
        if(buckets[bucket_no]->isEmpty() && buckets[bucket_no]->getDepth()>1)
            merge(bucket_no);
    }
    if(mode==2||mode==3)
    {
        shrink();
    }
}

void Directory::update(int key, string value)
{
    int bucket_no = hash(key);
    buckets[bucket_no]->update(key,value);
}

void Directory::search(int key)
{
    int bucket_no = hash(key);
    cout<<"Searching key "<<key<<" in bucket "<<bucket_id(bucket_no)<<endl;
    buckets[bucket_no]->search(key);
}

void Directory::display()
{
    int i,j,d;
    string s;
    std::set<string> shown;
    cout<<"Global depth : "<<global_depth<<endl;
    for(i=0;i<buckets.size();i++)
    {
        d = buckets[i]->getDepth();
        s = bucket_id(i);
        if(shown.find(s)==shown.end())
        {
            shown.insert(s);
            for(j=d;j<=global_depth;j++)
                cout<<" ";
            cout<<s<<" => ";
            buckets[i]->display();
        }
    }
}



/* Bucket class functions */

Bucket::Bucket(int depth, int size=BUCKET_SIZE)
{
    this->depth = depth;
    this->size = size;
}

int Bucket::insert(int key, string value)
{
    if(isFull())
        return 0;
    values[key] = value;
    return 1;
}

int Bucket::remove(int key)
{
    std::map<int,string>::iterator it;
    it = values.find(key);
    if(it!=values.end())
    {
        values.erase(it);
        return 1;
    }
    else
    {
        cout<<"Cannot remove : This key does not exists"<<endl;
        return 0;
    }
}

int Bucket::update(int key, string value)
{
    std::map<int,string>::iterator it;
    it = values.find(key);
    if(it!=values.end())
    {
        values[key] = value;
        cout<<"Value updated"<<endl;
        return 1;
    }
    else
    {
        cout<<"Cannot update : This key does not exists"<<endl;
        return 0;
    }
}

void Bucket::search(int key)
{
    std::map<int,string>::iterator it;
    it = values.find(key);
    if(it!=values.end())
    {
        cout<<"Value = "<<it->second<<endl;
    }
    else
    {
        cout<<"This key does not exists"<<endl;
    }
}

int Bucket::isFull(void)
{
    if(values.size()==size)
        return 1;
    else
        return 0;
}

int Bucket::isEmpty(void)
{
    if(values.size()==0)
        return 1;
    else
        return 0;
}

int Bucket::getDepth(void)
{
    return depth;
}

int Bucket::increaseDepth(void)
{
    depth++;
    return depth;
}

int Bucket::decreaseDepth(void)
{
    depth--;
    return depth;
}

std::map<int, string> Bucket::copy(void)
{
    std::map<int, string> temp(values.begin(),values.end());
    return temp;
}

void Bucket::clear(void)
{
    values.clear();
}

void Bucket::display()
{
    std::map<int,string>::iterator it;
    for(it=values.begin();it!=values.end();it++)
        cout<<it->first<<" ";
    cout<<endl;
}
