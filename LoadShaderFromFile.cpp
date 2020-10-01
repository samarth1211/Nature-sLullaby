#include"LoadShaderFromFile.h"

char * LoadShaderSourceFromFile(const char * filename, const char * preamble, size_t * iSize)
{
	
	FILE *pFile = NULL;
	size_t sizeSourceLength;
	size_t sizePreambleLength = (size_t)strlen(preamble);

	if (fopen_s(&pFile, filename, "rb") != 0)
	{
		return NULL;
	}

	fseek(pFile, 0, SEEK_END);
	sizeSourceLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char *sourceString = (char*)calloc(sizeSourceLength + sizePreambleLength + 1, sizeof(char));
	memcpy(sourceString, preamble, sizePreambleLength);//push preabmble

	if (fread((sourceString)+sizePreambleLength, sizeSourceLength, 1, pFile) != 1)
	{
		fclose(pFile);
		free(sourceString);
		return 0;
	}

	fclose(pFile);
	if (iSize != 0)
	{
		*iSize = sizeSourceLength + sizePreambleLength + 1;
	}
	sourceString[sizeSourceLength + sizePreambleLength] = '\0';
	return sourceString;
}
