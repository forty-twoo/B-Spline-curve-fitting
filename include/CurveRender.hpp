#ifndef CURVERENDER_HPP_
#define CURVERENDER_HPP_

#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "Shader.hpp"

// Include GLFW
#include <GLFW/glfw3.h>

class CurveRender {
private:
	unsigned int m_VAOcurve, m_VAOconstruct, m_VBOcurve, m_VBOconstruct;
	std::vector<glm::dvec3> m_vertices;
	std::vector<glm::dvec3> m_controlPoints;

public:
	CurveRender() :m_VAOcurve(0), m_VAOconstruct(0), m_VBOcurve(0), m_VBOconstruct(0) {}

	void Init();

	void SetUpVBO(std::vector<glm::dvec3>& cPts, std::vector<glm::dvec3>& vertices);

	void Draw(const Shader& shaderCurve, const Shader& shaderCPts, bool hideConstruction);

	void CleanUp();

	void Clear();
};

#endif // !CURVERENDER_HPP_
