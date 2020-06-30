#include "multigrid.h"
#include <iostream>
#include <ctime>
#include <fstream>
#include <iomanip>
#define pi 3.141592653589793238462643383279
using std::cout;
using std::endl;

Grid* generateHomogDirichletGrid(int nx, int ny) {
	std::vector<std::tuple<double, double, double>> points;
	vector<std::tuple<vector<int>, int, vector<double>>> boundaries;
	boundaries.resize(1);
	std::tuple<vector<int>, int, vector<double>> & bound = boundaries[0];
	vector<double> actualSoln_homog;
	vector<double> actualSoln_inhomog;
	Eigen::VectorXd source = Eigen::VectorXd(nx*ny);
	int pointIndex = 0;
	for (int i = 0; i < nx; i++) {
		for (int j = 0; j < ny; j++) {
			points.push_back(std::tuple<double, double, double>(i*1.0/(nx-1), j*1.0/(ny-1), 0));
			if (i == nx-1 || i == 0 || j == 0 || j == ny-1) {
				std::get<0>(bound).push_back(pointIndex);
			}
			source(pointIndex) = -2 * pi*pi*std::sin(pi*i / (double)(nx-1))*std::sin(pi*j / (double)(ny-1));
			actualSoln_homog.push_back(std::sin(pi*i / (double)(nx-1))*std::sin(pi*j / (double)(ny-1)));
			pointIndex++;
		}
	}
	Boundary boundary;
	boundary.bcPoints = std::get<0>(boundaries[0]);
	boundary.type = std::get<1>(boundaries[0]);
	boundary.values = std::get<2>(boundaries[0]);
	vector<Boundary> bcs;
	bcs.push_back(boundary);
	GridProperties props;
	props.rbfExp = 3;
	props.iters = 7;
	props.polyDeg = 5;
	// cloud size
	props.stencilSize = (int)(0.75*(props.polyDeg + 1) * (props.polyDeg + 2));
	props.omega = 1.5;
	Grid* grid = new Grid(points, bcs, props, source);
	vector<double> bcValues = vector<double>(4*(nx-1), 0.0);
	grid->setBCFlag(0, std::string("dirichlet"), bcValues);
	grid->build_laplacian();
	return grid;
}
int writeRes(vector<double> res)
{
	std::ofstream file;
	file.open("residual.txt");
	//f << fixed << setprecision(2) << endl;
	for (size_t i = 0; i < res.size(); i++) {
		file << res[i] << "\n";
	}
	file.close();
	return 0;
}
 int main() {
	 /*
	//100x100 sor now runs in 51 ms.
	
	Grid* testGrid = generateHomogDirichletGrid(63, 63);
	vector<double> res;
	testGrid->boundaryOp("fine");
	for (int i = 0; i < 1000; i++) {
		testGrid->sor(testGrid->laplaceMat_, testGrid->values_, &testGrid->source_);
		res.push_back(testGrid->residual().norm() / testGrid->source_.norm());
	}
	writeRes(res);
	*/
	
	Multigrid mg = Multigrid();
	clock_t start = std::clock();
	//mg.addGrid(generateHomogDirichletGrid(1000, 1000));
	//mg.addGrid(generateHomogDirichletGrid(500, 500));
	//mg.addGrid(generateHomogDirichletGrid(250, 250));
	//mg.addGrid(generateHomogDirichletGrid(125, 125));
	mg.addGrid(generateHomogDirichletGrid(100,100));
	mg.addGrid(generateHomogDirichletGrid(50, 50));
	mg.addGrid(generateHomogDirichletGrid(25, 25));
	mg.addGrid(generateHomogDirichletGrid(13, 13));
	mg.buildMatrices();
	clock_t build = std::clock();
	cout << (build - start) / ((double)CLOCKS_PER_SEC) << endl;
	vector<double> res;
	for (int i = 0; i < 30; i++) {
		res.push_back(mg.residual());
		mg.vCycle();
	}
	writeRes(res);
	cout << (std::clock() - build) / ((double)CLOCKS_PER_SEC) << endl;

}
