#include <bits/stdc++.h>

using namespace std;

class Block {
public:
	unsigned int tag;
	bool dirty;
	int lru;
	bool valid;
	Block(){
		tag = 0;
		lru = 0;
		dirty = false;
		valid = false;
	}
};

class CacheSet {
public:
	vector<Block> v1;
	CacheSet(int assoc){
		for (int i = 0; i < assoc; ++i)
		{
			Block b;
			v1.push_back(b);
		}
	}
};

class Cache {
public:
	vector<CacheSet> v2;
	int cacheHit,cacheMiss,WriteBack;
	int size,block_size;
	Cache(int cachesize,int blocksize,int assoc){
		block_size = blocksize;
		size = ((cachesize*1024)/blocksize)/assoc;
		//cout << "size = " << size << endl;
		for (int i = 0; i < size; ++i)
		{
			CacheSet c(assoc);
			v2.push_back(c);
		}
		cacheMiss = 0;
		cacheHit = 0;
		WriteBack = 0;
	}
	void read(unsigned int addr);
	void write(unsigned int addr);
};

void Cache::write(unsigned int addr){
	int offset = log2(block_size);
	addr = addr >> offset;
	int index = addr%size;
	int shift = log2(size);
	unsigned int this_tag = addr >> shift;
	//cout << this_tag << endl;
	//cout << offset << " " << shift << endl;
	bool ishit = false;
	for (int i = 0; i < v2[index].v1.size(); ++i)
	{
		if((this_tag == v2[index].v1[i].tag)&&v2[index].v1[i].valid){
			cacheHit++;
			ishit = true;
			v2[index].v1[i].lru = 1;
			v2[index].v1[i].dirty = true;
			v2[index].v1[i].valid = true;
			break;
		}
		else if((this_tag == v2[index].v1[i].tag)&&!v2[index].v1[i].valid){
			cacheMiss++;
			ishit = true;
			v2[index].v1[i].lru = 1;
			v2[index].v1[i].dirty = true;
			v2[index].v1[i].valid = true;
			break;
		}
	}
	if(!ishit){
		cacheMiss++;
		int min=v2[index].v1[0].lru,min_index=0;
		for (int i = 0; i < v2[index].v1.size(); ++i)
		{
			if(min>v2[index].v1[i].lru){
				min = v2[index].v1[i].lru;
				min_index = i;
			}
		}
		if(!v2[index].v1[min_index].valid){
			v2[index].v1[min_index].lru = 1;
			v2[index].v1[min_index].valid = true;
			v2[index].v1[min_index].dirty = true;
			v2[index].v1[min_index].tag = this_tag;
			//cout << index << endl;
		}
		else{
			if(v2[index].v1[min_index].dirty)
				WriteBack++;
			v2[index].v1[min_index].lru = 1;
			v2[index].v1[min_index].valid = true;
			v2[index].v1[min_index].dirty = true;
			v2[index].v1[min_index].tag = this_tag;
		}
	}
}

void Cache::read(unsigned int addr){
	int offset = log2(block_size);
	addr = addr >> offset;
	int index = addr%size;
	int shift = log2(size);
	unsigned int this_tag = addr >> shift;
	bool ishit = false;
	for (int i = 0; i < v2[index].v1.size(); ++i)
	{
		if((this_tag == v2[index].v1[i].tag)&&v2[index].v1[i].valid){
			cacheHit++;
			ishit = true;
			v2[index].v1[i].lru++;
			v2[index].v1[i].valid = true;
			break;
		}
		else if((this_tag == v2[index].v1[i].tag)&&!v2[index].v1[i].valid){
			cacheMiss++;
			ishit = true;
			v2[index].v1[i].lru = 1;
			v2[index].v1[i].dirty = false;
			v2[index].v1[i].valid = true;
			break;
		}
	}
	if(!ishit){
		cacheMiss++;
		int min=v2[index].v1[0].lru,min_index=0;
		//cout << v2[index].v1.size() << endl;
		for (int i = 0; i < v2[index].v1.size(); ++i)
		{
			if(min>v2[index].v1[i].lru){
				min = v2[index].v1[i].lru;
				min_index = i;
			}
		}
		if(!v2[index].v1[min_index].valid){
			v2[index].v1[min_index].lru = 1;
			v2[index].v1[min_index].valid = true;
			v2[index].v1[min_index].dirty = false;
			v2[index].v1[min_index].tag = this_tag;
		}
		else{
			if(v2[index].v1[min_index].dirty){
				WriteBack++;
			}
			v2[index].v1[min_index].lru = 1;
			v2[index].v1[min_index].valid = true;
			v2[index].v1[min_index].dirty = false;
			v2[index].v1[min_index].tag = this_tag;
		}
	}
}

int main(){
	Cache c1(16,32,1);
	Cache c2(16,32,2);
	Cache c3(16,32,4);
	ifstream myFile;
	myFile.open("i.txt");
	int count = 0;
	string line;
	while (getline(myFile,line)){
		count++;	
	}
	myFile.clear();
    myFile.seekg(0);
    char a;
    unsigned int b;
    for (int i = 0; i < count; ++i)
    {
    	myFile >> a; 
    	myFile >> hex >> b;
    	if(a=='W'||a=='w'){
    		c1.write(b);
    		c2.write(b);
    		c3.write(b);
    	}
    	else if(a=='R'||a=='r'){
    		c1.read(b);
    		c2.read(b);
    		c3.read(b);
    	}
    	else{
    		cout << "Its neither W nor R" << endl;
    	}
    }
    cout << "For direct-map,\nTotal misses = " << c1.cacheMiss << "\nTotal hits = " << c1.cacheHit << endl;
    cout << "\nFor two-way assosciative,\nTotal misses = " << c2.cacheMiss << "\nTotal hits = " << c2.cacheHit << endl;
    cout << "\nFor four-way assosciative,\nTotal misses = " << c3.cacheMiss << "\nTotal hits = " << c3.cacheHit << endl;
}