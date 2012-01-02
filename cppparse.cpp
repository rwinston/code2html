#include "CppParser.h"

CppParser::CppParser() {
		ifstream keyFile("keywords/CPP.kwd");
		char keyword[MAX_KEYWORD];
		int i = 0;
		
		cout << keyWords << endl;

		while(keyFile >> keyword) {
				keyWords[i] = new char[MAX_KEYWORD];
				strcpy((char*)keyWords[i], (const char*)keyword);
				i++; 
				KEYWORDS++;
		}
}

void CppParser::handle_multiline_comment(ifstream& inFile, ofstream& outFile) {

   	char ch;
   	
   	outFile << "<font color=\"green\">";
   	outFile << "/*";
   	while(inFile.get(ch)) {
   			outFile << ch;			// normal char
   			if(ch == '*') {
   					while(inFile.get(ch) && ch == '*')
   							outFile << ch;
   					if(ch == '/') {
   							outFile << ch;
   							outFile << "</font>";
   							break;
   					}
   			}
   	}

}

void CppParser::handle_single_comment(ifstream& inFile, ofstream& outFile) {
		char ch;
		outFile << "<font color=\"green\">";
		outFile << "//";
		while(inFile.get(ch) && ch != '\n') 
		outFile.put(ch);
		outFile << "</font><br>\n";		
}


int main() {
		CppParser* cpp = new CppParser();
		cpp->parse("CppParser.cpp");
		delete cpp;
}
