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
