#include "Skydome.h"

CSkyDome::CSkyDome()
{
	// values set according to gpu gems

	Samples = 4; // number of sample rays to use in integral equation
	Kr = 0.0030f; // Rayleigh scattering constant
	Km = 0.0015f; // Mie scattering constant
	ESun = 16.0f; // Sun brightness constant
	g = -0.98f; // The Mie phase asymmetry factor

	Exposure = 1.0f;

	/*InnerRadius = 10.0f;
	OuterRadius = 10.25f;*/

	/*InnerRadius = 10.0f * 1000000.0f;
	OuterRadius = 10.25f * 1000000.0f;*/


	InnerRadius = 10.0f * 100000.0f;
	OuterRadius = 10.25f * 100000.0f;

	Scale = 1.0f / (OuterRadius - InnerRadius);
	//ScaleDepth = 0.75f;
	ScaleDepth = 0.25f;
	ScaleOverScaleDepth = Scale / ScaleDepth;

	

	CameraPos = glm::vec3(0.0f, InnerRadius, 0.0f);

	WavelengthRed = 0.650f; // 650 nm for red
	WavelengthGreen = 0.570f; // 570 nm for green
	WavelengthBlue = 0.475f; // 475 nm for blue

	WavelengthRed4 = glm::pow(WavelengthRed, 4.0f);
	WavelengthGreen4 = glm::pow(WavelengthGreen, 4.0f);
	WavelengthBlue4 = glm::pow(WavelengthBlue, 4.0f);

	InvWaveLength = glm::vec3(1.0f / WavelengthRed4, 1.0f / WavelengthGreen4, 1.0f / WavelengthBlue4);

	SunRadius = 5.0f;
	//SunRotVec = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));
	SunRotVec = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
	//SunRotVec = glm::normalize(glm::vec3(-0.6f, -1.0f, -1.0f));
	SunCPos = glm::rotate(glm::vec3(-1200.0f, 0.0f, 0.0f), glm::radians(-10.5f), SunRotVec);
	//SunCPos = glm::rotate(glm::vec3(-600.0f, 0.0f, 0.0f), glm::radians(-10.5f), SunRotVec);
	//SunCPos = glm::rotate(glm::vec3(-500.0f, 0.0f, 0.0f), glm::radians(-22.5f), SunRotVec);

	SunCDir = glm::vec3(0.0f);
	SunWPos = glm::vec3(0.0f);

	RefractionFactor = 0.0f;
	TransitionSpeed = 0.1f;

	LightColor = glm::vec3(0.0f);
	AmbientIntensity = 0.0f;
	DiffuseIntensity = 0.0f;

	alpha = 1.0f;
	IncreaseAlpha = GL_FALSE;
	DecreaseAlpha = GL_FALSE;

	IsSunriseComplete = GL_FALSE;
	IsSunsetComplete = GL_FALSE;

	m_bItNight = false;
	m_bItDay = true;

	// opening position file
	if (fopen_s(&pSkyFile, "skyfile.txt", "w") != 0)
	{
		//MessageBox(NULL, TEXT("Failed to create the skyfile.txt. Exiting now."), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(pSkyFile, "skyfile.txt created successfully.\n");
	}

	fprintf_s(pSkyFile, "SunCPOS: %s\n", glm::to_string(SunCPos).c_str());
}

CSkyDome::~CSkyDome()
{
	fclose(pSkyFile);
	if (FrameBufferObjectSky)
	{
		glDeleteFramebuffers(1, &FrameBufferObjectSky);
		FrameBufferObjectSky = 0;
	}

	if (VBOSkyDomePosition)
	{
		glDeleteBuffers(1, &VBOSkyDomePosition);
		VBOSkyDomePosition = 0;
	}

	if (VAOSkyDome)
	{
		glDeleteVertexArrays(1, &VAOSkyDome);
		VAOSkyDome = 0;
	}

	if (ShaderProgramObjectSky)
	{
		glDeleteProgram(ShaderProgramObjectSky);
		ShaderProgramObjectSky = 0;
	}
}

