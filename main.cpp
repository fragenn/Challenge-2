#include "Matrix.hpp"
#include "Matrix_implementation.hpp"
#include "chrono.hpp"


int main(int argc,char* argv[]){
    // Read and memorize the matrix uncompressed by row
    Matrix<double,ORDER::ROW_ORDER> A_r(1,1);
    A_r.readMarketFormat("lnsp_131.mtx");

    // Read and memorize the matrix uncompressed by column
    Matrix<double,ORDER::COL_ORDER> A_c;
    A_c.readMarketFormat("lnsp_131.mtx");

    // Create the vector of right dimension
    std::vector<double> v(131,1.0);

    // Create the vector of results
    std::vector<double> res_row;
    std::vector<double> res_col;

    // Start the chronometer
    Timings::Chrono chrono;

    // TEST 1: Product with Matrix Uncompressed by Rows
    chrono.start();
    res_row = A_r*v;
    chrono.stop();
    std::cout<<"Time needed for product with Matrix in Uncompressed - Rows "<<std::endl;
    std::cout<<chrono<<std::endl;

    // TEST 2: Product with Matrix Uncompressed by Cols
    chrono.start();
    res_col = A_c*v;
    chrono.stop();
    std::cout<<"Time needed for product with Matrix in Uncompressed - Cols "<<std::endl;
    std::cout<<chrono<<std::endl;

    // TEST 3: Product with Matrix CSR
    A_r.compress();
    chrono.start();
    res_row = A_r*v;
    chrono.stop();
    std::cout<<"Time needed for product with Matrix in CSR "<<std::endl;
    std::cout<<chrono<<std::endl;

    // TEST 4: Product with Matrix CSC
    A_c.compress();
    chrono.start();
    res_row = A_c*v;
    chrono.stop();
    std::cout<<"Time needed for product with Matrix in CSC "<<std::endl;
    std::cout<<chrono<<std::endl;

    // Clear all the memory
    A_r.clear();
    A_c.clear();
    v.clear();

    return 0;
};
