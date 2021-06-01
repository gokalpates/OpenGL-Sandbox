#include "Shader.h"

Shader::Shader(std::filesystem::path vertexShaderPath, std::filesystem::path fragmentShaderPath) :
	m_shaderId(0u)
{
	std::ifstream fileStream(vertexShaderPath);
	if (!fileStream.is_open())
	{
		std::cout << "ERROR: Program could not open: " << vertexShaderPath << "\n";
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}
	std::stringstream vertexStream;
	vertexStream << fileStream.rdbuf();
	std::string vertexShaderSource = vertexStream.str();
	const char* vertexSource = vertexShaderSource.c_str();
	fileStream.close();

	fileStream.open(fragmentShaderPath);
	if (!fileStream.is_open())
	{
		std::cout << "ERROR: Program could not open: " << fragmentShaderPath << "\n";
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}
	std::stringstream fragmentStream;
	fragmentStream << fileStream.rdbuf();
	std::string fragmentShaderSource = fragmentStream.str();
	const char* framentSource = fragmentShaderSource.c_str();
	fileStream.close();

	unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexSource, nullptr);
	glCompileShader(vertexShaderId);
	int success = 0;
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShaderId, 512, nullptr, infoLog);
		std::cout << "ERROR: OpenGL could not compile vertex shader: " << infoLog << "\n";
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &framentSource, nullptr);
	glCompileShader(fragmentShaderId);
	success = 0;
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShaderId, 512, nullptr, infoLog);
		std::cout << "ERROR: OpenGL could not compile fragment shader: " << infoLog << "\n";
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	m_shaderId = glCreateProgram();
	glAttachShader(m_shaderId, vertexShaderId);
	glAttachShader(m_shaderId, fragmentShaderId);
	glLinkProgram(m_shaderId);
	success = 0;
	glGetProgramiv(m_shaderId, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(m_shaderId, 512, nullptr, infoLog);
		std::cout << "ERROR: OpenGL could not link vertex and fragment shaders: " << infoLog << "\n";
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
}

Shader::~Shader()
{
	glDeleteProgram(m_shaderId);
}

void Shader::use() const
{
	glUseProgram(m_shaderId);
}

void Shader::disuse() const
{
	glUseProgram(0u);
}

void Shader::setBool(const char* name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_shaderId, name), (int)value);
}

void Shader::setFloat(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(m_shaderId, name), value);
}

void Shader::setInt(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(m_shaderId, name), value);
}

void Shader::setMat4(const char* name, glm::mat4& matrices) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_shaderId, name), 1, GL_FALSE, glm::value_ptr(matrices));
}

void Shader::setVec3(const char* name, glm::vec3& vector) const
{
	glUniform3f(glGetUniformLocation(m_shaderId, name), vector.x, vector.y, vector.z);
}

void Shader::setVec3(const char* name, float& x, float& y, float& z) const
{
	glUniform3f(glGetUniformLocation(m_shaderId, name), x, y, z);
}

void Shader::setVec3(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(m_shaderId, name), x, y, z);
}
