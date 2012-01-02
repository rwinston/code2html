// CodeToHTML version 0.98 

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif 

#include <stdio.h>
#include <errno.h>

// Fixed / inside other delimiters bug 
// Added wildcard support for Win32
// Added proper file extension support
// Added <pre> tag and formatting directive
// Added C++ pointer/reference/address-of semantics
// Added string literal colorization 
// Fixed tab character problem
// Fixed escaped characters in string literal bug
// Fixed <> chars in comments not being handled properly

using namespace std;

const int MAX_KEYWORD = 64;
const int KEYWORDS = 47;

const char* keyWords[KEYWORDS] = { "void", "short", "char", "int", "float", "double", "long",
"while", "enum" , "if", "else", "switch", "case", "break", "default","union", "struct", 
"class" ,"private", "public", "for", "do", "#define", "#include", "#import", "const",
"new", "delete", "this", "friend", "using", "unsigned", "signed", "sizeof",
"return", "true", "false", "bool", "#pragma", "#ifndef", "#endif", "#ifdef", "typedef", "#else", 
"template", "typename", "#else" };

const int MAX_DELIMS = 13;
const char delims[MAX_DELIMS] = { '=', '<' , '>' , ';' , '\n', '\t', '*', '&', ':', '(', ')', ',' , ' ' };

char keyWord[MAX_KEYWORD] = "";
int keyIndex = 0;

int outputChar(char ch, ofstream& outFile) {
	
				if(ch == '>') {
					outFile << "&gt;";
					return 0;
				}
				else if (ch == '<') {
					outFile << "&lt;";
					return 0;
				}
				else if(ch == '\n') {
					outFile << "\n";   
					return 0;
				}
				else if(ch == '&') {
					outFile << "&amp;";
					return 0;
				}
				else {
					outFile.put(ch);
					return 0;
				}
				
}

bool isDelim(char ch) {
	
	for(int i = 0; i < MAX_DELIMS; ++i) 
		if(ch == delims[i])
			return true;
		
		return false;
}


int keyMatch(char ch, ofstream& outFile) {
	
	// no existing keyword, and character cannot be part of a keyword,
	// (i.e. a delimiter), output it
	if(strcmp(keyWord,"") == 0) {
		if(isDelim(ch)) {
			outputChar(ch, outFile);	
			return 0;
		}
	}
	
	
	// delimiter marks the end of a possible existing keyword		
	if(isDelim(ch)) {
		// Search for a match
		for(int i = 0; i < KEYWORDS; ++i) {	  	
			if((strcmp((const char*)keyWord, keyWords[i])) == 0) {
				outFile << "<font color=\"blue\">";
				outFile << keyWord;
				outFile << "</font>";
				outputChar(ch, outFile);
				keyIndex = 0;
				strncpy(keyWord,"\0", 64);
				return 1;
			}
		}
		
		// No match, keyword could be a var name, etc. 
		outFile << keyWord;
		outFile.flush();
		outputChar(ch, outFile);	
		strncpy(keyWord, "\0", 64);
		keyIndex = 0;
		return 0;
		
	}
	else 	
	{
		// tack the character onto the keyword
		keyWord[keyIndex++] = ch;
	}
}

// File states
enum Context { string_literal, char_literal, cpp_comment, c_comment, file_end, newline, keyword };

// C-comment state
void handle_c_comment(ifstream& inFile, ofstream& outFile) {
	
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

// C++ comment state
void handle_cpp_comment(ifstream& inFile, ofstream& outFile) {
	
	char ch;
	outFile << "<font color=\"green\">";
	outFile << "//";
	while(inFile.get(ch) && ch != '\n') 
		outputChar(ch, outFile);
	outFile << "</font>\n";
}

// Literals (char, string)
// Make sure that <,>, etc are escaped inside literals!
void handle_literal(char delimiter, ifstream& inFile, ofstream& outFile) {
	
	outFile << "<font color=\"gray\">";	
	outFile.put(delimiter);
	
	char ch;
	
	while(inFile.get(ch)) {
		
		if(ch == delimiter) {
			outFile.put(ch);
			break;
		}
		else if(ch == '\\'){
			outFile.put(ch);		// TEST
			inFile.get(ch) && outFile.put(ch);
		}
		else
			outputChar(ch, outFile);
	}
	
	outFile << "</font>";
	
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
				else if(ch == '/')
					return cpp_comment;
				else {
					cout << "Context = / followed by reg. text, ch = " << ch << endl;
					outFile << keyWord;
					keyIndex = 0;
					strncpy(keyWord,"\0", 64);
					outFile.put('/');
					inFile.putback(ch);
					break;
				}
			}
			
		case '\"' : return string_literal;
		case '\'' : return char_literal;
		case '\n' : return newline;
			
		default: 
			{
				keyMatch(ch, outFile);
			}
		}
	}
	
	return file_end;
	
}

#ifdef _WIN32

int error(int errCode, char* message, char* param)
{
	cerr << "ERROR:" << errCode <<  ' ' << message << ' ' <<  param << endl;
	return 0;
}



