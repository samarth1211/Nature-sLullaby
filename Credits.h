#pragma once
//#include <windows.h>

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CustomFontRenderer.h"

class CCredits
{
public:
	FILE *pCCreditsFile = NULL;

	CCredits();
	~CCredits();

	void initialize(void);
	void update(void);
	void display(void);
	void resize(int width, int height);

	void displayStartCredits();
	
	void displayEndCredits();

	void fadeInText();
	void fadeOutText();

private:
	enum
	{
		AMC_ATTRIBUTE_POSITION = 0,
		AMC_ATTRIBUTE_COLOR,
		AMC_ATTRIBUTE_NORMAL,
		AMC_ATTRIBUTE_TEXTURE0,
	};

	bool bPlayEndCredits;
	const char* paragraphSeparator = "-----";
	const GLfloat endCreditsStartPosition = -3.60f; //-3.75f;
	const GLfloat endCreditsEndPosition = 2.25f; //1.5f;
	const GLfloat endCreditsTranslationFactor = 0.01f;
	   	
	glm::mat4 perspectiveProjectionMatrix;
	glm::vec3 endCreditsTranslation;

	CCustomFontRenderer *pCustFontRenderer = NULL;
	TextData *titleTextData = NULL;
	
	std::vector<TextData *> endCreditsTextData;
	
	TextData *groupNameTextData = NULL;
	TextData *projectTitleTextData = NULL;	
	TextData *spaceBarNoteTextData = NULL;

	glm::vec3 titleColor;
	glm::vec3 groupNameColor;
	glm::vec3 projectTitleColor;
	glm::vec3 spaceBarNoteColor;

	glm::vec3 endCreditsColor;

	void initializefontRenderer(void);
	void loadStartCredits();
	void loadEndCredits();

	void loadTextDataFromFile(const char * fileName, glm::vec3 textPosition, glm::vec3 color, std::vector<TextData *> &textDataList);
	TextData* generateTextData(const char * line, glm::vec3 position, glm::vec3 color);
	
	int readFile(const char * fileName, char **data);

	void cleanUp(void);
};
