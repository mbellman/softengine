#include <stdio.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <Loaders/Loader.h>

Loader::~Loader() {}

void Loader::fillBufferUntil(string end) {
	setChunkDelimiter(end);

	char c;

	while (!isAtDelimiter() && !isAtEOL() && (c = nextChar()) != EOF) {
		buffer += c;
	}

	if (c == EOF) {
		fclose(file);

		isLoading = false;
	} else if (isAtDelimiter()) {
		int pos = (int)(buffer.length() - delimiter.length());
		int len = delimiter.length();

		// Ignore the delimiter string at the end of the buffer
		// so chunks can be cleanly parsed with only their contents.
		buffer.erase(pos, len);
	}
}

bool Loader::bufferEndsWith(string str) {
	int pos = std::max((int)(buffer.length() - str.length()), 0);
	int len = str.length();

	return buffer.length() > 0 && buffer.compare(pos, len, str) == 0;
}

bool Loader::isAtDelimiter() {
	return bufferEndsWith(delimiter);
}

bool Loader::isAtEOL() {
	return bufferEndsWith("\n");
}

void Loader::load(const char* filePath) {
	FILE* f = fopen(filePath, "r");

	if (f != NULL) {
		file = f;
		isLoading = true;
	} else {
		printf("[Loader] Error opening file: %s\n", filePath);
	}
}

char Loader::nextChar() {
	return fgetc(file);
}

string Loader::readNextChunk() {
	buffer.clear();
	fillBufferUntil(delimiter);

	return buffer;
}

void Loader::nextLine() {
	buffer.clear();
}

void Loader::setChunkDelimiter(string delimiter) {
	this->delimiter = delimiter;
}
