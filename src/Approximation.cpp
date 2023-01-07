#include "Approximation.hpp"
#include <Eigen/Dense>
//Use the uniform method to set the parameters and the knot vectors
void UniformApprox(std::vector<glm::dvec3> points, int conptsSize, int degree,
	std::vector<double>& knots, std::vector<double>& param) {

	int n = points.size();
	int m = conptsSize + degree + 1;

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

void ChorldApprox(std::vector<glm::dvec3> points, int conptsSize, int degree,
	std::vector<double>& knots, std::vector<double>& param) {

	int n = points.size();
	int m = conptsSize + degree + 1;
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

void CentripetalApprox(std::vector<glm::dvec3> points, int conptsSize, int degree,
	std::vector<double>& knots, std::vector<double>& param) {

	int n = points.size();
	int m = conptsSize + degree + 1;
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
void UniversalApprox(std::vector<glm::dvec3> points, int conptsSize, int degree,
	std::vector<double>& knots, std::vector<double>& param) {
	int n = points.size();
	int m = conptsSize + degree + 1;

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



void Approximation(int Choice, std::vector<glm::dvec3> points, int degree, std::vector<double>& knots,
	std::vector<double>& param, int h, std::vector<glm::dvec3>& controlPts) {

	switch(Choice) {
		case 0:
			UniformApprox(points, h + 1, degree, knots, param);
			break;
		case 1:
			ChorldApprox(points, h + 1, degree, knots, param);
			break;
		case 2:
			UniversalApprox(points, h + 1, degree, knots, param);
			break;
		case 3:
			CentripetalApprox(points, h + 1, degree, knots, param);
			break;

		default:
			return;
	}

	controlPts.resize(h + 1);
	int n = points.size() - 1;
	Eigen::MatrixXd matN(n + 1, h + 1);
	Eigen::MatrixXd matP_0(h + 1, 1);
	Eigen::MatrixXd matP_1(h + 1, 1);

	Eigen::MatrixXd matQ_0(h + 1, 1);
	Eigen::MatrixXd matQ_1(h + 1, 1);


	//(n+1) points to be fitting; (h+1) control points of b-spline curve
	for(int i = 0; i <= n; i++) {
		for(int j = 0; j <= h; j++) {
			matN(i, j) = tinynurbs::bsplineOneBasis(j, degree, knots, param[i]);
		}
	}

	for(int i = 0; i <= h; i++) {
		glm::dvec3 sum(0);
		for(int k = 1; k <= n - 1; k++) {
			auto Qk = points[k] - tinynurbs::bsplineOneBasis(0, degree, knots, param[k]) * points[0] - tinynurbs::bsplineOneBasis(h, degree, knots, param[k]) * points[n];
			sum = sum + Qk * tinynurbs::bsplineOneBasis(i, degree, knots, param[k]);
		}
		matQ_0(i, 0) = sum.x;
		matQ_1(i, 0) = sum.y;
	}
	Eigen::MatrixXd matRight = matN.transpose() * matN;
	matP_0 = matRight.colPivHouseholderQr().solve(matQ_0);
	matP_1 = matRight.colPivHouseholderQr().solve(matQ_1);

	controlPts[0] = points[0];
	controlPts[h] = points[n];
	for(int i = 1; i < h; i++) {
		controlPts[i].x = matP_0(i, 0);
		controlPts[i].y = matP_1(i, 0);
		controlPts[i].z = 0;
	}


}
