#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <fstream>
#include <sstream>
#include "MatrixTraits.hpp"

enum class ORDER{
    ROW_ORDER,
    COL_ORDER,
};

template<Scalar T,ORDER StorageOrder>
class Matrix{
    using MatrixUncompressedRow = std::map<std::array<std::size_t,2>,T>;
    using MatrixUncompressedCol = std::map<std::array<std::size_t,2>,T,CompareByColumns>;
    private:
        // Flag (TRUE if Matrix is compressed, FALSE if uncompressed)
        bool flag=false;
        // Number of rows
        unsigned int m_rows=0;
        // Number of columns
        unsigned int m_cols=0;
        // Number of non zero elements
        unsigned int m_nnz=0;
        // Matrix in dynamic storage by ROWS or COLS
        MatrixUncompressedRow m_data_r;
        MatrixUncompressedCol m_data_c;
        // Matrix in static storage (CSR) (CSC)
        std::vector<std::size_t> rows_indexes;
        std::vector<std::size_t> cols_indexes;
        std::vector<T> m_data_stat;

    public:
        // Default constructor
        Matrix():m_rows(1),m_cols(1){
            (*this).mat_resize(1,1);
        };
        // Shortened Constructor (only with size)
        Matrix(unsigned int rows,unsigned int cols):m_rows(rows),m_cols(cols){
            (*this).mat_resize(rows,cols);
        };
        // Clear memory
        void clear();
        // Call operators (both CONST and NON-CONST)
        T operator()(std::size_t i, std::size_t j) const;
        T& operator()(std::size_t i, std::size_t j);
        // Compress and uncompress methods
        void compress();
        void uncompress();
        // Is_Compressed method
        bool is_compressed() const{return flag;};
        // Matrix resize
        void mat_resize(unsigned int rows,unsigned int cols);
        // Matrix Reader
        void readMarketFormat(const std::string& name);
        // Product Matrix-Vector
        friend std::vector<T> operator*(const Matrix<T,StorageOrder>& A,const std::vector<T>& v){
            // declare the result
            T def_val;
            std::vector<T> result(A.m_rows,def_val);
            // exit if there are problems of sizing
            if(A.m_cols!=v.size()){
                std::cerr<<"SIZING ERROR"<<std::endl;
                return v;
            }
            // check the different 4 cases
            if(A.is_compressed()){
                if(StorageOrder==ORDER::ROW_ORDER){
                    // CASE 1: CSR
                    // for all the elements in result
                    for(std::size_t i=0;i<result.size();i++){
                        T sum = 0.0;
                        //loop over the nnz elements of row i
                        for(std::size_t k=A.rows_indexes[i];k<A.rows_indexes[i+1];k++){
                            // take the column index
                            std::size_t j=A.cols_indexes[k];
                            sum += A.m_data_stat[k] * v[j];
                        }
                        result[i] = sum;
                    }
                    return result;
                }
                if(StorageOrder==ORDER::COL_ORDER){
                    // CASE 2: CSC
                    // in order to perform a linear combination of the columns
                    for(std::size_t j=0;j<A.m_cols;j++){
                        T val=v[j];
                        for(std::size_t k=A.cols_indexes[j];k<A.cols_indexes[j+1];k++){
                            //take the row index
                            std::size_t i=A.rows_indexes[k];
                            result[i] += val*A.m_data_stat[k];
                        }
                    }
                    return result;
                }
            }
            else{
                if(StorageOrder==ORDER::ROW_ORDER){
                    // CASE 3: Uncompr - Row
                    for(auto it=A.m_data_r.begin();it!=A.m_data_r.end();it++){
                        std::size_t i=it->first[0];
                        std::size_t j=it->first[1];
                        T value=it->second;
                        result[i] += value*v[j];
                    }
                    return result;
                }
                if(StorageOrder==ORDER::COL_ORDER){
                    // CASE 4: Uncompr - Col
                    for(auto it=A.m_data_c.begin();it!=A.m_data_c.end();it++){
                        std::size_t i=it->first[0];
                        std::size_t j=it->first[1];
                        T value=it->second;
                        result[i] += value*v[j];
                    }
                    return result;
                }
            }
        };
};

#endif //MATRIX_HPP_