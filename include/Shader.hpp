#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	unsigned int ID;
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------

	Shader() {
		ID = 0;
	}

	//Shader(const char* vertexPath, const char* fragmentPath, const char * geometryPath = nullptr, const char * tessControlPath = nullptr, const char * tessEvalPath = nullptr);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr, const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr) {
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string tessControlCode;
		std::string tessEvalCode;
		std::string geometryCode;
		std::string fragmentCode;

		std::ifstream vShaderFile;
		std::ifstream tcShaderFile;
		std::ifstream teShaderFile;
		std::ifstream gShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			// open files
			vShaderFile.open(vertexPath);
			if(tessControlPath != nullptr) tcShaderFile.open(tessControlPath);
			if(tessEvalPath != nullptr) teShaderFile.open(tessEvalPath);
			if(geometryPath != nullptr) gShaderFile.open(geometryPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, tcShaderStream, teShaderStream, gShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			if(tessControlPath != nullptr) tcShaderStream << tcShaderFile.rdbuf();
			if(tessEvalPath != nullptr) teShaderStream << teShaderFile.rdbuf();
			if(geometryPath != nullptr) gShaderStream << gShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			if(tessControlPath != nullptr) tcShaderFile.close();
			if(tessEvalPath != nullptr) teShaderFile.close();
			if(geometryPath != nullptr) gShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			if(tessControlPath != nullptr) tessControlCode = tcShaderStream.str();
			if(tessEvalPath != nullptr) tessEvalCode = teShaderStream.str();
			if(geometryPath != nullptr) geometryCode = gShaderStream.str();
			fragmentCode = fShaderStream.str();
		} catch(std::ifstream::failure& e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* gShaderCode, * tcShaderCode, * teShaderCode;
		if(tessControlPath != nullptr) tcShaderCode = tessControlCode.c_str();
		if(tessEvalPath != nullptr) teShaderCode = tessEvalCode.c_str();
		if(geometryPath != nullptr) gShaderCode = geometryCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, tessC, tessE, geometry, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// tesselation control
		if(tessControlPath != nullptr) {
			tessC = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(tessC, 1, &tcShaderCode, nullptr);
			glCompileShader(tessC);
			checkCompileErrors(tessC, "TESS_CONTROL");
		}
		// tesselation evaluation
		if(tessEvalPath != nullptr) {
			tessE = glCreateShader(GL_TESS_EVALUATION_SHADER);
			glShaderSource(tessE, 1, &teShaderCode, nullptr);
			glCompileShader(tessE);
			checkCompileErrors(tessE, "TESS_EVALUATION");
		}
		// geometry shader
		if(geometryPath != nullptr) {
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, nullptr);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		if(tessControlPath != nullptr) glAttachShader(ID, tessC);
		if(tessEvalPath != nullptr) glAttachShader(ID, tessE);
		if(geometryPath != nullptr) glAttachShader(ID, geometry);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		if(tessControlPath != nullptr) glDeleteShader(tessC);
		if(tessEvalPath != nullptr) glDeleteShader(tessE);
		if(geometryPath != nullptr) glDeleteShader(geometry);
		glDeleteShader(fragment);
	}

	~Shader() {
		glDeleteProgram(ID);
	}

	static void unused() {
		glUseProgram(0);
	}

	// activate the shader
	// ------------------------------------------------------------------------
	void use() const {
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const {
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w) const {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat) const {
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}



private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	static void checkCompileErrors(GLuint shader, const std::string& type) {
		GLint success;
		GLchar infoLog[1024];
		if(type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if(!success) {
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		} else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if(!success) {
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};
#endif
