#include "CurveRender.hpp"

void CurveRender::Init() {
	glGenVertexArrays(1, &m_VAOcurve);
	glGenBuffers(1, &m_VBOcurve);

	glGenVertexArrays(1, &m_VAOconstruct);
	glGenBuffers(1, &m_VBOconstruct);
}

void CurveRender::SetUpVBO(std::vector<glm::dvec3>& cPts, std::vector<glm::dvec3>& vertices) {
	// draw curve
	m_vertices = vertices;
	glBindVertexArray(m_VAOcurve);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOcurve);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::dvec3) * m_vertices.size(), m_vertices.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);

	m_controlPoints = cPts;
	glBindVertexArray(m_VAOconstruct);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOconstruct);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::dvec3) * m_controlPoints.size(), m_controlPoints.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
}

void CurveRender::Draw(const Shader& shaderCurve, const Shader& shaderCPts, bool hideConstruction) {
	glLineWidth(8.0);
	shaderCurve.use();
	glBindVertexArray(m_VAOcurve);
	glDrawArrays(GL_LINE_STRIP, 0, m_vertices.size());

	if(!hideConstruction) {
		shaderCPts.use();
		glPointSize(10.0);
		glBindVertexArray(m_VAOconstruct);
		glDrawArrays(GL_POINTS, 0, m_controlPoints.size());
		glLineWidth(2.0);
		glDrawArrays(GL_LINE_STRIP, 0, m_controlPoints.size());
	}

}

void CurveRender::CleanUp() {
	glDeleteVertexArrays(1, &m_VAOconstruct);
	glDeleteVertexArrays(1, &m_VAOcurve);
	glDeleteBuffers(1, &m_VBOconstruct);
	glDeleteBuffers(1, &m_VBOcurve);

}

void CurveRender::Clear() {
	m_vertices.clear();
	m_controlPoints.clear();
}