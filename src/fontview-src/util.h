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

#ifndef FONTVIEW_UTIL_H_
#define FONTVIEW_UTIL_H_

#include <vector>

#include <ft2build.h>
#include "freetype/ftsnames.h"
#include "freetype/ttnameid.h"
#include FT_TYPES_H

#include "unicode/ucnv.h"


static FT_Library freeTypeLibrary_ = NULL;

namespace fontview {
	static inline FT_Library GetFreeTypeLibrary() {
		if (nullptr == freeTypeLibrary_) {
			FT_Init_FreeType(&freeTypeLibrary_);
		}
		return freeTypeLibrary_;
	}

	inline double FTFixedToDouble(FT_Fixed value) {
		// The cast to FT_Int32 is needed because FreeType defines FT_Fixed as
		// 'signed long', which is 64 bits on 64-bit platforms, but without
		// sign-extending negative values returned by its APIs.
		// https://savannah.nongnu.org/bugs/index.php?48732
		return static_cast<FT_Int32>(value) / 65536.0;
	}

	inline FT_Fixed FTDoubleToFixed(double value) {
		return static_cast<FT_Fixed>(value * 65536);
	}

	inline double clamp(double value, double min, double max) {
		if (value < min) {
			return min;
		}
		else if (value > max) {
			return max;
		}
		else {
			return value;
		}
	}

	static bool AttachExternalMetrics(FT_Face face, const std::string& path) {
		// 已经不需要了
		// https://community.adobe.com/t5/type-typography-discussions/how-to-install-multiple-master-fonts-on-windows-10/td-p/11062530
		/*wxFileName filename(path);
		std::string metricsFile;

		filename.SetExt("mmm");
		metricsFile = filename.GetFullPath().ToStdString();
		if (FT_Attach_File(face, metricsFile.c_str()) == 0) {
			return true;
		}

		filename.SetExt("pfm");
		metricsFile = filename.GetFullPath().ToStdString();
		if (FT_Attach_File(face, metricsFile.c_str()) == 0) {
			return true;
		}

		filename.SetExt("afm");
		metricsFile = filename.GetFullPath().ToStdString();
		if (FT_Attach_File(face, metricsFile.c_str()) == 0) {
			return true;
		}*/

		return false;
	}

	static std::vector<FT_Face>* LoadFaces(const char* stream, int len) {
		FT_Library freeTypeLib = GetFreeTypeLibrary();
		std::vector<FT_Face>* faces = new std::vector<FT_Face>();
		FT_Long numFaces = 0;
		FT_Face face = NULL;
		FT_Error error = FT_New_Memory_Face(freeTypeLib, (FT_Byte*)stream, len, -1, &face);
		//FT_Error error = FT_New_Face(freeTypeLib, path.c_str(), -1, &face);
		bool hasExternalMetrics = false;
		if (face) {
			if (!error) {
				numFaces = face->num_faces;
				//hasExternalMetrics = AttachExternalMetrics(face, path);
			}
			FT_Done_Face(face);
		}
		for (FT_Long faceIndex = 0; faceIndex < numFaces; ++faceIndex) {
			face = NULL;
			if (FT_New_Memory_Face(freeTypeLib, (FT_Byte*)stream, len, faceIndex, &face)) {
				continue;
			}
			//if (hasExternalMetrics) {
				//AttachExternalMetrics(face, path);
			//}
			faces->push_back(face);
		}
		return faces;
	}

}  // namespace fontview

/// <summary>
/// //////stringConvertot
/// </summary>
constexpr const int s_bufferSize = 256;
class StringConvertor {
public:
	StringConvertor(const char* convertName)
	{
		if (nullptr == convertName)
			return;
		UErrorCode errorCode = U_ZERO_ERROR;
		m_handle = ucnv_open(convertName, &errorCode);
	}

	~StringConvertor()
	{
		if (m_handle)
			ucnv_close(m_handle);
	}

	UConverter* getHandle() { return m_handle; }

	void decodeData(const char* readData, size_t dataSize)
	{
		if (nullptr == m_handle || nullptr == readData || dataSize <= 0)
			return;

		auto readPos = readData;
		auto readEnd = readPos + dataSize;

		UChar decodeBuffer[s_bufferSize];

		UChar* writeBegin = decodeBuffer;
		UChar* writeEnd = decodeBuffer + s_bufferSize;

		auto error = U_ZERO_ERROR;
		do {
			auto writePos = writeBegin;

			error = U_ZERO_ERROR;

			ucnv_toUnicode(m_handle, &writePos, writeEnd, &readPos, readEnd, nullptr, true, &error);

			m_data.append(decodeBuffer, writePos);
		} while (U_BUFFER_OVERFLOW_ERROR == error);
	};
	std::string encodeData(const char* convertName)
	{
		StringConvertor       convertor(convertName);
		auto handle = convertor.getHandle();
		if (nullptr == handle)
			return {};

		const UChar* readPos = m_data.data();
		const UChar* readEnd = readPos + m_data.length();

		char encodeBuffer[s_bufferSize];

		char* writeBegin = encodeBuffer;
		char* writeEnd = encodeBuffer + s_bufferSize;

		std::string encodedData;

		auto error = U_ZERO_ERROR;
		do {
			auto writePos = writeBegin;

			error = U_ZERO_ERROR;

			ucnv_fromUnicode(handle, &writePos, writeEnd, &readPos, readEnd, nullptr, true, &error);

			encodedData.append(encodeBuffer, writePos);
		} while (U_BUFFER_OVERFLOW_ERROR == error);

		return encodedData;
	};
private:
	UConverter* m_handle = nullptr;
	std::u16string m_data;
};


