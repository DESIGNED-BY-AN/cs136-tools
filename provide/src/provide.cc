#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <queue>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <climits>

using namespace std;

static bool is_whitespace(char c){
  return (c <= 32) && (c != 0) && (c != 10);
}

//wow, cs136 assignment question much shock
static string remove_extra_ws(string line){
  string retval = "";
  
  bool on_space = false;
  bool empty = true;

  for (char c : line){
    if (!is_whitespace(c)){
      empty = false;
      retval += c;
      on_space = false;
    }
    else{
      if (empty || !on_space) retval += ' ';
      on_space = true;
    }
  }
  return retval;
}

static void compute_lsp_table(string pattern, int *lsp){
  lsp[0] = 0;
  for (int i = 1; i < pattern.length(); ++i){
    int j = lsp[i - 1];
    while (j > 0 && pattern[i] != pattern[j]){
      j = lsp[j - 1];
    }
    if (pattern[i] == pattern[j]){
      ++j;
    }
    lsp[i] = j;
  }
}

static int find_substr(string text, string pattern){
  int *lsp = new int[pattern.length()];
  compute_lsp_table(pattern, lsp);
  
  int j = 0;
  for (int i = 0; i < text.length(); ++i){
    while (j > 0 && text[i] != pattern[j]){
      j = lsp[j - 1];
    }
    if (text[i] == pattern[j]){
      ++j;
      if (j == pattern.length()){
	delete[] lsp;
	return i - (j - 1);
      }
    }
  }
  delete[] lsp;
  return -1;
}

static string ignore_comment(string line){
  int k = find_substr(line,"//");
  if (k < 0) return line;
  return line.substr(0,k);
}

int main(int argc, char *argv[]){
  
  if (argc != 4){
    cerr << "" << endl;
    cerr << "+-------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "+ Usage	             : ./provide [submission_file] [correct_file] [struct_name]" << endl;
    cerr << "+-------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "+ Purpose            : to enfore that the student's submission uses the correct definition of the struct struct_name" << endl; 
    cerr << "+-------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "+ [submission_file]  : student's submission file" << endl;
    cerr << "+ [correct_file]     : a header (.h) file containing ONLY the correct definition for struct struct_name" << endl;
    cerr << "+ [struct_name]      : name of the struct to provide" << endl;
    cerr << "+-------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "+ Author             : An Le (Spring 2018)" << endl;
    cerr << "+-------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "" << endl;
    exit(1);
  }
  ifstream f_i;
  ofstream f_o;
  ifstream f_s;
  string line;

  const string SRC	      = argv[1];
  const string SUB	      = argv[2];
  const string NAME	      = argv[3];
  const string E_FILE	      = "nyanners.c"; 
  
  f_o.open(E_FILE);
  
  if (!f_o.is_open()){
    cerr << "Cannot create file " << E_FILE << endl;
    exit(2);
  }

  f_i.open(SRC);

  if (!f_i.is_open()){
    cerr << "Cannot open file " << SRC << endl;
    exit(3);
  }

  f_s.open(SUB);

  if (!f_s.is_open()){
    cerr << "Cannot open file " << SUB << endl;
    exit(4);
  } 

  string Mike = "";

  queue<long> newline_indexes;

  int max_length = 1000;
  long length_so_far = 0;

  while(getline(f_i,line)){
    line = ignore_comment(line);
    line = remove_extra_ws(line);
    int line_length = line.length();
    length_so_far += line_length;
    if (line_length > 0) newline_indexes.push(length_so_far);
    Mike += line;
  }

  string Shinoda = "struct " + NAME + "{";
  string LP = "struct " + NAME + " {";
  
  int start_one = find_substr(Mike,Shinoda);
  int start_two = find_substr(Mike, LP);

  int length_one = Shinoda.length();
  int length_two = LP.length();

  int bracket_count = 1;

  int offset;
  int start;

  if (start_one > 0){
    start = start_one;
    offset = Shinoda.length();
  }
  else if (start_two > 0){ 
    start = start_two;
    offset = LP.length();
  }
  else{
    cerr << "ERROR: struct " << NAME << " not found!" << endl;
    exit(5);
  }

  int index = start + offset;
  int length = Mike.length();

  while(1){
    if (index >= length){
      cerr << "ERROR: struct " << NAME << "'s definition has a syntax error!" << endl;
      exit(6);
    }
    if (Mike[index] == '{') ++bracket_count;
    if (Mike[index] == '}') --bracket_count;
    if (!bracket_count){
      while(1){
	++index;
	if (index >= length){
	  cerr << "ERROR: struct " << NAME << "'s definition has a syntax error!" << endl;
	  exit(7);
	}
	if (Mike[index] == ';') break;
      }
      break;
    }
    ++index;
  }

  for (int i = 0; i < length; ++i){
    if (!newline_indexes.empty() && i == newline_indexes.front()){
      newline_indexes.pop();
      if (!(i >= start && i <= index)) f_o << endl;
      --i;
    }else{
      if (!(i >= start && i <= index)){
	f_o << Mike[i];
	if (Mike[i] == '}') f_o << endl;
      }else if (i == start){
	f_o << "\n" << endl;
	while (getline(f_s,line)){
	  f_o << line << endl;
	}
      }
    }
  }

  f_i.close();
  f_o.close();
  f_s.close();
  remove(SRC.c_str());
  rename(E_FILE.c_str(),SRC.c_str());
}
