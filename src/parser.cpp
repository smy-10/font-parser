#include "parser.h"

#include <fstream>
#include "fontview-src/font_style.h"
#include "fontview-src/name_table.h"
#include "fontview-src/font_var_axis.h"
#include "fontview-src/util.h"

using namespace fontview;

Parser::Parser()
{
}

Parser::~Parser()
{
}

void Parser::run(const char* stream, int size)
{
	if (nullptr == stream || size <= 0)
		return;

	runImpl(stream, size);
}

void Parser::run(const char* filePath)
{
	if (nullptr == filePath)
		return;

	std::ifstream handle(filePath, std::ios::binary | std::ios::in);
	handle.seekg(0, std::ios::end);
	int len = handle.tellg();
	handle.seekg(0, std::ios::beg);
	char* stream = new char[len];
	handle.read(stream, len);
	handle.close();

	runImpl(stream, len);

	if (stream)
		delete[] stream;
}

std::string Parser::format() const
{
	// family
	std::string familyStr;
	if (!m_families.empty()){
		familyStr += "[";
		for (const auto &key : m_families) {
			familyStr += "\"" + key + "\",";
		}
		// remove end ","
		familyStr.erase(familyStr.size() - 1, 1);
		familyStr += "]";
	}

	// style
	std::string styleStr;
	if (!m_styles.empty()) {	
		styleStr += "{";
		for (const auto &style : m_styles) {
			// style
			styleStr += "\"styleName\":";
			styleStr += "\"";
			styleStr += style->GetStyleName();
			styleStr += "\",";
			// family
			styleStr += "\"familyName\":";
			styleStr += "\"";
			styleStr += style->GetFamilyName();
			styleStr += "\",";
			// width
			styleStr += "\"width\":";
			styleStr += "\"";
			styleStr += std::to_string(style->GetWidth());
			styleStr += "\",";
			// weight
			styleStr += "\"weight\":";
			styleStr += "\"";
			styleStr += std::to_string(style->GetWeight());
			styleStr += "\",";
			// slant
			styleStr += "\"slant\":";
			styleStr += "\"";
			styleStr += std::to_string(style->GetSlant());
			styleStr += "\",";
			// varAxis
			std::string axisStr;
			if (!style->GetAxes().empty()) {
				axisStr += "{";
				for (const auto &axis : style->GetAxes()) {
					// name
					axisStr += "\"name\":";
					axisStr += "\"";
					axisStr += axis->GetName();
					axisStr += "\",";
					// minValue
					axisStr += "\"minValue\":";
					axisStr += "\"";
					axisStr += std::to_string(axis->GetMinValue());
					axisStr += "\",";
					// maxValue
					axisStr += "\"maxValue\":";
					axisStr += "\"";
					axisStr += std::to_string(axis->GetMaxValue());
					axisStr += "\",";
					// defaultValue
					axisStr += "\"defaultValue\":";
					axisStr += "\"";
					axisStr += std::to_string(axis->GetDefaultValue());
					axisStr += "\",";
				}
				// remove end ","
				axisStr.erase(axisStr.size() - 1, 1);
				axisStr += "}";
			}
			styleStr += "\"axes\":";
			axisStr.empty() ? styleStr += "[]" : styleStr += axisStr;
			styleStr += ",";
		}
		// remove end ","
		styleStr.erase(styleStr.size() - 1, 1);
		styleStr += "}";
	}


	std::string str;
	str += "{";

	// family
	str += "\"families\":";
	familyStr.empty() ? str += "[]" : str += familyStr;
	str += ",";

	// style
	str += "\"styles\":";
	styleStr.empty() ? str += "[]" : str += styleStr;


	str += "}";
	return str;
}

void Parser::clear()
{
	m_faces.clear();
	m_faceNameTables.clear();
	m_styles.clear();
	m_families.clear();
	m_family.clear();
}

void Parser::runImpl(const char* stream, int size)
{
	//refer & modified in fontview text_settings.cpp SetFontContainer()

	std::unique_ptr<std::vector<FT_Face>> faces(LoadFaces(stream, size));
	if (!faces.get() || faces->empty()) {
		return;
	}

	clear();

	for (FT_Face face : *faces) {
		m_faces.emplace_back(face);
		m_faceNameTables.emplace_back(BuildNameTable(face));
	}

	for (NameTable* t : m_faceNameTables) {
		const std::string& familyName = GetFontFamilyName(*t);
		m_families.insert(familyName);
		if (m_family.empty()) {
			m_family = familyName;
		}
	}

	for (size_t i = 0; i < m_faces.size(); ++i) {
		std::vector<fontview::FontStyle*> styles =
			fontview::FontStyle::GetStyles(m_faces[i], *m_faceNameTables[i]);
		for (fontview::FontStyle* s : styles) {
			m_styles.emplace_back(s);
		}
	}
}
