#include "CustomFontRenderer.h"

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0,
};

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::CCustomFontRenderer
//-------------------------------------------------------------------------------------------------------------
CCustomFontRenderer::CCustomFontRenderer(const char* fontFile, int fontSize)
{
	this->fontFile = fontFile;
	this->fontSize = fontSize;

	// opening position file
	if (fopen_s(&pCustFontFile, "CustFontRenderLog.txt", "w") != 0)
	{
		//MessageBox(NULL, TEXT("Failed to create the CustFontRenderLog.txt. Exiting now."), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(pCustFontFile, "CustFontRenderLog.txt created successfully.\n");
	}

}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::initialize
//-------------------------------------------------------------------------------------------------------------
FT_Error CCustomFontRenderer::initialize()
{
	FT_Error error = FT_Init_FreeType(&freeType);

	if (error != 0)
	{
		fprintf_s(pCustFontFile, "Not able to initialize FreeType, error code: %d\n", error);
		return error;
	}

	error = FT_New_Face(freeType, fontFile, 0, &fontFace);

	if (error != 0)
	{		
		fprintf_s(pCustFontFile, "Not able to load font face, error code: %d\n", error);
		return error;
	}

	// Font's width and height parameters, setting the width to 0 lets the face dynamically calculate the width
	// based on the given height.
	error = FT_Set_Pixel_Sizes(fontFace, 0, fontSize);

	if (error != 0)
	{		
		fprintf_s(pCustFontFile, "Not able to set font size, error code: %d\n", error);
		return error;
	}

	initializeVertexShader();
	initializeFragmentShader();
	initializeShaderProgram();
	initializeTextBuffers();

	return 0;
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::loadCharacters
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::loadCharacters(TextData *textData)
{
	char *charactersToLoad = textData->text;
	int numberOfCharacters = textData->textSize;

	for (int counter = 0; counter < numberOfCharacters; ++counter)
	{
		GLchar characterToLoad = charactersToLoad[counter];

		if (characters.find(characterToLoad) != characters.end())
		{
			// We have already loaded character data, move to next one.
			continue;
		}

		// Load character glyph.
		FT_Error error = FT_Load_Char(fontFace, characterToLoad, FT_LOAD_RENDER);

		if (error != 0)
		{			
			fprintf_s(pCustFontFile, "Cannot load character '%c', error code: %d\n", characterToLoad, error);
			continue;
		}

		GLuint textureId = 0;
		glGenTextures(1, &textureId);
		// For programmable pipeline set 1 instead of 4 for better performation.
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// Generate the mipmapped texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, fontFace->glyph->bitmap.buffer);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Unbind the texture.
		glBindTexture(GL_TEXTURE_2D, 0);

		TextCharacter *nextChar = (TextCharacter *)malloc(sizeof(TextCharacter));
		memset(nextChar, 0, sizeof(TextCharacter));

		nextChar->textChar = characterToLoad;
		nextChar->textureId = textureId;
		nextChar->size = glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows);
		nextChar->bearing = glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top);
		nextChar->advance = glm::ivec2(fontFace->glyph->advance.x, fontFace->glyph->advance.y);

		characters[characterToLoad] = nextChar;
	}

	calculateTextVertices(textData);
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::initializeVertexShader
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::initializeVertexShader(void)
{
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderCode = "#version 430 core" \
		"\n" \
		"in vec3 vertexPosition;" \
		"in vec2 vertexTextureCoordinate0;" \
		"\n" \
		"out vec2 outVertexTextureCoordinate0;" \
		"\n" \
		"uniform mat4 modelMatrix;" \
		"uniform mat4 viewMatrix;" \
		"uniform mat4 projectionMatrix;" \
		"\n" \
		"void main(void)" \
		"{" \
		"   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);" \
		"   outVertexTextureCoordinate0 = vertexTextureCoordinate0;" \
		"}";

	glShaderSource(vertexShaderObject, 1, (const char**)&vertexShaderCode, NULL);
	glCompileShader(vertexShaderObject);

	GLint infoLogLength = 0;
	GLint shaderCompileStatus = 0;
	char *infoLog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);

	if (shaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			infoLog = (char *)malloc(infoLogLength);

			if (infoLog != NULL)
			{
				GLsizei written = 0;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, infoLog);
				fprintf_s(pCustFontFile, "Vertex shader compilation log: %s\n", infoLog);
				free(infoLog);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::initializeFragmentShader
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::initializeFragmentShader(void)
{
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderCode = "#version 430 core" \
		"\n" \
		"in vec2 outVertexTextureCoordinate0;" \
		"\n" \
		"out vec4 fragmentColor;" \
		"\n" \
		"uniform sampler2D textureSampler0;"
		"uniform vec3 textColor;"
		"uniform float u_alpha;"
		"\n" \
		"void main(void)" \
		"{" \
		"    vec4 sampled = vec4(1.0,1.0,1.0,texture(textureSampler0, outVertexTextureCoordinate0).r);" \
		"    fragmentColor = vec4(textColor, u_alpha) * sampled;" \
		"}";

	glShaderSource(fragmentShaderObject, 1, (const char**)&fragmentShaderCode, NULL);
	glCompileShader(fragmentShaderObject);

	GLint infoLogLength = 0;
	GLint shaderCompileStatus = 0;
	char *infoLog = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);

	if (shaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			infoLog = (char *)malloc(infoLogLength);

			if (infoLog != NULL)
			{
				GLsizei written = 0;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, infoLog);
				//logError("Fragment shader compilation log: %s\n", infoLog);
				fprintf_s(pCustFontFile, "Fragment shader compilation log: %s\n", infoLog);
				free(infoLog);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::initializeShaderProgram
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::initializeShaderProgram(void)
{
	shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	// Bind the position attribute location before linking.
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "vertexPosition");

	// Bind the texture attribute location before linking.
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXTURE0, "vertexTextureCoordinate0");

	// Now link and check for error.
	glLinkProgram(shaderProgramObject);

	GLint infoLogLength = 0;
	GLint shaderProgramLinkStatus = 0;
	char *infoLog = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			infoLog = (char *)malloc(infoLogLength);

			if (infoLog != NULL)
			{
				GLsizei written = 0;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, infoLog);
				fprintf_s(pCustFontFile, "Shader program link log: %s\n", infoLog);
				free(infoLog);
			}
		}
	}

	// After linking get the value of modelView and projection uniform location from the shader program.
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "modelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "viewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "projectionMatrix");
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "textureSampler0");
	textColorUniform = glGetUniformLocation(shaderProgramObject, "textColor");
	alphaUniform = glGetUniformLocation(shaderProgramObject, "u_alpha");
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::initializeTextBuffers
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::initializeTextBuffers(void)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vboPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboPosition);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vboTexture);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexture);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::calculateTextVertices
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::calculateTextVertices(TextData *textData)
{
	GLfloat xPosition = textData->textPosition[0];
	textData->vertices.clear();

	GLfloat totalWidth = 0.0f;
	GLfloat maxHeight = 0.0f;

	for (GLsizeiptr counter = 0; counter < textData->textSize; ++counter)
	{
		TextCharacter *nextChar = characters[textData->text[counter]];

		xPosition += nextChar->bearing[0] * textData->scale;
		GLfloat yPosition = textData->textPosition[1] - (nextChar->size[1] - nextChar->bearing[1]) * textData->scale;
		GLfloat width = nextChar->size[0] * textData->scale;
		GLfloat height = nextChar->size[1] * textData->scale;

		totalWidth += width;

		if (height >= maxHeight)
		{
			maxHeight = height;
		}

		std::vector<GLfloat> vertices;
		vertices.push_back(xPosition);
		vertices.push_back(yPosition + height);
		vertices.push_back(textData->textPosition[2]);

		vertices.push_back(xPosition);
		vertices.push_back(yPosition);
		vertices.push_back(textData->textPosition[2]);

		vertices.push_back(xPosition + width);
		vertices.push_back(yPosition);
		vertices.push_back(textData->textPosition[2]);

		vertices.push_back(xPosition);
		vertices.push_back(yPosition + height);
		vertices.push_back(textData->textPosition[2]);

		vertices.push_back(xPosition + width);
		vertices.push_back(yPosition);
		vertices.push_back(textData->textPosition[2]);

		vertices.push_back(xPosition + width);
		vertices.push_back(yPosition + height);
		vertices.push_back(textData->textPosition[2]);

		textData->vertices.push_back(vertices);

		// Now advance the cursors for next glyph, advance is number of 1/64 pixels
		// So bitshift by 6 to get value in pixels (2^6 = 64)
		xPosition += (nextChar->advance[0] >> 6) * textData->scale;
	}

	textData->rectSize.width = totalWidth;
	textData->rectSize.height = maxHeight;
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::renderText
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::renderText(TextData *textData, glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 perspectiveProjectionMatrix)
{
	glUseProgram(shaderProgramObject);

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, glm::value_ptr(perspectiveProjectionMatrix));
	glUniform3fv(textColorUniform, 1, glm::value_ptr(textData->textColor));

	glUniform1f(alphaUniform, alphaValue);
	//glUniform1fv()

	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	drawLine(textData);
	glBindVertexArray(0);
	glUseProgram(0);
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::drawLine
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::drawLine(TextData *textData)
{
	for (GLsizeiptr counter = 0; counter < textData->textSize; ++counter)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * textData->vertices[counter].size(), textData->vertices[counter].data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vboTexture);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		TextCharacter *nextChar = characters[textData->text[counter]];
		glBindTexture(GL_TEXTURE_2D, nextChar->textureId);
		glUniform1i(textureSamplerUniform, 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);
	}
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::cleanUp
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::cleanUp(void)
{
	std::map<GLchar, TextCharacter*>::iterator  characterIterator;

	for (characterIterator = characters.begin(); characterIterator != characters.end(); ++characterIterator)
	{
		TextCharacter *nextChar = characterIterator->second;
		glDeleteBuffers(1, &(nextChar->textureId));
		free(nextChar);

		characterIterator->second = NULL;
	}

	characters.clear();

	FT_Done_Face(fontFace);
	FT_Done_FreeType(freeType);

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vboPosition)
	{
		glDeleteBuffers(1, &vboPosition);
		vboPosition = 0;
	}

	if (vboTexture)
	{
		glDeleteBuffers(1, &vboTexture);
		vboTexture = 0;
	}

	if (shaderProgramObject)
	{
		if (vertexShaderObject)
		{
			glDetachShader(shaderProgramObject, vertexShaderObject);
		}

		if (fragmentShaderObject)
		{
			glDetachShader(shaderProgramObject, fragmentShaderObject);
		}
	}

	if (vertexShaderObject)
	{
		glDeleteShader(vertexShaderObject);
		vertexShaderObject = 0;
	}

	if (fragmentShaderObject)
	{
		glDeleteShader(fragmentShaderObject);
		fragmentShaderObject = 0;
	}

	if (shaderProgramObject)
	{
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}

	glUseProgram(0);

	if (pCustFontFile)
	{
		fprintf_s(pCustFontFile, "Closing CustFontRenderLog File \n");
		fclose(pCustFontFile);
		pCustFontFile = NULL;
	}
		
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::fadeInText
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::fadeInText()
{
	fadeInFlag = true;
	fadeOutFlag = false;
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::fadeOutText
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::fadeOutText()
{
	fadeInFlag = false;
	fadeOutFlag = true;
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::update
//-------------------------------------------------------------------------------------------------------------
void CCustomFontRenderer::update()
{
	if (fadeOutFlag)
	{
		alphaValue = alphaValue - 0.000000000000000002f;
		if (alphaValue >= 0.0f)
		{
			alphaValue = 0.0f;
		}

	}
	else if (fadeInFlag)
	{
		alphaValue = alphaValue + 0.0000000000000000002f;
		if (alphaValue <= 1.0f)
		{
			alphaValue = 1.0f;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------
// CCustomFontRenderer::~CCustomFontRenderer
//-------------------------------------------------------------------------------------------------------------
CCustomFontRenderer::~CCustomFontRenderer()
{
	cleanUp();
}
