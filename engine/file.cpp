#include "file.h"

#include <iostream>
#include <fstream>
#include <vector>

char* readTextFile(char *file)
{
	FILE *fptr;
	long length;
	char *buf;

	fptr = fopen(file, "rb");
	if (!fptr) 
		return NULL;
	fseek(fptr, 0, SEEK_END); 
	length = ftell(fptr);
	buf = (char*)malloc(length + 1); 
	fseek(fptr, 0, SEEK_SET); 
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;

	return buf;
}

FileWatcher::FileWatcher(char* filepath) {
	m_filepath = filepath;
	
	checkTimestamp();
}

void FileWatcher::checkTimestamp() {
	if (_stat64(m_filepath, &fileinfo) != -1) {
		fileTimestamp = fileinfo.st_mtime;
	}
	else {
		std::cerr << "FileWatcher can't get timestamp of given file." << std::endl;
	}
}

bool FileWatcher::fileChanged() {
	__time64_t fileTimestamp_tmp = fileTimestamp;

	checkTimestamp();

	return fileTimestamp_tmp != fileTimestamp;
}