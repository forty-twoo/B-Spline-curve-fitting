#ifndef CURVE_HPP_
#define CURVE_HPP_

#include "tinynurbs.h"
#include <vector>

class Curve {
private:
	//unsigned int degree;
	//std::vector<double> knots;
	//std::vector<glm::vec<3, double>> control_points;
	tinynurbs::Curve<double> curve;
	std::vector<glm::dvec3> curvePts;

public:
	void SetControlPoints(std::vector<glm::vec<3, double>>& control_points_) {
		curve.control_points = control_points_;
	}
	void Setknots(std::vector<double>& knots_) {
		curve.knots = knots_;
	}
	void SetDegree(unsigned int degree_) {

		curve.degree = degree_;
	}
	std::vector<glm::dvec3>& GetControlPoints() {
		return curve.control_points;
	}
	std::vector<glm::dvec3>& ConstructCurve(const unsigned int degree_, const std::vector<double>& knots_, const std::vector<glm::dvec3>& controlPts);
	unsigned int ControlPtsSize() const {
		return curve.control_points.size();
	}
	const int GetDegree() const {
		return curve.degree;
	}
	void clear() {
		curve.knots.clear();
		curve.control_points.clear();
		curvePts.clear();
	}
};


#endif // !CURVE_HPP_
