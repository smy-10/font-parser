/*
 * Copyright 2017 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <map>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H
#include "util.h"
#include "name_table.h"

namespace fontview {

	NameTable* BuildNameTable(FT_Face face) {
		NameTable* result = new NameTable();
		const FT_UInt numNames = FT_Get_Sfnt_Name_Count(face);

		FT_SfntName name;
		for (FT_UInt i = 0; i < numNames; ++i) {
			if (FT_Get_Sfnt_Name(face, i, &name) != 0) {
				continue;
			}

			auto nameStr = FcSfntNameTranscode(&name);
			(*result)[name.name_id] = nameStr;
		}

		// Add a family name if we haven't one yet, eg. for Type 1 PostScript fonts.
		if (result->count(1) == 0 && face->family_name) {
			(*result)[1] = std::string(face->family_name);
		}

		// Add a style name if we haven't one yet, eg. for Type 1 PostScript fonts.
		if (result->count(2) == 0 && face->style_name) {
			(*result)[2] = std::string(face->style_name);
		}

		return result;
	}


	static const std::string EMPTY_STRING;

	const std::string& GetFontName(const NameTable& names, int id) {
		NameTable::const_iterator iter = names.find(id);
		if (iter != names.end()) {
			return iter->second;
		}
		else {
			return EMPTY_STRING;
		}
	}

	const std::string& GetFontFamilyName(const NameTable& names) {
		const std::string& name = GetFontName(names, 16);
		if (!name.empty()) {
			return name;
		}

		return GetFontName(names, 1);
	}

	const std::string& GetFontStyleName(const NameTable& names) {
		const std::string& name = GetFontName(names, 17);
		if (!name.empty()) {
			return name;
		}

		return GetFontName(names, 2);
	}
}  // namespace fontview
