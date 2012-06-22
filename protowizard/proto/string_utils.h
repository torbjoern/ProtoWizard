
#include <vector>
#include <string>

namespace protowizard
{

// http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c

inline
void split( std::vector<std::string> &tokens, const std::string &str, char sep )
{
	using std::string;
	int start = 0;
	int end = 0;
	while( (end=str.find_first_of(sep, start)) != string::npos ) {
		tokens.push_back( str.substr( start, end ) );
		start = end+1;
	}
	tokens.push_back( str.substr( start ) );
}

inline
std::vector<std::string> split( const std::string &str, char sep )
{
	std::vector<std::string> tokens;
	split(tokens, str, sep);
	return tokens;
}


}