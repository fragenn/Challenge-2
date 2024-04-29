#include "Matrix.hpp"
//#include "Matrix_Implementation.hpp"
#include "chrono.hpp"


int main(int argc,char* argv[]){

    // Read and memorize the matrix by row
    Matrix<double,ORDER::ROW_ORDER> A_r(1,1);
    A_r(0,0)=1.0;
    A_r.readMarketFormat("Insp_131.mtx");

    // Create the vector of right dimension
    std::vector<double> v(131,1.0);

    // Start the chronometer
    Timings::Chrono chrono;
    chrono.start();



    return 0;
};