#ifndef INTERPOLATION_HPP_
#define INTERPOLATION_HPP_


void GlobalInterp(std::vector<glm::dvec3> points, int degree, std::vector<double>& knots,
	std::vector<double>& param, std::vector<glm::dvec3>& controlPts);

void UniformInterp(std::vector<glm::dvec3> points, int degree,
	std::vector<double>& knots, std::vector<double>& param);


void ChorldInterp(std::vector<glm::dvec3> points, int degree,
	std::vector<double>& knots, std::vector<double>& param);


void UniversalInterp(std::vector<glm::dvec3> points, int degree,
	std::vector<double>& knots, std::vector<double>& param);


void CentriperalInterp(std::vector<glm::dvec3> points, int degree,
	std::vector<double>& knots, std::vector<double>& param);

#endif // !INTERPOLATION_HPP_
