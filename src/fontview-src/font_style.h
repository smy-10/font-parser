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

#ifndef FONTVIEW_FONT_STYLE_
#define FONTVIEW_FONT_STYLE_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <ft2build.h>
#include FT_TYPES_H
#include "freetype/freetype.h"

namespace fontview {
	typedef std::map<int, std::string> NameTable;
	class FontVarAxis;

	class FontStyle {
	public:
		typedef std::map<FT_Tag, double> Variation;
		static std::vector<FontStyle*> GetStyles(
			FT_Face face, const NameTable& names);
		~FontStyle();

		FT_Face GetFace(const Variation& variation) const;
		const std::string& GetFamilyName() const;
		const std::string& GetStyleName() const { return styleName_; }

		double GetWeight() const { return weight_; }
		double GetWidth() const { return width_; }
		double GetSlant() const { return slant_; }

		const std::vector<FontVarAxis*>& GetAxes() const { return *axes_; }
		double GetDistance(const Variation& var) const;
		const Variation& GetVariation() const { return variation_; }

	private:

		FontStyle(FT_Face face, const NameTable& names,
			const std::string& styleName,
			std::vector<FontVarAxis*>* axes,  // takes ownership
			const Variation& variation);

		FT_Face face_;
		NameTable names_;
		const std::string family_;
		const std::string styleName_;
		const double weight_, width_, slant_;
		std::unique_ptr<std::vector<FontVarAxis*>> axes_;  // also owning elements
		const Variation variation_;
	};

}  // namespace fontview

#endif // FONTVIEW_FONT_STYLE_
