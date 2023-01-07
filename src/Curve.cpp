#include "Curve.hpp"

void Curve::ConstructCurve(const unsigned int degree_, const std::vector<double>& knots_, const std::vector<glm::dvec3>& controlPts) {
	curve.control_points = controlPts;
	curve.degree = degree_;
	curve.knots = knots_;
	curvePts.clear();

	double t = *(knots_.end() - 1);

	curvePts.clear();
	for(double i = 0; i < t; i += t / 5000.f) {
		auto it = tinynurbs::curvePoint(curve, i);
		curvePts.push_back(it);
	}
}
