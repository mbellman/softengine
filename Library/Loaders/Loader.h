#pragma once

#include <string>
#include <vector>

using namespace std;

class Loader {
public:
	virtual ~Loader() = 0;

protected:
	bool isLoading = false;

	void load(const char* filePath);
	string readNextChunk();
	void nextLine();
	void setChunkDelimiter(string delimiter);

private:
	string buffer = "";
	string delimiter = " ";
	FILE* file = 0;

	void fillBufferUntil(string end);
	bool bufferEndsWith(string str);
	bool isAtDelimiter();
	bool isAtEOL();
	int getDelimiterOffset();
	char nextChar();
};
