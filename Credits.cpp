#pragma once

#define _USE_MATH_DEFINES

#include "Credits.h"

#include <GL/glew.h>
#include <gl/GL.h>
#include <gl/glew.h>
#include <gl/gl.h>

//-------------------------------------------------------------------------------------------------------------
// CCredits::CCredits
//-------------------------------------------------------------------------------------------------------------
CCredits::CCredits()
{
	bPlayEndCredits = false;
	// Golden color (212,175,55) = (0.83f, 0.68f, 0.21f)
	titleColor = glm::vec3(0.83f, 0.68f, 0.21f);

	// Silver color (192,192,192) = ( 0.75f,0.75f,0.75f)
	groupNameColor = glm::vec3(0.75f, 0.75f, 0.75f);
	// Metallic copper color R:184, G:115, B:51 = (0.72f, 0.45f, 0.2f)
	projectTitleColor = glm::vec3(0.72f, 0.45f, 0.2f);
	
	spaceBarNoteColor = glm::vec3(0.0f, 1.0f, 1.0f);

	endCreditsColor = glm::vec3(1.0f, 1.0f, 0.0f);  // glm::vec3(0.83f, 0.68f, 0.21f);

	// opening position file
	if (fopen_s(&pCCreditsFile, "CreditsLogFile.txt", "w") != 0)
	{
		//MessageBox(NULL, TEXT("Failed to create the CreditsLogFile.txt. Exiting now."), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(pCCreditsFile, "CreditsLogFile.txt created successfully.\n");
	}

}

