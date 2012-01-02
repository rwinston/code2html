#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <strstream>

using namespace std;

typedef map<string, vector<int> > LineIndex;
typedef vector<string>::const_iterator EntryIt;

LineIndex* make_index(istream& input, vector<string> const& entries) {
		LineIndex* index = new LineIndex;

		for(EntryIt p = entries.begin(); p != entries.end(); ++p)
				(*index)[*p];	// causes insertion ?

		string line, word;
		int line_number = 0;

		// read a line from input at a time into a string
		while(getline(input, line)){
				++line_number;
				
				// make an istream from the string
				istrstream words(static_cast<const char*>(line.c_str()));
	
				// read a word at a time from string
				while(words >> word) {

						// use find() on map<string, vector<int>> to see
						// if it contains the word already, if it does,
						// save the line number in the map's vector
						LineIndex::iterator p = index->find(word);
						if(p != index->end())
								(*p).second.push_back(line_number);
				}
		}
}
		
int main() {
		vector<string> entries;

		entries.push_back("kernel");
		entries.push_back("printf");

		LineIndex* index = make_index(cin, entries);
		
		vector<string>::iterator p = entries.begin();		
		for(; p != entries.end(); ++p) {
				cout <<  "Word " << *p << " appears in lines ";
						LineIndex::iterator lines = index->find(*p);
				copy((*lines).second.begin(), (*lines).second.end(),
								ostream_iterator<int>(cout, ", "));
				cout << ".\n";
		}
}
