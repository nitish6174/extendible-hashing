#include <bits/stdc++.h>
using namespace std;


#define BUCKET_SIZE 1
#define INITIAL_GLOBAL_DEPTH 1


int menu();

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
    public:
        Directory(int depth, int bucket_size);
        void insert(int key,string value);
        void remove(int key,int mode);
        void update(int key, string value);
        void search(int key);
        void display(void);
};



int main()
{
    Directory d(INITIAL_GLOBAL_DEPTH,BUCKET_SIZE);
    int choice, key, mode;
    string value;

    do
    {
        choice = menu();
        switch(choice)
        {
            case 1:
                cout<<"Key : ";
                cin>>key;
                cout<<"Value : ";
                cin>>value;
                d.insert(key,value);
                break;
            case 2:
                cout<<"Key : ";
                cin>>key;
                cout<<"Mode ( 0-Lazy / 1-Merge empty / 2-Shrink directory / 3-Both merge and shrink ) : ";
                cin>>mode;
                d.remove(key,mode);
                break;
            case 3:
                cout<<"Key : ";
                cin>>key;
                cout<<"Value : ";
                cin>>value;
                d.update(key,value);
                break;
            case 4:
                cout<<"Key : ";
                cin>>key;
                d.search(key);
                break;
            case 5:
                d.display();
                break;
        }
    } while(choice!=0);

    return 0;
}



/* Display choices in main */

int menu()
{
    int choice;
    cout<<"-----"<<endl;
    cout<<"0) Exit"<<endl;
    cout<<"1) Insert data"<<endl;
    cout<<"2) Delete data"<<endl;
    cout<<"3) Update data"<<endl;
    cout<<"4) Search data"<<endl;
    cout<<"5) Display directory"<<endl;
    cout<<"Insert choice : ";
    cin>>choice;
    cout<<endl;
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
        insert((*it).first,(*it).second);
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

void Directory::insert(int key,string value)
{
    int bucket_no = hash(key);
    if(!(buckets[bucket_no]->insert(key,value)))
    {
        split(bucket_no);
        insert(key,value);
    }
}

void Directory::remove(int key,int mode)
{
    int bucket_no = hash(key);
    buckets[bucket_no]->remove(key);
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
    buckets[bucket_no]->search(key);
}

void Directory::display()
{
    cout<<"Displaying directory (global depth="<<global_depth<<"):"<<endl;
    for(int i=0;i<buckets.size();i++)
    {
        cout<<"  Bucket "<<i<<" (depth="<<buckets[i]->getDepth()<<") : ";
        buckets[i]->display();
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
        cout<<it->second<<endl;
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
