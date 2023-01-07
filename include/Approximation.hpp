#ifndef APPROXIMATION_HPP_
#define APPROXIMATION_HPP_

#include "Curve.hpp"
#include "Interpolation.hpp"

void Approximation(int Choice, std::vector<glm::dvec3> points, int degree, std::vector<double>& knots,
	std::vector<double>& param, int h, std::vector<glm::dvec3>& controlPts);

void UniformApprox(std::vector<glm::dvec3> points, int conptsSize, int degree,
	std::vector<double>& knots, std::vector<double>& param);


void ChorldApprox(std::vector<glm::dvec3> points, int conptsSize, int degree,
	std::vector<double>& knots, std::vector<double>& param);


void UniversalApprox(std::vector<glm::dvec3> points, int conptsSize, int degree,
	std::vector<double>& knots, std::vector<double>& param);


void CentripetalApprox(std::vector<glm::dvec3> points, int conptsSize, int degree,
	std::vector<double>& knots, std::vector<double>& param);

#endif // !APPROXIMATION_HPP_
