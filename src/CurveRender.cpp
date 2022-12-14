#include "CurveRender.hpp"

void PointsRender::Init() {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
}

void PointsRender::SetUpVBO(std::vector<glm::dvec3>& points) {
	m_points = points;
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::dvec3) * m_points.size(), m_points.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(glm::dvec3), (void*)nullptr);

	glEnableVertexAttribArray(0);
}

void PointsRender::Draw(const Shader& shaderPoints) {
	shaderPoints.use();
	glPointSize(10.0);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, m_points.size());
}

void PointsRender::CleanUp() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void PointsRender::Clear() {
	m_points.clear();
}

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
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(glm::dvec3), (void*)nullptr);

	glEnableVertexAttribArray(0);

	m_controlPoints = cPts;
	glBindVertexArray(m_VAOconstruct);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOconstruct);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::dvec3) * m_controlPoints.size(), m_controlPoints.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, sizeof(glm::dvec3), (void*)nullptr);
	glEnableVertexAttribArray(0);
}

void CurveRender::Draw(const Shader& shaderCurve, const Shader& shaderCPts, glm::vec3 curveColor, bool hideConstruction) {
	glLineWidth(3.0);
	shaderCurve.use();
	shaderCurve.setVec3("curveColor", curveColor);
	shaderCPts.use();
	shaderCPts.setVec3("curveColor", curveColor);
	glBindVertexArray(m_VAOcurve);
	glDrawArrays(GL_LINE_STRIP, 0, m_vertices.size());

	if(!hideConstruction) {
		shaderCPts.use();
		glPointSize(10.0);
		glBindVertexArray(m_VAOconstruct);
		glDrawArrays(GL_POINTS, 0, m_controlPoints.size());
		glLineWidth(0.5);
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