/// <summary>
/// //////from fontconfig/fcfreetype.c
/// </summary>
typedef struct {
	const FT_UShort platform_id;
	const FT_UShort encoding_id;
	const char      fromcode[12];
} FcFtEncoding;

typedef int            FcBool;
typedef unsigned char  FcChar8;
typedef unsigned short FcChar16;
typedef unsigned int   FcChar32;

typedef enum {
	FcEndianBig,
	FcEndianLittle
} FcEndian;

#define FcFalse    0
#define FcTrue     1
#define FcDontCare 2

#define TT_ENCODING_DONT_CARE 0xffff
#define FC_ENCODING_MAC_ROMAN "MACINTOSH"

static const FcFtEncoding fcFtEncoding[] = {
	{TT_PLATFORM_APPLE_UNICODE, TT_ENCODING_DONT_CARE, "UTF-16BE"},
	{TT_PLATFORM_MACINTOSH, TT_MAC_ID_ROMAN, "MACINTOSH"},
	{TT_PLATFORM_MACINTOSH, TT_MAC_ID_JAPANESE, "SJIS"},
	{TT_PLATFORM_MICROSOFT, TT_MS_ID_SYMBOL_CS, "UTF-16BE"},
	{TT_PLATFORM_MICROSOFT, TT_MS_ID_UNICODE_CS, "UTF-16BE"},
	{TT_PLATFORM_MICROSOFT, TT_MS_ID_SJIS, "SJIS-WIN"},
	{TT_PLATFORM_MICROSOFT, TT_MS_ID_GB2312, "GB2312"},
	{TT_PLATFORM_MICROSOFT, TT_MS_ID_BIG_5, "BIG-5"},
	{TT_PLATFORM_MICROSOFT, TT_MS_ID_WANSUNG, "Wansung"},
	{TT_PLATFORM_MICROSOFT, TT_MS_ID_JOHAB, "Johab"},
	{TT_PLATFORM_MICROSOFT, TT_MS_ID_UCS_4, "UTF-16BE"},
	{TT_PLATFORM_ISO, TT_ISO_ID_7BIT_ASCII, "ASCII"},
	{TT_PLATFORM_ISO, TT_ISO_ID_10646, "UTF-16BE"},
	{TT_PLATFORM_ISO, TT_ISO_ID_8859_1, "ISO-8859-1"},
};


typedef struct {
	FT_UShort language_id;
	char      fromcode[12];
} FcMacRomanFake;

static const FcMacRomanFake fcMacRomanFake[] = {
	{TT_MS_LANGID_JAPANESE_JAPAN, "SJIS-WIN"},
	{TT_MS_LANGID_ENGLISH_UNITED_STATES, "ASCII"},
};
#define NUM_FC_MAC_ROMAN_FAKE (int)(sizeof(fcMacRomanFake) / sizeof(fcMacRomanFake[0]))

/*
 * A shift-JIS will have many high bits turned on
 */
static FcBool FcLooksLikeSJIS(FcChar8* string, int len)
{
	int nhigh = 0, nlow = 0;

	while (len-- > 0) {
		if (*string++ & 0x80)
			nhigh++;
		else
			nlow++;
	}
	/*
	 * Heuristic -- if more than 1/3 of the bytes have the high-bit set,
	 * this is likely to be SJIS and not ROMAN
	 */
	if (nhigh * 2 > nlow)
		return FcTrue;
	return FcFalse;
}

// from fontconfig/fcfreetype.c
static std::string FcSfntNameTranscode(FT_SfntName* sname)
{
	int         i;
	const char* fromcode = nullptr;

	std::string u8Str(sname->string, sname->string + sname->string_len);

	const int NUM_FC_FT_ENCODING = (int)(sizeof(fcFtEncoding) / sizeof(fcFtEncoding[0]));

	for (i = 0; i < NUM_FC_FT_ENCODING; i++) {
		if (fcFtEncoding[i].platform_id == sname->platform_id &&
			(fcFtEncoding[i].encoding_id == TT_ENCODING_DONT_CARE || fcFtEncoding[i].encoding_id == sname->encoding_id))
			break;
	}
	if (i >= NUM_FC_FT_ENCODING)
		return u8Str;
	fromcode = fcFtEncoding[i].fromcode;

	if (!strcmp(fromcode, FC_ENCODING_MAC_ROMAN)) {
		if (sname->language_id == TT_MAC_LANGID_ENGLISH && FcLooksLikeSJIS(sname->string, sname->string_len)) {
			fromcode = "Shift_JIS";
		}
		else if (sname->language_id >= 0x100) {
			int f;

			fromcode = nullptr;
			for (f = 0; f < NUM_FC_MAC_ROMAN_FAKE; f++)
				if (fcMacRomanFake[f].language_id == sname->language_id) {
					fromcode = fcMacRomanFake[f].fromcode;
					break;
				}
			if (!fromcode)
				return u8Str;
		}
	}
	//    if (!strcmp(fromcode, "UCS-2BE") || !strcmp(fromcode, "UTF-16BE") || !strcmp(fromcode, "ASCII") ||
	//        !strcmp(fromcode, "ISO-8859-1") || !strcmp(fromcode, FC_ENCODING_MAC_ROMAN))
	FcChar8* src = sname->string;
	unsigned int src_len = sname->string_len;

	// 大小写  big-5都兼容
	StringConvertor convertor(fromcode);
	convertor.decodeData(reinterpret_cast<const char*>(src), src_len);

	std::string result = convertor.encodeData("utf-8");
	if (!result.empty()) {
		result.erase(std::remove(result.begin(), result.end(), '\0'), result.end());
		return result;
	}
	return u8Str;
}




#endif  // FONTVIEW_UTIL_H_
