#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <fstream>

using namespace std;

const int MAX_KEYWORD = 64;
char* keyWords[4] = { "char", "int", "while", "enum" };

char keyWord[64] = "";
int keyIndex = 0;

int keyMatch(char ch, ofstream& outFile) {
		
		
		   if(ch == ' ') {
				keyWord[keyIndex++] = ch;
				keyWord[keyIndex] = '\0';
				outFile << keyWord;
				strcpy(keyWord, "");
				keyIndex = 0;
				return 0;
		}
		

		keyWord[keyIndex++] = ch;

		for(int i = 0; i < 3; ++i) {
				if((strcmp((const char*)keyWord, keyWords[i])) == 0) {
						outFile << "<font color=\"blue\">";
						outFile << keyWord;
						outFile << "<font>";
						keyIndex = 0;
						strcpy(keyWord,"");
						return 1;
				}
		}
}

// File states
enum Context { string_literal, char_literal, cpp_comment, c_comment, file_end };

// C-comment state
void handle_c_comment(ifstream& inFile) {
		char ch;
		while(inFile.get(ch)) {
				if(ch == '*') {
						while(inFile.get(ch) && ch == '*')
								;
						if(ch == '/')
								break;
				}
		}
}
// C++ comment state
void handle_cpp_comment(ifstream& inFile, ofstream& outFile) {
		char ch;
		outFile << "<font color=\"green\">";
		while(inFile.get(ch) && ch != '\n') 
				outFile.put(ch);
		outFile << "</font><br>\n";
}

// Literals (char, string)
void handle_literal(char delimiter, ifstream& inFile, ofstream& outFile) {
		outFile.put(delimiter);
		char ch;
		while(inFile.get(ch)) {
				// Test
				outFile.put(ch);
				if(ch == delimiter)
						break;
				else if(ch == '\\')
						inFile.get(ch) && outFile.put(ch);
		}
}


Context handle_code(ifstream& inFile, ofstream& outFile) {
		char ch;
		while(inFile.get(ch)) {
				switch(ch) {
						case '/':
								if(!inFile.get(ch)) {
										outFile.put('/');
										return file_end;
								}
								else
								{
										if(ch == '*')
												return c_comment;
										else if(ch =='/')
												return cpp_comment;
										else {
												outFile.put('/');
												inFile.putback(ch);
												break;
										}
								}

						case '\"' : return string_literal;
						case '\'' : return char_literal;
						default: 
									{
											keyMatch(ch, outFile);
											//outFile.put(ch);
									}
				}
		}
return file_end;
}
									

int main(int argc, char** argv) {
		Context context;

		if(argc != 2) {
				cerr << "Usage: parse <infile>" << endl;
				exit(1);
		}
		
		string* output = new string(argv[1]);
		
		ifstream inFile(argv[1]);
		ofstream outFile("test.dat");
		
		while((context = handle_code(inFile, outFile)) != file_end)
				switch(context) {
						case c_comment:
								handle_c_comment(inFile);
								break;
						case cpp_comment:
								handle_cpp_comment(inFile, outFile);
								break;
						case string_literal:
								handle_literal('\"', inFile, outFile);
								break;
						case char_literal:
								handle_literal('\'', inFile, outFile);
								break;
				}
		return 0;
}

