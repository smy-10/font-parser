#ifndef PARSER_H
#define PARSER_H

#include <set>
#include <vector>
#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

//#include "export.h"

namespace fontview {
	class FontStyle;

}
typedef std::map<int, std::string> NameTable;
class  Parser {

public:
	Parser();
	~Parser();

	void run(const char* stream, int size);
	void run(const char* filePath);

	std::string format() const;

	void clear();
private:
	void runImpl(const char* stream, int size);

private:
	std::vector<FT_Face> m_faces;
	std::vector<NameTable*> m_faceNameTables;
	std::vector<fontview::FontStyle*> m_styles;
	std::set<std::string> m_families;
	std::string m_family;
};

#endif // PARSER_H