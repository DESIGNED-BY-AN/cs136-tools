#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <vector>

using namespace std;

static int min(int x, int y){
  if (x <= y) return x;
  return y;
}

static int find_first_of(string line, char c, int n){
  const int L = line.length();
  int retval = -1;
  if (n <= 0) return retval;

  for (int i = 0; i < L; ++i){
    if (n == 0){
      return retval;
    }
    if (line[i] == c){
      retval = i;
      --n;
    }
  }
  if (n) retval = -1;
  return retval;
}

static int find_last_of(string line, char c, int n){
  int total = (int) count(line.begin(),line.end(),c);
  return find_first_of(line,c,total-n+1);
}

static bool is_whitespace(char c){
  return (c == 32);
}

static string remove_adjacent_ws(string line){
  bool on_space = false;
  string retval = "";

  for (char c : line){
    if (!is_whitespace(c)){
      retval += c;
      on_space = false;
    }
    else{
      if (!on_space){
	retval += ' ';
	on_space = true;
      }
    }
  }
  return retval;
}

static string remove_trailing_ws(string line){
  int last = 0;
  int length = line.length();
  for (int i = 0; i < length; ++i){
    if (!is_whitespace(line[i])) last = i; 
  }
  return line.substr(0, min(last + 1, length));
}

static string remove_leading_ws(string line){
  int i = 0;
  for (char c : line){
    if (!is_whitespace(c)) return line.substr(i);
    ++i;
  }
  return line; 
}

static string remove_extra_ws(string line){
  string retval = line;
  retval = remove_leading_ws(retval);
  retval = remove_trailing_ws(retval);
  retval = remove_adjacent_ws(retval);
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
  return line.substr(0,k);
}

static int find_main(string line){
  line = remove_extra_ws(line);
  line = ignore_comment(line); 

  const string begin = "int main";
  int k = find_substr(line,begin);
  return k;
}

static void shift_spaces(vector<long> &indexes, int threshold, int amn){
  int size = indexes.size();
  for (int i = 0; i < size; ++i){
    if (indexes[i] >= threshold){
      indexes[i] += amn;
    }
  }
}

int main(int argc, char *argv[]){
  
  if (argc != 3){
    cerr << "" << endl;
    cerr << "+--------------------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "+ Usage	         : ./xchrono [main_file] [exec_file]" << endl;
    cerr << "+--------------------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "+ Purpose        : generates a C source file called exec_file that runs the main function in main_file and prints to stderr" << endl;
    cerr << "                   \"marmoset_time:\" followed by the execution time (in time_t units) and then a newline" << endl;
    cerr << "+--------------------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "+ [main_file]    : the name of the .c file containing the main function" << endl;
    cerr << "+ [exec_file]    : the name of the .c file that will be produced by this script" << endl;
    cerr << "+--------------------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "+ Author         : An Le (Spring 2018)" << endl;
    cerr << "+--------------------------------------------------------------------------------------------------------------------------------+" << endl;
    cerr << "" << endl;
    exit(1);
  }
  
  ifstream f_i;
  ofstream f_o;
  string line;

  const string SRC	      = argv[1];
  const string E_FILE	      = argv[2];

  const string space	      = " ";
  const string measure_end    = "\n"
				"  clock_t end = clock();\n"
				"  fprintf(stderr, \"marmoset_time:%ld\\n\", end - begin);\n";

  const string measure_start  =	"  \nclock_t begin = clock();\n";
   
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
  
  const string includes	      = "#include <time.h>\n"
				"#include <stdlib.h>\n"
				"#include <stdio.h>";

  f_o << includes << endl;

  string Mike = "";

  vector<long> newline_indexes;
  
  int max_length = 1000;
  long length_so_far = 0;

  while(getline(f_i,line)){
    line = ignore_comment(line);
    line = remove_extra_ws(line);
    int line_length = line.length();
    length_so_far += line_length;
    if (line_length > 0) newline_indexes.push_back(length_so_far);
    Mike += line;
    Mike += space; //every newline is now replaced by a space, very very important
    ++length_so_far;
  }

  const string NAME = "main";

  //proof that these two options together are adequate is left to the reader ;)

  string Shinoda = "int " + NAME + "(";
  string LP = "int " + NAME + " (";
  
  int start_one = find_substr(Mike,Shinoda);
  int start_two = find_substr(Mike, LP);

  int start;
  int offset;

  if (start_one > 0){
    start = start_one;
    offset = Shinoda.length();    
  }
  else if (start_two > 0){ 
    start = start_two;
    offset = LP.length();
  }
  else{
    cerr << "ERROR: main function not found!" << endl;
    cerr << endl;
    cerr << Mike << endl;
    exit(5);
  }

  int length = Mike.length();
  int index = start + offset;
  int paren_count = 1;

  while(true){
    if (index >= length || paren_count < 0){
      cerr << "ERROR 1: the main function's definition has a syntax error!" << endl;
      exit(2);
    }
    if (Mike[index] == ')') --paren_count;
    if (Mike[index] == '(') ++paren_count;
    if (!paren_count){
      break;
    }
    ++index;
  }

  while(true){
    if (Mike[index] == '{'){
      break;
    }
    ++index;
  }

  ++index;
  string first = Mike.substr(0, index);
  string rest = Mike.substr(index);

  string setup = measure_start;
  first += setup;

  first += rest;
  Mike = first;

  shift_spaces(newline_indexes, index, setup.length());

  int bracket_count = 1;
  length = Mike.length();

  //skips the one curly brace that comes with the inserted for-loop
  bool pardon = false; 

  while(1){
    if (index >= length || bracket_count < 0){
      cerr << "ERROR 2: the main function's definition has a syntax error!" << endl;
      exit(6);
    }
    if (Mike[index] == '{'){
      if (!pardon) ++bracket_count;
      else pardon = false;
    }
    if (Mike[index] == '}') --bracket_count;
    if (!bracket_count){
      break;
    }
    ++index;
  }

  first = Mike.substr(0, index);
  rest = Mike.substr(index);

  first += measure_end;
  first += rest;

  Mike = first;

  shift_spaces(newline_indexes, index, measure_end.length());

  length = Mike.length();

  int spaces = newline_indexes.size();

  int cur = 0;
  for (int i = 0; i < length; ++i){
    if (i == newline_indexes[cur]){
      ++cur;
      f_o << endl;
    }else{
      f_o << Mike[i];
    }
  }

  f_i.close();
  f_o.close();
}