void CSkyDome::SetShaderProgramObject(GLuint SPO)
{
	this->ShaderProgramObjectSky = SPO;
}

GLuint CSkyDome::GetShaderProgramObject(void)
{
	return(this->ShaderProgramObjectSky);
}

void CSkyDome::InitializeSky(GLuint ShaderObject)
{
	this->ShaderProgramObjectSky = ShaderObject;

	glUseProgram(ShaderProgramObjectSky);
	glUniform3fv(glGetUniformLocation(ShaderProgramObjectSky, "v3CameraPos"), 1, glm::value_ptr(CameraPos));
	glUniform3fv(glGetUniformLocation(ShaderProgramObjectSky, "v3InvWaveLength"), 1, glm::value_ptr(InvWaveLength));
	//glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fCameraHeight"), InnerRadius);
	//glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fCameraHeight2"), InnerRadius * InnerRadius);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fCameraHeight"), glm::length(CameraPos));
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fCameraHeight2"), glm::length2(CameraPos));
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fInnerRadius"), InnerRadius);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fInnerRadius2"), InnerRadius * InnerRadius);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fOuterRadius"), OuterRadius);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fOuterRadius2"), OuterRadius * OuterRadius);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fKrESun"), Kr * ESun);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fKmESun"), Km * ESun);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fKr4PI"), Kr * 4.0f * glm::pi<float>());
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fKm4PI"), Km * 4.0f * glm::pi<float>());
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fScale"), Scale);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fScaleDepth"), ScaleDepth);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "fScaleOverScaleDepth"), ScaleOverScaleDepth);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "g"), g);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "g2"), g * g);
	glUniform1i(glGetUniformLocation(ShaderProgramObjectSky, "Samples"), Samples);
	glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "alpha"), alpha);
	glUseProgram(0);

	ComputeSkyDome();
}

