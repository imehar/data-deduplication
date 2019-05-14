#include <string>
#include <iostream>
#include<fstream>
#include<sstream>
#include<bits/stdc++.h>
#include<sys/stat.h>
#include <experimental/filesystem>
#include "./sha256/sha256.h"
#include "./sha256/sha.h"
#include <map>
#include <sstream>
#include <fstream>
#include  <iostream>
#include <bits/stdc++.h>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


namespace fs = std::experimental;

using namespace std;



void save(std::map<string, string> hashtable ,string hash_path){
    ofstream ofs {hash_path};
    boost::archive::text_oarchive oarch(ofs);
    oarch << hashtable;
}


map<string, string>  load(string hash_path){
    ifstream ifs {hash_path};
    boost::archive::text_iarchive iarch(ifs);
    std::map<string, string> hashtable1;
    iarch >> hashtable1;
    // cout<< hashtable1.size()<<endl;
    // for (auto i = hashtable1.begin(); i != hashtable1.end(); ++i)
    // {
    //     cout<<i->first<<" ";
    //     cout<<i->second<<endl;
    // }
    return hashtable1;
}
map<string, string> hashtable;
void listfiles(string path,string hash_path){
	struct stat s;
	error_code ec;

    for (const auto & entry : fs::filesystem::directory_iterator(path))
    {
    	const char* p = entry.path().c_str();
    	if (stat(p, &s) == 0)
    	{
    		// if (s.st_mode & S_IFDIR)
    		// {
    		// 	listfiles(entry.path(),hashtable);
    		// }
    		if (s.st_mode & S_IFREG)
    		{

    			// std::cout << entry.path() << std::endl;
    			ifstream inFile;
    			inFile.open(entry.path());
    			stringstream strStream;
    			strStream << inFile.rdbuf();
    			string outString = strStream.str();
    			// cout<< outString<<endl;
    			string outputsha = sha256(outString);
    			// cout << outputsha<<endl;
    			hashtable[outputsha] = entry.path();
    			// return;
    		}
    	}
    }
 	save(hashtable, hash_path);
}


