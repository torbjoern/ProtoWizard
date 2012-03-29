#include <fstream>
#include <string>

class TextFile
{
private:
	std::ifstream input_file;
	
	#define MAX_LINE_LENGTH 1024
	char parse_line[MAX_LINE_LENGTH];
	char* parse_char_ptr;
public:
	TextFile(const std::string& path);
	~TextFile();
	void fail_check();

	int numLines();
	std::string getLine(int i);
	
	void setParseLine(int i);
	float getFloat();
	int getInt();
	void rewind();
		
};