void expand_wild (int argc, char** argv, int* pargc, char*** pargv)
{
	int i;
	int new_argc;
	char **new_argv;
	/* Allocated size of new_argv.  We arrange it so there is always room for
	one more element.  */
	int max_new_argc;
	
	new_argc = 0;
	/* Add one so this is never zero.  */
	max_new_argc = argc + 1;
	new_argv = (char **) malloc (max_new_argc * sizeof (char *));
	
	for (i = 0; i < argc; ++i)
	{
        HANDLE h;
        WIN32_FIND_DATA fdata;
		
        /* These variables help us extract the directory name from the
		given pathname. */
		
        char *last_forw_slash, *last_back_slash, *end_of_dirname;
        int dirname_length = 0;
		
        /* FIXME: If argv[i] is ".", this code will expand it to the
		name of the current directory in its parent directory which
		will cause start_recursion to do all manner of strange things
		with it (culminating in an error).  This breaks "cvs co .".
		As nearly as I can guess, this bug has existed since
		expand_wild was first created.  At least, it is in CVS 1.9 (I
		just tried it).  */
		
        /* FindFirstFile doesn't return pathnames, so we have to do
		this ourselves.  Luckily, it's no big deal, since globbing
		characters under Win32s can only occur in the last segment
		of the path.  For example,
		/a/path/q*.h                      valid
		/w32/q*.dir/cant/do/this/q*.h     invalid */
		
        /* Win32 can handle both forward and backward slashes as
		filenames -- check for both. */
		
        last_forw_slash = strrchr (argv[i], '/');
        last_back_slash = strrchr (argv[i], '\\');
		
#define cvs_max(x,y) ((x >= y) ? (x) : (y))
		
        /* FIXME: this comparing a NULL pointer to a non-NULL one is
		extremely ugly, and I strongly suspect *NOT* sanctioned by
		ANSI C.  The code should just use last_component instead.  */
        end_of_dirname = cvs_max (last_forw_slash, last_back_slash);
		
        if (end_of_dirname == NULL)
			dirname_length = 0;        /* no directory name */
        else
			dirname_length = end_of_dirname - argv[i] + 1; /* include slash */
		
        h = FindFirstFile (argv[i], &fdata);
        if (h == INVALID_HANDLE_VALUE)
        {
            if (GetLastError() == ENOENT)
            {
			/* No match.  The file specified didn't contain a wildcard (in which case
			we clearly should return it unchanged), or it contained a wildcard which
			didn't match (in which case it might be better for it to be an error,
				but we don't try to do that).  */
                new_argv [new_argc++] = strdup(argv[i]);

#ifdef _DEBUG
				cout << "new_argv[" << new_argc << "]=" << argv[i] << endl;
#endif

                if (new_argc == max_new_argc)
                {
                    max_new_argc *= 2;
                    new_argv = (char**)realloc(new_argv, max_new_argc * sizeof (char*));
                }
            }
            else
            {
                error (errno, "cannot find %s", argv[i]);
            }
        }
        else
        {
            while (1)
            {
                new_argv[new_argc] = (char *) malloc (strlen (fdata.cFileName) + 1 + dirname_length);

#ifdef _DEBUG
				cout << "new_argv[" << new_argc << "]=" << argv[i] << endl;
#endif
				
                /* Copy the directory name, if there is one. */
				
                if (dirname_length)
                {
                    strncpy (new_argv[new_argc], argv[i], dirname_length);
                    new_argv[new_argc][dirname_length] = '\0';
                }
                else
                    new_argv[new_argc][0] = '\0';
				
                /* Copy the file name. */
                
                if (strcmp(argv[i] + dirname_length, fdata.cFileName) == 0)
				
				/* We didn't expand a wildcard; we just matched a filename.
				Use the file name as specified rather than the filename
				which exists in the directory (they may differ in case).
				This is needed to make cvs add on a directory consistently
				use the name specified on the command line, but it is
				probably a good idea in other contexts too.  */
				strcpy (new_argv[new_argc], argv[i]);
                else
                    strcat (new_argv[new_argc], fdata.cFileName);
				
				
                new_argc++;
				
                if (new_argc == max_new_argc)
                {
                    max_new_argc *= 2;
                    new_argv = (char**)realloc (new_argv, max_new_argc * sizeof (char *));
                }
                
				if (!FindNextFile (h, &fdata))
                {
                    if (GetLastError () == ERROR_NO_MORE_FILES)
                        break;
                    else
                        error (errno, "cannot find %s", argv[i]);
                }
            }
            if (!FindClose (h))
                error (GetLastError (), "cannot close %s", argv[i]);
        }
     }
     
     *pargc = new_argc;
     *pargv = new_argv;
}

#endif
 



void printHeader(char* fileName, ofstream& outFile) {
	outFile << "<html>\n<head>\n<title>" << fileName << "</title>\n";
	outFile << "<style type=\"text/css\"> PRE { font-family: Verdana; font-size: 8pt } </style>\n";
	outFile << "\n</head>\n<body><pre>\n";
}


void printFooter(ofstream& outFile) {
	outFile << "</pre></body>\n</html>";
}


// Main entry point
int main(int argc, char** argv) {
	Context context;
	
	if(argc < 2) {
		cerr << "Usage: CodeToHTML <infile>" << endl;
		cerr << "Wildcards may be used, e.g. CodeToHTML *.cpp" << endl;
		exit(1);
	}
	
#ifdef _DEBUG
	cout << "argc=" << argc << '\t' << argv[1] << endl;
#endif

#ifdef _WIN32
	expand_wild(argc, argv, &argc, &argv);
#endif

#ifdef _DEBUG
	cout << "argc=" << argc << '\t' << argv[1] << endl;
#endif
	
	for(int i = 1; i < argc; ++i) {
		char ofName[64];
		
		cout << "processing " << argv[i] << "...";

		ifstream inFile(argv[i]);
		strcpy(ofName, argv[i]);
		
		strcat(ofName, ".html");
		ofstream outFile(ofName);
		
		printHeader(ofName, outFile);
		
		while((context = handle_code(inFile, outFile)) != file_end)
			switch(context) {
						case c_comment:
							handle_c_comment(inFile, outFile);
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
						case newline:
							keyMatch('\n', outFile);
							break;
		}
		
		printFooter(outFile);
		outFile.close();

		cout << "done\n";
		
	}
	
	return 0;
}



