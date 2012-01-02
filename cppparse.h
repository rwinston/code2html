#include "Parser.h"

class CppParser : public Parser {
		public:
				CppParser();
				~CppParser()  { delete [] keyWords; }
				 void handle_single_comment(ifstream& inFile, ofstream& outFile);
				 void handle_multiline_comment(ifstream& inFile, ofstream& outFile);
};
