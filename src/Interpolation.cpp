#include "Curve.hpp"
#include "Interpolation.hpp"
#include <Eigen/Dense>

//Solve linear equations to find the control points for the fitting curve 
void GlobalInterp(std::vector<glm::dvec3> points, int degree, std::vector<double>& knots,
	std::vector<double>& param, std::vector<glm::dvec3>& controlPts) {
	int n = points.size();
	controlPts.resize(n);

	Eigen::MatrixXd matN(n, n);
	Eigen::MatrixXd D_0(n, 1);
	Eigen::MatrixXd D_1(n, 1);
	Eigen::MatrixXd P_0(n, 1);
	Eigen::MatrixXd P_1(n, 1);
	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++) {
			matN(j, i) = tinynurbs::bsplineOneBasis(i, degree, knots, param[j]);
		}
		D_0(i, 0) = points[i].x;
		D_1(i, 0) = points[i].y;
	}
	P_0 = matN.colPivHouseholderQr().solve(D_0);
	P_1 = matN.colPivHouseholderQr().solve(D_1);
	for(int i = 0; i < n; i++) {
		controlPts[i].x = P_0(i, 0);
		controlPts[i].y = P_1(i, 0);
		controlPts[i].z = 0;
	}

}

//Use the uniform method to set the parameters and the knot vectors
void UniformInterp(std::vector<glm::dvec3> points, int degree,
	std::vector<double>& knots, std::vector<double>& param) {

	int n = points.size();
	int m = n + degree + 1;

	knots.resize(m);
	param.resize(n);

	for(int i = 0; i < n; i++) {
		param[i] = (double)i / (double)(n - 1);
	}
	double num = m - 2 * degree - 2;
	double delta = (double)1.0 / num;

	for(int i = 0; i <= degree; i++)knots[i] = 0;
	for(int i = degree + 1; i < degree + num + 1; i++)knots[i] = double(i - degree) / double(num + 1);
	for(int i = degree + num + 1; i < m; i++)knots[i] = 1;
}

void ChorldInterp(std::vector<glm::dvec3> points, int degree,
	std::vector<double>& knots, std::vector<double>& param) {

	int n = points.size();
	int m = n + degree + 1;
	knots.resize(m);
	param.resize(n);

	std::vector<double> chorldLen;
	double sum = 0;
	for(int i = 1; i < n; i++) {
		double len = std::sqrt((points[i].x - points[i - 1].x) * (points[i].x - points[i - 1].x) + (points[i].y - points[i - 1].y) * (points[i].y - points[i - 1]).y);
		sum += len;
		chorldLen.push_back(sum);
	}
	param[0] = 0, param[n - 1] = 1;
	for(int i = 1; i < n - 1; i++) {
		param[i] = chorldLen[i - 1] / sum;
	}

	double num = m - 2 * degree - 2;
	for(int i = 0; i <= degree; i++)knots[i] = 0;
	for(int i = degree + 1; i < degree + num + 1; i++) {
		double sum = 0;
		for(int k = i - degree; k <= i - 1; k++) {
			sum += param[k];
		}
		knots[i] = sum / (double)degree;
	}
	for(int i = degree + num + 1; i < m; i++)knots[i] = 1;

}

void CentriperalInterp(std::vector<glm::dvec3> points, int degree,
	std::vector<double>& knots, std::vector<double>& param) {

	int n = points.size();
	int m = n + degree + 1;
	knots.resize(m);
	param.resize(n);

	std::vector<double> chorldLen;
	double sum = 0;
	for(int i = 1; i < n; i++) {
		double len = std::sqrt((points[i].x - points[i - 1].x) * (points[i].x - points[i - 1].x) + (points[i].y - points[i - 1].y) * (points[i].y - points[i - 1]).y);
		len = std::sqrt(len);
		sum += len;
		chorldLen.push_back(sum);
	}
	param[0] = 0, param[n - 1] = 1;
	for(int i = 1; i < n - 1; i++) {
		param[i] = chorldLen[i - 1] / sum;
	}

	double num = m - 2 * degree - 2;
	for(int i = 0; i <= degree; i++)knots[i] = 0;
	for(int i = degree + 1; i < degree + num + 1; i++) {
		double sum = 0;
		for(int k = i - degree; k <= i - 1; k++) {
			sum += param[k];
		}
		knots[i] = sum / (double)degree;
	}
	for(int i = degree + num + 1; i < m; i++)knots[i] = 1;

}

//Use the universal method to set the parameters and the knot vectors
void UniversalInterp(std::vector<glm::dvec3> points, int degree,
	std::vector<double>& knots, std::vector<double>& param) {
	int n = points.size();
	int m = n + degree + 1;

	knots.resize(m);
	param.resize(n);

	double num = m - 2 * degree - 2;
	double delta = (double)1.0 / num;

	for(int i = 0; i <= degree; i++)knots[i] = 0;
	for(int i = degree + 1; i < degree + num + 1; i++)knots[i] = double(i - degree) / double(num + 1);
	for(int i = degree + num + 1; i < m; i++)knots[i] = 1;

	for(int i = 0; i < n; i++) {
		double tMax = 0, valMax = -1;
		for(double t = 0; t <= 1; t += 1 / 1000.0) {
			int span = tinynurbs::findSpan(degree, knots, t);
			double v = tinynurbs::bsplineOneBasis(i, degree, knots, t);
			if(v > valMax) {
				valMax = v; tMax = t;
			}
		}
		param[i] = tMax;
	}
	//param[0] = 0, param[n - 1] = 1;

}