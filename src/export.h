#ifndef EXPORT_H
#define EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  DLL_EXPORT
#else
#define DLL_EXPORT _declspec(dllexport)
#endif

	///////////////////////EXPORT//////////////////////////

	DLL_EXPORT char* parseFontData(char* fontData, int size);
	DLL_EXPORT char* parseFontFile(char *fontPath);
	DLL_EXPORT void  freeString(char* str);

	///////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif //EXPORT_H