void CSkyDome::ComputeSkyDome(void)
{
	glm::vec3 *SkyDomeVertices = new glm::vec3[112 * 3], va, vb, vc, vd;

	float stepa = glm::pi<float>() * 2.0f / 16;
	//float startb = asin(InnerRadius / OuterRadius);
	float startb = asin(0.9f);
	float stepb = (glm::half_pi<float>() - startb) / 4;
	int pos = 0;

	fprintf_s(pSkyFile, "stepa = %f\nstartb = %f\nstepb = %f\npos = %d\n", stepa, startb, stepb, pos);

	for (int y = 0; y < 3; y++)
	{
		float b = startb + stepb * y;

		fprintf_s(pSkyFile, "b = %f\n", b);

		for (int x = 0; x < 16; x++)
		{
			float a = stepa * x;
			fprintf_s(pSkyFile, "a = %f\n", a);

			va = glm::vec3(sin(a) * cos(b) * OuterRadius, sin(b) * OuterRadius, -cos(a) * cos(b) * OuterRadius);
			vb = glm::vec3(sin(a + stepa) * cos(b) * OuterRadius, sin(b) * OuterRadius, -cos(a + stepa) * cos(b) * OuterRadius);
			vc = glm::vec3(sin(a + stepa) * cos(b + stepb) * OuterRadius, sin(b + stepb) * OuterRadius, -cos(a + stepa) * cos(b + stepb) * OuterRadius);
			vd = glm::vec3(sin(a) * cos(b + stepb) * OuterRadius, sin(b + stepb) * OuterRadius, -cos(a) * cos(b + stepb) * OuterRadius);

			fprintf_s(pSkyFile, "va = %s\nvb = %s\nvc = %s\nvd = %s\n", glm::to_string(va).c_str(), glm::to_string(vb).c_str(), glm::to_string(vc).c_str(), glm::to_string(vd).c_str());

			SkyDomeVertices[pos + 0] = va;
			SkyDomeVertices[pos + 1] = vb;
			SkyDomeVertices[pos + 2] = vc;

			fprintf_s(pSkyFile, "for pos: %d value: %s\tpos: %d value: %s\tpos: %d value: %s\n", pos + 0, glm::to_string(va).c_str(), pos + 1, glm::to_string(vb).c_str(), pos + 2, glm::to_string(vc).c_str());

			pos += 3;

			SkyDomeVertices[pos + 0] = vc;
			SkyDomeVertices[pos + 1] = vd;
			SkyDomeVertices[pos + 2] = va;

			fprintf_s(pSkyFile, "for pos: %d value: %s\tpos: %d value: %s\tpos: %d value: %s\n", pos + 0, glm::to_string(va).c_str(), pos + 1, glm::to_string(vb).c_str(), pos + 2, glm::to_string(vc).c_str());

			pos += 3;
		}
	}

	float b = startb + stepb * 3;
	fprintf_s(pSkyFile, "b = %f\n\n\n", b);

	for (int x = 0; x < 16; x++)
	{
		float a = stepa * x;
		fprintf_s(pSkyFile, "a = %f\n", a);

		va = glm::vec3(sin(a) * cos(b) * OuterRadius, sin(b) * OuterRadius, -cos(a) * cos(b) * OuterRadius);
		vb = glm::vec3(sin(a + stepa) * cos(b) * OuterRadius, sin(b) * OuterRadius, -cos(a + stepa) * cos(b) * OuterRadius);
		vc = glm::vec3(0, OuterRadius, 0);

		fprintf_s(pSkyFile, "va = %s\nvb = %s\nvc = %s\nvd = %s\n", glm::to_string(va).c_str(), glm::to_string(vb).c_str(), glm::to_string(vc).c_str(), glm::to_string(vd).c_str());

		SkyDomeVertices[pos + 0] = va;
		SkyDomeVertices[pos + 1] = vb;
		SkyDomeVertices[pos + 2] = vc;

		fprintf_s(pSkyFile, "for pos: %d value: %s\tpos: %d value: %s\tpos: %d value: %s\n", pos + 0, glm::to_string(va).c_str(), pos + 1, glm::to_string(vb).c_str(), pos + 2, glm::to_string(vc).c_str());

		pos += 3;
	}

	glGenVertexArrays(1, &VAOSkyDome);
	glBindVertexArray(VAOSkyDome);

	//Vbo_Model_Vertex
	glGenBuffers(1, &VBOSkyDomePosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBOSkyDomePosition);
	glBufferData(GL_ARRAY_BUFFER, 112 * 3 * 3 * sizeof(float), SkyDomeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete[] SkyDomeVertices;
	fclose(pSkyFile);

	fopen_s(&pSkyFile, "skyfile.txt", "a+");
}

void CSkyDome::RenderSky(glm::vec3 CamPos, glm::mat4 MVP, GLboolean Value)
{
	fprintf_s(pSkyFile, "CamPos: %s\n", glm::to_string(CamPos).c_str());
	fprintf_s(pSkyFile, "MVP: %s\n", glm::to_string(MVP).c_str());
	this->CamPos = CamPos;
	if (Value == GL_TRUE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(ShaderProgramObjectSky);
	
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgramObjectSky, "u_mvp_matrix"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniform3fv(glGetUniformLocation(ShaderProgramObjectSky, "v3LightPos"), 1, glm::value_ptr(SunCDir));
	
	glBindVertexArray(VAOSkyDome);
	glDrawArrays(GL_TRIANGLES, 0, 112 * 3);
	glBindVertexArray(0);
	
	glUseProgram(0);
	if (Value == GL_TRUE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CSkyDome::UpdateSky(GLfloat fDelta)
{

	
		SunCDir = glm::normalize(SunCPos);
		SunWPos = CamPos + SunCPos;
		RefractionFactor = (1.0f - glm::sqrt(glm::max(0.0f, SunCDir.y)));
		SunColor = 1.0f - glm::vec3(0.0f, 0.5f, 1.0f) * RefractionFactor;

		LightColor = 1.0f - glm::vec3(0.0f, 0.25f, 0.5f) * RefractionFactor;
		AmbientIntensity = 0.0625f + 0.1875f * glm::min(1.0f, glm::max(0.0f, (0.375f + SunCDir.y) / 0.25f));
		DiffuseIntensity = 0.75f * glm::min(1.0f, glm::max(0.0f, (0.03125f + SunCDir.y) / 0.0625f));

		fprintf_s(pSkyFile, "CamPos: %s\n", glm::to_string(CamPos).c_str());

		SunCPos = glm::rotate(SunCPos, glm::radians(0.05f), SunRotVec);
		fprintf_s(pSkyFile, "SunCPOS: %s\n", glm::to_string(SunCPos).c_str());

		if (SunCPos.y >= 100.0f && SunCPos.y <= 110.0f)
		{
			DecreaseAlpha = GL_TRUE;
			alpha = 1.0;
		}


		if (SunCPos.y <= -50.0f && DecreaseAlpha == GL_TRUE && SunCPos.x >= 0.0f)
		{
			alpha -= 0.01f;
			if (alpha <= 0.0f)
			{
				alpha = 0.0f;
				DecreaseAlpha = GL_FALSE;
				IncreaseAlpha = GL_TRUE;
			}
		}

		if (SunCPos.y >= -100.0f && IncreaseAlpha == GL_TRUE && SunCPos.x <= 0.0f)
		{
			alpha += 0.01f;
			if (alpha >= 1.0f)
			{
				alpha = 1.0f;
				DecreaseAlpha = GL_TRUE;
				IncreaseAlpha = GL_FALSE;
			}
		}


		if (SunCPos.y >= 0.0f)
		{
			m_bItDay = true;
			m_bItNight = false;
		}
		else
		{
			m_bItDay = false;
			m_bItNight = true;
		}

		/*if (m_bItDay)
		{
			fprintf_s(pSkyFile, "bItsDay = true\n");
		}
		else
		{
			fprintf_s(pSkyFile, "bItsDay = false\n");
		}

		if (m_bItDay)
		{
			fprintf_s(pSkyFile, "bItsNight = true\n");
		}
		else
		{
			fprintf_s(pSkyFile, "bItsNight = false\n");
		}*/

		glUseProgram(ShaderProgramObjectSky);
		glUniform1f(glGetUniformLocation(ShaderProgramObjectSky, "alpha"), alpha);
		glUseProgram(0);
	
}

GLfloat CSkyDome::GetRefractionFactor(void)
{
	return(this->RefractionFactor);
}

GLfloat CSkyDome::GetSunCDirY(void)
{
	return(this->SunCDir.y);
}

glm::vec3 CSkyDome::GetSunRotVec(void)
{
	return(this->SunRotVec);
}

glm::vec3 CSkyDome::GetSunCPos(void)
{
	return(this->SunCPos);
}

glm::vec3 CSkyDome::GetSunCDir(void)
{
	return(this->SunCDir);
}

glm::vec3 CSkyDome::GetSunColor(void)
{
	return(this->SunColor);
}

GLfloat CSkyDome::GetInnerRadius(void)
{
	return(this->InnerRadius);
}

GLfloat CSkyDome::GetSunRadius(void)
{
	return(this->SunRadius);
}

glm::vec3 CSkyDome::GetLightColor(void)
{
	return(this->LightColor);
}

glm::vec3 CSkyDome::GetSunWPos(void)
{
	return(this->SunWPos);
}

GLfloat CSkyDome::GetAmbientIntensity(void)
{
	return(this->AmbientIntensity);
}

GLfloat CSkyDome::GetDiffuseIntensity(void)
{
	return(this->DiffuseIntensity);
}

void CSkyDome::SetSunCDirY(GLfloat SunCDirY)
{
	this->SunCDir.y = SunCDirY;
}

void CSkyDome::SetSunWPos(glm::vec3 SunWPos)
{
	this->SunWPos = SunWPos;
}

void CSkyDome::SetSunCPos(glm::vec3 SunCPos)
{
	this->SunCPos = SunCPos;
}

void CSkyDome::SetSunCDir(glm::vec3 SunCDir)
{
	this->SunCDir = SunCDir;
}

bool CSkyDome::IsDay()
{
	return this->m_bItDay;
}

bool CSkyDome::IsNight()
{
	return this->m_bItNight;
}