//-------------------------------------------------------------------------------------------------------------
// CCredits::initializefontRenderer
//-------------------------------------------------------------------------------------------------------------
void CCredits::initialize(void)
{
	initializefontRenderer();

	perspectiveProjectionMatrix = glm::mat4(1.0f);
	endCreditsTranslation = glm::vec3(0.0f, endCreditsStartPosition, 0.0f);

	loadStartCredits();
	loadEndCredits();
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::initializefontRenderer
//-------------------------------------------------------------------------------------------------------------
void CCredits::initializefontRenderer(void)
{
	pCustFontRenderer = new CCustomFontRenderer("fonts/ubuntuMono.ttf", 78);
	FT_Error error = pCustFontRenderer->initialize();

	if (error != 0)
	{		
		fprintf_s(pCCreditsFile, "Not able to initialize FreeType, error code: %d\n", error);
	}
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::loadEndCredits
//-------------------------------------------------------------------------------------------------------------
void CCredits::loadEndCredits()
{
	loadTextDataFromFile("EndCrerditsAndBookRef.rtf", glm::vec3(-4.0f, 0.0f, -6.0f), endCreditsColor, endCreditsTextData);
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::loadStartCredits
//-------------------------------------------------------------------------------------------------------------
void CCredits::loadStartCredits()
{

	titleTextData = generateTextData("ASTROMEDICOMP", glm::vec3(-2.70f, 2.0f, -6.0f), titleColor);
	titleTextData->scale = 0.01f;
	pCustFontRenderer->loadCharacters(titleTextData);
	
	groupNameTextData = generateTextData("DomainShader Group Presentation", glm::vec3(-3.9f, 1.00f, -6.0f), groupNameColor);
	groupNameTextData->scale = 0.006f;
	pCustFontRenderer->loadCharacters(groupNameTextData);
	
	projectTitleTextData = generateTextData("Nature's Lullaby", glm::vec3(-2.8f, -0.7f, -4.0f), projectTitleColor);
	projectTitleTextData->scale = 0.008f;
	pCustFontRenderer->loadCharacters(projectTitleTextData);

	spaceBarNoteTextData = generateTextData("Press Space bar to continue..", glm::vec3(-2.150f, -2.750f, -6.0f), spaceBarNoteColor);
	//spaceBarNoteTextData->scale = 0.008f;
	pCustFontRenderer->loadCharacters(spaceBarNoteTextData);

}

//-------------------------------------------------------------------------------------------------------------
// CCredits::displayEndCredits
//-------------------------------------------------------------------------------------------------------------
void CCredits::displayEndCredits()
{
	bPlayEndCredits = true;
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	modelMatrix = glm::translate(modelMatrix, endCreditsTranslation);

	for (int counter = 0; counter < endCreditsTextData.size(); ++counter)
	{
		TextData *nextTextData = endCreditsTextData[counter];
		GLfloat currentPosition = nextTextData->textPosition[1] + endCreditsTranslation[1];

		if (currentPosition < endCreditsEndPosition)
		{
			pCustFontRenderer->renderText(nextTextData, modelMatrix, viewMatrix, perspectiveProjectionMatrix);
		}
	}

}

//-------------------------------------------------------------------------------------------------------------
// CCredits::loadTextDataFromFile
//-------------------------------------------------------------------------------------------------------------
void CCredits::loadTextDataFromFile(const char * fileName, glm::vec3 textPosition, glm::vec3 color, std::vector<TextData *> &textDataList)
{
	char *data = NULL;
	char *context = NULL;
	char *nextLine = NULL;
	const char *sep = "\n";
	int dataLength = readFile(fileName, &data);
	int column = 0;

	if (dataLength > 0)
	{
		nextLine = strtok_s(data, sep, &context);

		if (nextLine != NULL)
		{
			TextData *nextLineText = NULL;

			if (strlen(nextLine) > 0)
			{
				nextLineText = generateTextData(nextLine, textPosition, color);
				pCustFontRenderer->loadCharacters(nextLineText);
				textDataList.push_back(nextLineText);
				textPosition[1] -= (2.0f * nextLineText->rectSize.height);
			}

			while ((nextLine = strtok_s(NULL, "\n", &context)) != NULL)
			{
				if (strcmp(paragraphSeparator, nextLine) == 0 && strlen(nextLine) > 0)
				{
					textPosition[1] -= (2.0f * nextLineText->rectSize.height);
				}
				else if (strlen(nextLine) > 0)
				{
					nextLineText = generateTextData(nextLine, textPosition, color);
					pCustFontRenderer->loadCharacters(nextLineText);
					textDataList.push_back(nextLineText);
					textPosition[1] -= (2.0f * nextLineText->rectSize.height);
				}
			}
		}
	}

	if (data != NULL)
	{
		free(data);
		data = NULL;
	}
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::generateTextData
//-------------------------------------------------------------------------------------------------------------
TextData* CCredits::generateTextData(const char * line, glm::vec3 position, glm::vec3 color)
{
	int lineSize = (int)strlen(line);

	TextData *nextLineText = (TextData *)malloc(sizeof(TextData));
	memset(nextLineText, 0, sizeof(TextData));

	nextLineText->text = (char *)malloc(sizeof(char *) * lineSize);
	strcpy(nextLineText->text, line);

	nextLineText->textSize = lineSize;
	nextLineText->textColor = color; // glm::vec3(0.0f, 0.0f, 0.0f);
	nextLineText->textPosition = position;
	nextLineText->scale = 0.003f;

	return nextLineText;
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::update
//-------------------------------------------------------------------------------------------------------------
void CCredits::update(void)
{
	if (bPlayEndCredits)
	{

		endCreditsTranslation[1] += endCreditsTranslationFactor;

		//TextData *lastCreditTextData = endCreditsTextData[endCreditsTextData.size() - 1];
		//GLfloat lastCreditTextDataPosition = lastCreditTextData->textPosition[1] + endCreditsTranslation[1];

		//if ((lastCreditTextDataPosition > endCreditsEndPosition))
		//{
		//	//fadeOutText();
		//}
	}

	if (pCustFontRenderer)
	{
		pCustFontRenderer->update();
	}
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::fadeInText
//-------------------------------------------------------------------------------------------------------------
void CCredits::fadeInText()
{	 
	 if (pCustFontRenderer)
	 {
		 pCustFontRenderer->fadeInText();
	 }
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::fadeOutText
//-------------------------------------------------------------------------------------------------------------
void CCredits::fadeOutText()
{
	if (pCustFontRenderer)
	{
		pCustFontRenderer->fadeOutText();
	}
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::display
//-------------------------------------------------------------------------------------------------------------
void CCredits::display(void)
{

	//displayStartCredits();
		
	//displayEndCredits();
	
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::displayStartCredits
//-------------------------------------------------------------------------------------------------------------
void CCredits::displayStartCredits()
{
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	pCustFontRenderer->renderText(titleTextData, modelMatrix, viewMatrix, perspectiveProjectionMatrix);

	pCustFontRenderer->renderText(groupNameTextData, modelMatrix, viewMatrix, perspectiveProjectionMatrix);

	pCustFontRenderer->renderText(projectTitleTextData, modelMatrix, viewMatrix, perspectiveProjectionMatrix);

	pCustFontRenderer->renderText(spaceBarNoteTextData, modelMatrix, viewMatrix, perspectiveProjectionMatrix);
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::resize
//-------------------------------------------------------------------------------------------------------------
void CCredits::resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	perspectiveProjectionMatrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::readFile
//-------------------------------------------------------------------------------------------------------------
int CCredits::readFile(const char * fileName, char **data)
{
	FILE *dataFile = NULL;
	char *textData = NULL;
	long dataLength = 0;

	fopen_s(&dataFile, fileName, "r");

	if (dataFile == NULL)
	{		
		fprintf_s(pCCreditsFile, "Not able to load data from file '%s'\n", fileName);
		return 0;
	}

	fseek(dataFile, 0L, SEEK_END);
	dataLength = ftell(dataFile);
	fseek(dataFile, 0L, SEEK_SET);

	textData = (char*)calloc(dataLength + 1, sizeof(char));

	if (textData != NULL)
	{
		fread(textData, sizeof(char), dataLength, dataFile);
	}
	else
	{		
		fprintf_s(pCCreditsFile, "Not able to allocate memory.");
		fclose(dataFile);
		return 0;
	}

	fclose(dataFile);

	*data = textData;

	return dataLength + 1;
}

//-------------------------------------------------------------------------------------------------------------
// CCredits::cleanUp
//-------------------------------------------------------------------------------------------------------------
void CCredits::cleanUp(void)
{
	if (titleTextData)
	{
		titleTextData->vertices.clear();
		free(titleTextData->text);
		free(titleTextData);
		titleTextData = NULL;
	}

	if (groupNameTextData)
	{
		groupNameTextData->vertices.clear();
		free(groupNameTextData->text);
		free(groupNameTextData);
		groupNameTextData = NULL;
	}

	if (projectTitleTextData)
	{
		projectTitleTextData->vertices.clear();
		free(projectTitleTextData->text);
		free(projectTitleTextData);
		projectTitleTextData = NULL;
	}

	if (spaceBarNoteTextData)
	{
		spaceBarNoteTextData->vertices.clear();
		free(spaceBarNoteTextData->text);
		free(spaceBarNoteTextData);
		spaceBarNoteTextData = NULL;
	}

	for (int counter = 0; counter < endCreditsTextData.size(); ++counter)
	{
		if (endCreditsTextData[counter] != NULL)
		{
			endCreditsTextData[counter]->vertices.clear();
			free(endCreditsTextData[counter]->text);
			free(endCreditsTextData[counter]);
			endCreditsTextData[counter] = NULL;
		}
	}

	endCreditsTextData.clear();

	if (pCustFontRenderer != NULL)
	{
		delete pCustFontRenderer;
		pCustFontRenderer = NULL;
	}

	if (pCCreditsFile)
	{
		fprintf_s(pCCreditsFile, "Closing CreditsLogFile File \n");
		fclose(pCCreditsFile);
		pCCreditsFile = NULL;
	}

}

//-------------------------------------------------------------------------------------------------------------
// CCredits::~CCredits()
//-------------------------------------------------------------------------------------------------------------
CCredits::~CCredits()
{
	cleanUp();
}
