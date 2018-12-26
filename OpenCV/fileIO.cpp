#include "stdafx.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <regex>
//#include <boost/algorithm/string.hpp>

using namespace std;
//using namespace boost;

void readFileToVector(vector<vector<float>> &result);

//void main()
//{
//	vector<vector<float>> result;
//	readFileToVector(result);
//	for(int i = 0; i < result.size(); i++)
//	{
//		for(int j = 0; j < 4; j++)
//		{
//			cout << result[i][j] << " ";
//		}
//		cout << endl;
//	}
//	
//}

void readFileToVector(vector<vector<float>> &result)
{
	ifstream file("E:\\JavaProgram\\autoGame\\keyFile\\targetDir.txt");
	char buffer[100];
	while (!file.eof())
	{
		file.getline(buffer, 100);
		string isEmpty = buffer;
		/*string str = buffer;
		vector<string> result;
		split(result, str, is_any_of(" "), token_compress_on);
		for(vector<string>::iterator it=result.begin();it!=result.end();it++){  
        if(*it!="")  
            cout<<*it<<" ";  
		}  
		cout << endl;*/
		char *p;
		p = strtok(buffer, " ");
		int count = 0;
		vector<float> direction(4, -1);
		while( p != NULL )
		{
			if(count == 1 || count == 2)
			{
				string str = p;
				if(str == "UP")
				{
					direction[0] = 1;
				}
				else if(str == "RIGHT")
				{
					direction[1] = 1;
				}
				else if(str == "DOWN")
				{
					direction[2] = 1;
				}
				else if(str == "LEFT")
				{
					direction[3] = 1;
				}
			}

			p = strtok( NULL, " " );
			count++;
		}
		if(isEmpty != "")
		{
			result.push_back(direction);
		}
		
	}
}