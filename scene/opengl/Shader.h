#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
std::string src_from_file(const std::string& file) {
	std::fstream stream(file);
	std::string res = "";
	std::string c;
	for (int i = 0; i < 3; i++) {
		stream >> c;
		res += c + ' ';
	}
	res += '\n';
	while (stream >> c) {
		res += c + ' ';
	}
	return res;
}

class Shader
{
public:
	GLuint vertexShader, fragmentShader, geometryShader, shaderProgram;
	Shader(const std::string& vertexFile, const std::string& fragmentFile, const std::string& geometryFile = "") {
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		std::string vsh_src = src_from_file(vertexFile);
		std::cout << vsh_src << std::endl;
		const char* src = vsh_src.c_str();
		glShaderSource(vertexShader, 1, &src, NULL);
		glCompileShader(vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		std::string fsh_src = src_from_file(fragmentFile);
		const char* frag_src = fsh_src.c_str();
		glShaderSource(fragmentShader, 1, &frag_src, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		if (geometryFile != "") {
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			std::string gsh_src = src_from_file(geometryFile);
			const char* geom_src = gsh_src.c_str();

			glShaderSource(geometryShader, 1, &geom_src, NULL);
			glCompileShader(geometryShader);
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
		}

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		if (geometryFile != "") {
			glAttachShader(shaderProgram, geometryShader);
		}
		glLinkProgram(shaderProgram);
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if (geometryFile != "") {
			glDeleteShader(geometryShader);
		}

	}
	void use() {
		glUseProgram(shaderProgram);
	}
	GLuint get_location(const GLchar* name) {
		return glGetUniformLocation(shaderProgram, name);
	}
};