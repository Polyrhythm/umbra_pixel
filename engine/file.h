#pragma once

#include <vector>

char* readTextFile(char* file);

class FileWatcher {
	private:
		struct _stat64 fileinfo;
		__time64_t fileTimestamp;
		char* m_filepath;
		void checkTimestamp();

	public:
		FileWatcher(char* filepath);
		bool fileChanged();
		char* getFilepath() { return m_filepath; };
};