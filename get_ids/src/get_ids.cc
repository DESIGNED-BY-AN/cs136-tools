#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace std;

int main(int argc, char *argv[]){

	if (argc != 2){	
	  cerr << "" << endl;
	  cerr << "+----------------------------------------------------------------------------------------------------------------+" << endl;
	  cerr << "+ Usage            : ./get_ids [classlist_file] [output_file]" << endl;	
	  cerr << "+----------------------------------------------------------------------------------------------------------------+" << endl;
	  cerr << "+ Purpose          : to obtain clean, properly truncated Quest/Marmoset IDs from a raw classlist" << endl;		 
	  cerr << "+----------------------------------------------------------------------------------------------------------------+" << endl; 
	  cerr << "+ [classlist_file] : the name of a file containing a valid classlist" << endl; 
	  cerr << "+ [output_file]    : the name of the output file, each line containing one valid Quest ID and nothing else" << endl;
	  cerr << "+----------------------------------------------------------------------------------------------------------------+" << endl;
	  cerr << "+ Author           : An Le (Spring 2018)" << endl;
	  cerr << "+----------------------------------------------------------------------------------------------------------------+" << endl;
	  cerr << "" << endl;
	  return 0;
	  exit(1);
	}
	ifstream f_i;
	ofstream f_o;
	const string RAW_LIST = argv[1];
	const string NEW_LIST = argv[2];

	f_i.open(RAW_LIST);
	f_o.open(NEW_LIST);

	string line;

	if (f_i.is_open() && f_o.is_open()){
		while (getline(f_i,line)){
			string id = "";
			bool start = false;
			for (char c : line){
				if (c == ':'){
					if (start) break;
					else start = true;
				}else{
					if (start) id += c;
				}
			}
			f_o << id << endl;
		}
	}else{
		cerr << "Unable to open classlist and/or unable to make output file" << endl;
	}
}
