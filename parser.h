#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <fstream>


using namespace std;

const int MAX_DELIMS = 14;
const int MAX_KEYWORD = 64;
const char delims[MAX_DELIMS] = { '<', '>', ';', '\n', '*', '&', ':', '(', ')',',', ' ','\t','[',']' };

class Parser { 
		public:
				Parser();
				Parser(char* inFile) { parse(inFile); }
				virtual ~Parser() {}
				int parse(char* fileName);
		protected:
				enum Context { string_literal, char_literal, single_line_comment, multiline_comment, file_end, newline, keyword };

				char* keyWords[MAX_KEYWORD]; 
				char keyWord[MAX_KEYWORD];
				int keyIndex;
				int KEYWORDS;

				int outputChar(char ch, ofstream& outFile);
				bool isDelim(char ch);
				void printHeader(char* fileName, ofstream& outFile);
				void printFooter(ofstream& outFile);
				Context handle_code(ifstream& inFile, ofstream& outFile);
				void handle_literal(char delimiter, ifstream& inFile, ofstream& outFile);
				void handle_single_comment(ifstream& inFile, ofstream& outFile);
				int keyMatch(char ch, ofstream& outFile); 
				void handle_multiline_comment(ifstream& inFile, ofstream& outFile); 
				void _parse(ifstream& inFile, ofstream& outFile);

};







