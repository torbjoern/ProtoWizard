#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "proto/fileio/text_file.h"

using std::cout;
using std::endl;

TextFile::TextFile(const std::string& path)
{
	parse_char_ptr = 0x0;

	input_file = std::ifstream(path);
	if ( input_file.good() == false )
	{
		cout << path << " cannot be read" << endl;
	}
}
TextFile::~TextFile(){
	cout << "be nice to the os. closing file" << endl;
	input_file.close();
}
void TextFile::fail_check()
{
	if ( (input_file.rdstate() & std::ifstream::failbit ) != 0 )
		cout << "Error opening failbit 'test.txt'\n";

	if ( (input_file.rdstate() & std::ifstream::badbit ) != 0 )
		cout << "Error opening BADbit 'test.txt'\n";
			
	cout << "are we at eof? ";
	if ( (input_file.rdstate() & std::ifstream::eofbit ) != 0 )
		cout << "yup! \n";
	else
		cout << "nop! \n";
}
	
int TextFile::numLines()
{
	rewind();
	return std::count( std::istreambuf_iterator<char>(input_file),
				std::istreambuf_iterator<char>(), '\n');
}
	
std::string TextFile::getLine(int i)
{
	int num_lines = 0;
	rewind();
		
	std::string line;
	while( std::getline(input_file, line )  )
	{
		if ( num_lines == i ) return line;
		num_lines++;
	}
	return std::string("failed to getLine");
}
	
void TextFile::setParseLine(int i){
	// this might be stupid. better to load a larger buffer into strtok
	// and then advance that using strtok(NULL, "\r"); instead of
	// rewinding file, skipping all lines not wanted
	// and then parsing found line... ad infinitum
	std::string line = getLine(i); 
	size_t len = std::max<int>( line.length()+1, MAX_LINE_LENGTH-1 );
	strcpy_s( parse_line, len, line.c_str() );
	parse_char_ptr = strtok(parse_line, " ");		
}
	
float TextFile::getFloat()
{
	if ( parse_char_ptr == 0x0 ) return HUGE_VAL;
		
	float val = atof( parse_char_ptr );
	parse_char_ptr = strtok(0x0, " ");
	return val;
}
	
int TextFile::getInt()
{
	if ( parse_char_ptr == 0x0 ) return INT_MAX;
		
	int val = atoi( parse_char_ptr );
	parse_char_ptr = strtok(0x0, " ");
	return val;
}
	
void TextFile::rewind()
{
	input_file.seekg (0, std::ios::beg);
}
