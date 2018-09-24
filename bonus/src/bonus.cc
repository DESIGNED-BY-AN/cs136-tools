#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

static int min(int a, int b){
  if (a <= b) return a;
  return b;
}

int main(int argc, char *argv[]){

	if (argc == 1){
		cerr << "" << endl;
		cerr << "+---------------------------------------------------------------------------------------------------------------------+" << endl;
		cerr << "+ Usage         : ./bonus [marm_id] [accept_ids] [all_ids] [bonus_grade]" << endl;	
		cerr << "+---------------------------------------------------------------------------------------------------------------------+" << endl;
		cerr << "+ Purpose       : generates a course-compliant bonus grade .csv file" << endl;	
		cerr << "+---------------------------------------------------------------------------------------------------------------------+" << endl;
		cerr << "+ [marm_id]     : the name of the assignment question directory on Marmoset IN CAPS! e.g. A6Q5" << endl;
		cerr << "+ [accept_ids]  : the name of the file containg the Quest IDs of the students who passed the bonus (ONE ID per line)" << endl;
		cerr << "+ [all_ids]     : the name of the file containing the Quest IDs of ALL students currently enrolled (ONE ID per line)" << endl;
		cerr << "+ [bonus_grade] : an integer representing the amount of points (all-or-nothing) that the bonus is worth (e.g. 2)" << endl;	
		cerr << "+---------------------------------------------------------------------------------------------------------------------+" << endl;
		cerr << "+ Author        : An Le (Spring 2018)" << endl;
		cerr << "+---------------------------------------------------------------------------------------------------------------------+" << endl;
		cerr << "" << endl;
		return 0;
	}

	const string MARM_ID = argv[1];
	const string ACC_IDS = argv[2];
	const string ALL_IDS = argv[3];
	const int BONUS_GRADE = stoi(argv[4]);
	
	const string OUT_ID = MARM_ID + "Bonus.raw.csv";
	const int ZERO = 0;

	ifstream f_i;
	ofstream f_o;
	ifstream f;
	string line;
	string id;

	f_i.open(ALL_IDS);
	f_o.open(OUT_ID);

	if (f_i.is_open() && f_o.is_open()){
		while (getline(f_i,line)){
			line = line.substr(0,min(line.length(),8));
			bool accept = false;
			f.open(ACC_IDS);
			if (f.is_open()){
				while (getline(f,id)){
					if (line == id) accept = true;	
				}
				f.close();
			}else{
				cerr << "Unable to open " << ACC_IDS << endl;
			}
			if (accept){
				f_o << line << "," << BONUS_GRADE << ",On-time" << endl;
			}else{
				f_o << line << "," << ZERO << ",On-time" << endl;
			}
		}
		f_i.close();
		f_o.close();
	}else{
		cerr << "Unable to open " << ALL_IDS << " and/or create file " << OUT_ID << endl;
	}
}
