#include <bits/stdc++.h>
#include<string.h>
#include<iostream>
#include<fstream>

#define lld long long int

using namespace std;

class Rolling
{
public:
	lld k;
	lld base;
	lld value;
	string str;
	lld start_pointer;
	lld end_pointer;
	void compute();
	string window_string();
	Rolling(string text1,lld window_size)
	{
		k = window_size-1;
		base = 26;
		value = 0;
		str = text1;
		for (lld i = 0; i < window_size; ++i)
		{
			value += (int(str[i]) - int('a') + 1 )* (pow(base,(window_size - i - 1)));
		}
		start_pointer = 0;
		end_pointer = window_size;
	}
	
};


void Rolling::compute(){
	if (end_pointer <= str.length() -1)
	{
		value -= (int(str[start_pointer]) - int('a') + 1)*pow(26,(k));
		value *= base;
		value += int(str[end_pointer]) - int('a') +1;
		start_pointer+=1;
		end_pointer+=1;
	}
}

string Rolling::window_string(){
	return str.substr(start_pointer,k+1);
}



pair<lld,lld> rabinkarp(string text,string target_string,lld window){
	if (text.length() <=0 || target_string.length() <=0 || text.length() < window || target_string.length() < window){
		cout<<"invalid";
		pair< lld, lld > index_pair (-1,-1);
		return index_pair;
	}
	lld target_len = target_string.length();
	lld string_len = text.length();

	Rolling target_hash = Rolling(target_string,window);
	// Rolling rolling_hash = Rolling(text,window);

	// if (target_hash.value == rolling_hash.value)
	// {
	// 	return 0;
	// }
	for (lld j = 0; j < (target_len - window + 1); ++j)
	{
		Rolling rolling_hash = Rolling(text,window);
		for (lld i = 0; i < (string_len - window + 1); ++i)
		{
			if (target_hash.value == rolling_hash.value)
			{
				if (target_hash.window_string().compare(rolling_hash.window_string()) == 0)
				{
					cout<<"pat "<<target_hash.window_string()<<" start pointer "<<target_hash.start_pointer<<" end pointer "<<target_hash.end_pointer<<endl;
					cout<<"txt "<<rolling_hash.window_string()<<" start pointer "<<rolling_hash.start_pointer<<" end pointer "<<rolling_hash.end_pointer<<endl;
					cout<<" i  "<<i<<" j "<<j<<endl;
					pair< lld, lld > index_pair (i,j);
					return index_pair;
				}
			}
			rolling_hash.compute();
		}

		target_hash.compute();
	}
	pair< lld, lld > index_pair (-1,-1);
	return index_pair;
}

