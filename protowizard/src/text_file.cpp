#include <fstream>
#include <string>

#include <proto/fileio/text_file.h>


std::vector<std::string> TextFile::ReadAllLines( const std::string &path )
{
	auto input_file = std::ifstream(path);
	std::vector<std::string> lines;
	if ( input_file.good() == false )
	{
		throw "file can't be read";
	} else {
		std::string line;
		while( std::getline(input_file, line )  )
		{
			lines.push_back(line);
		}
	}
	return lines;
}	

