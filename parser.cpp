#include "Parser.h"

int             Parser::
outputChar(char ch, ofstream & outFile)
{

	if (ch == '>') {
		outFile << "&gt;";
		return 0;
	} else if (ch == '<') {
		outFile << "&lt;";
		return 0;
	} 
		else if (ch == '\t') {
		
			outFile << "&nbsp;&nbsp;&nbsp;&nbsp;";
		
			return 0;
		
	} else if (ch == '\n') {
		outFile << "<br>";
		outFile.put(ch);
		return 0;
	} else if (ch == '&') {
		outFile << "&amp;";
		return 0;
	} else {
		outFile.put(ch);
		return 0;
	}

}

bool            Parser::
isDelim(char ch)
{
	for (int i = 0; i < MAX_DELIMS; ++i)
		if (ch == delims[i])
			return true;

	return false;
}


int             Parser::
keyMatch(char ch, ofstream & outFile)
{

	//no existing keyword, and character cannot be part of a keyword,
	//output it
	if (strcmp(keyWord, "") == 0) {
		if (isDelim(ch)) {
			outputChar(ch, outFile);
			return 0;
		}
	}
	//delimiter marks the end of a possible existing keyword
		if (isDelim(ch)) {

		//Search for a
			match
				for (int i = 0; i < KEYWORDS; ++i) {
				if ((strcmp((const char *) keyWord, keyWords[i])) == 0) {
					cout << " match! ";
					outFile << "<font color=\"blue\">";
					outFile << keyWord;
					outFile << "</font>";
					outputChar(ch, outFile);
					keyIndex = 0;
					strncpy(keyWord, "\0", 64);
					return 1;
				}
			}

		//No match, keyword could be a var name, etc.
			outFile << keyWord;
		outFile.flush();
		outputChar(ch, outFile);
		strncpy(keyWord, "\0", 64);
		//clear string for re
			-use
				keyIndex = 0;
		return 0;

	} else {
		//tack the character onto the keyword
			keyWord[keyIndex++] = ch;

	}
}

void            Parser::
handle_multiline_comment(ifstream & inFile, ofstream & outFile)
{

}

void            Parser::
handle_single_comment(ifstream & inFile, ofstream & outFile)
{
}

//Literals(char, string)
//Make sure that <, >, etc are escaped inside literals !
	void            Parser::handle_literal(char delimiter, ifstream & inFile, ofstream & outFile)
{
	outFile.put(delimiter);

	char            ch;

	while (inFile.get(ch)) {
		if (ch == delimiter) {
			outFile.put(ch);
			break;
		} else if (ch == '\\') {
			inFile.get(ch) && outFile.put(ch);
		} else
			outputChar(ch, outFile);
	}
}


Parser: : Context Parser: :handle_code(ifstream & inFile, ofstream & outFile)
{
	char            ch;
	while (inFile.get(ch)) {
		switch (ch) {
		case '/':
			if (!inFile.get(ch)) {
				outFile.put('/');
				return file_end;
			} else {
				if (ch == '*')
					return multiline_comment;
				else if (ch == '/')
					return single_line_comment;
				else {
					outFile.put('/');
					inFile.putback(ch);
					break;
				}
			}

		case '\"':
			return string_literal;
		case '\'':
			return char_literal;
		case '\n':
			return newline;
		default:
			{
				keyMatch(ch, outFile);
			}
		}
	}
	return file_end;
}

void            Parser::
printHeader(char *fileName, ofstream & outFile)
{
	outFile << "<html>\n<head>\n<title>" << fileName << "</title>\n";
	outFile << "<body>\n";
}


void            Parser::
printFooter(ofstream & outFile)
{
	outFile << "</body>\n</html>";
}


int             Parser::
parse(char *fileName)
{
	char           *fileExt;
	ifstream        inFile(fileName);
	char            ofName[64];
	int             len = strlen(fileName);

	strcpy(ofName, fileName);
	char           *index = strrchr(ofName, '.');

	fileExt = index + 1;
	//if ext
		== cpp, load CppParser, etc.

			* index = '\0';
	strcat(ofName, ".html");

	ofstream        outFile(ofName);

	printHeader(ofName, outFile);
	_parse(inFile, outFile);
	printFooter(outFile);

	return 0;
}

void            Parser::
_parse(ifstream & inFile, ofstream & outFile)
{

Parser: :Context context;

	while ((context = handle_code(inFile, outFile)) != file_end)
		switch (context) {
		case multiline_comment:
			handle_multiline_comment(inFile, outFile);
			break;
		case single_line_comment:
			handle_single_comment(inFile, outFile);
			break;
		case string_literal:
			handle_literal('\"', inFile, outFile);
			break;
		case char_literal:
			handle_literal('\'', inFile, outFile);
			break;
		case newline:
			keyMatch('\n', outFile);
			break;
		}
}






Parser: : Parser():keyIndex(0), KEYWORDS(0)
{
}


int 
main(int argc, char *argv[])
{
	Parser         *p = new Parser();
	p->parse("parseFiles.cpp");

	delete          p;
	return 0;
}
