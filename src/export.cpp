#include "export.h"
#include "parser.h"

namespace {
	char *cpyStr(const std::string &string) {
		char *str = new char[string.size() + 1];
		memset(str, 0, string.size() + 1);
		memcpy(str, string.c_str(), string.size());
		return str;
	}
}

DLL_EXPORT char* parseFontData(char* fontData, int size) {
	Parser p;
	p.run(fontData, size);
	return cpyStr(p.format());
}
DLL_EXPORT char *parseFontFile(char *fontPath) {
	Parser p;
	p.run(fontPath);
	return cpyStr(p.format());
}

DLL_EXPORT void  freeString(char* str) {
	if (str) 
		delete[] str;
}