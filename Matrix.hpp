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
        bool flag=true;
        // Number of rows
        unsigned int m_rows;
        // Number of columns
        unsigned int m_cols;
        // Number of non zero elements
        unsigned int m_nnz;
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
                return std::cerr<<"SIZING ERROR"<<std::endl;
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

template<Scalar T,ORDER StorageOrder>
void Matrix<T,StorageOrder>::mat_resize(unsigned int rows,unsigned int cols){
    if(is_compressed()){
        //we need the UNCOMPRESSED VERSION so we need to uncompress it
        uncompress();
    }
    
    if(StorageOrder==ORDER::ROW_ORDER){
        //if my matrix is bigger than the new dimensions I must cancel out the elements from the map
        if(rows < m_rows || cols < m_cols){
            for(auto it=m_data_r.begin();it!=m_data_r.end();it++){
                if(it->first[0]>=rows || it->first[1]>=cols){
                    m_data_r.erase(it);
                }
            }
        }

        m_rows=rows;
        m_cols=cols;
    }

    if(StorageOrder==ORDER::COL_ORDER){
        //if my matrix is bigger than the new dimensions I must cancel out the elements from the map
        if(rows < m_rows || cols < m_cols){
            for(auto it=m_data_c.begin();it!=m_data_c.end();it++){
                if(it->first[0]>=rows || it->first[1]>=cols){
                    m_data_c.erase(it);
                }
            }
        }

        m_rows=rows;
        m_cols=cols;
    }


};

template<Scalar T,ORDER StorageOrder>
T& Matrix<T,StorageOrder>::operator()(std::size_t i,std::size_t j){
    // if UNCOMPRESSED add new element
    if(!is_compressed()){
        if(StorageOrder==ORDER::ROW_ORDER){
            if(i<m_rows && j<m_cols){
                auto it = m_data_r.find({i,j});
                if(it!=m_data_r.end()){
                    return it->second;
                }
                else{
                    //here you need to create the element in the map with a default value of type Scalar S
                    T value;
                    std::array<std::size_t,2> v={i,j};
                    std::pair<std::array<std::size_t,2>,T> pair(v,value);
                    m_data_r.insert(pair);
                    return value;
                }
            }
            else{
                std::cerr<<"Out of bounds"<<std::endl;
                T def_val;
                return def_val;
            }
        }
        if(StorageOrder==ORDER::COL_ORDER){
            // CHECK COL ORDERING
            if(i<m_rows && j<m_cols){
                auto it = m_data_c.find({i,j});
                if(it!=m_data_c.end()){
                    return it->second;
                }
                else{
                    //here you need to create the element in the map with a default value of type Scalar S
                    T value;
                    std::array<std::size_t,2> v={i,j};
                    std::pair<std::array<std::size_t,2>,T> pair(v,value);
                    m_data_r.insert(pair);
                    return value;
                }
            }
            else{
                std::cerr<<"Out of bounds"<<std::endl;
                T def_val;
                return def_val;
            }
        }
    }
    // if COMPRESSED add only if there exists ALREADY an element in the position i,j DIFFERENT FROM ZERO
    else{
        if(StorageOrder==ORDER::ROW_ORDER){
            //ROW ORDERING
            if(i<m_rows && j<m_cols){
                //to check if effectively an element is present
                if(rows_indexes[i+1]>0){
                    for(std::size_t k=rows_indexes[i];k<rows_indexes[i+1];k++){
                        if(cols_indexes[k]==j){
                            //ELEMENT NNZ IS PRESENT
                            return m_data_stat[k];
                        }
                    }
                    std::cerr<<"Element non zero non present"<<std::endl;
                    T def_val;
                    return def_val;
                }
                else{
                    std::cerr<<"Element non zero non present"<<std::endl;
                    T def_val;
                    return def_val;
                }
            }
            else{
                std::cerr<<"Out of bounds"<<std::endl;
                T def_val;
                return def_val;
            }
        }
        if(StorageOrder==ORDER::COL_ORDER){
            //COL ORDERING
            if(i<m_rows && j<m_cols){
                //to check if effectively an element is present
                if(cols_indexes[j+1]>0){
                    for(std::size_t k=cols_indexes[j];k<cols_indexes[j+1];k++){
                        if(rows_indexes[k]==i){
                            // ELEMENT IS PRESENT
                            return m_data_stat[k];
                        }
                    }
                    std::cerr<<"Element non zero non present"<<std::endl;
                    T def_val;
                    return def_val;
                }
                else{
                    std::cerr<<"Element non zero non present"<<std::endl;
                    T def_val;
                    return def_val;
                }
            }
            else{
                std::cerr<<"Out of bounds"<<std::endl;
                T def_val;
                return def_val;
            }
        }
    } 
};

template<Scalar T,ORDER StorageOrder>
T Matrix<T,StorageOrder>::operator()(std::size_t i,std::size_t j) const{
    // if UNCOMPRESSED add new element
    if(!is_compressed()){
        if(StorageOrder==ORDER::ROW_ORDER){
            if(i<m_rows && j<m_cols){
                auto it = m_data_r.find({i,j});
                if(it!=m_data_r.end()){
                    return it->second;
                }
                else{
                    //here you need to create the element in the map with a default value of type Scalar S
                    return 0.0;
                }
            }
            else{
                std::cerr<<"Out of bounds"<<std::endl;
                T def_val;
                return def_val;
            }
        }
        if(StorageOrder==ORDER::COL_ORDER){
            // CHECK COL ORDERING
            if(i<m_rows && j<m_cols){
                auto it = m_data_c.find({i,j});
                if(it!=m_data_c.end()){
                    return it->second;
                }
                else{
                    //here you need to create the element in the map with a default value of type Scalar S
                    return 0.0;
                }
            }
            else{
                std::cerr<<"Out of bounds"<<std::endl;
                T def_val;
                return def_val;
            }
        }
    }
    // if COMPRESSED add only if there exists ALREADY an element in the position i,j DIFFERENT FROM ZERO
    else{
        if(StorageOrder==ORDER::ROW_ORDER){
            //ROW ORDERING
            if(i<m_rows && j<m_cols){
                //to check if effectively an element is present
                if(rows_indexes[i+1]>0){
                    // how many nnz there are before the row i
                    int cont_nnz=rows_indexes[i];
                    // how many nnz there are INSIDE row i
                    int delta_nnz=rows_indexes[i+1]-rows_indexes[i];
                    for(std::size_t k=cont_nnz;k<cont_nnz+delta_nnz;k++){
                        if(cols_indexes[k]==j){
                            //ELEMENT NNZ IS PRESENT
                            return m_data_stat[k];
                        }
                    }
                    return 0.0;
                }
                else{
                    return 0.0;
                }
            }
            else{
                std::cerr<<"Out of bounds"<<std::endl;
                T def_val;
                return def_val;
            }
        }
        if(StorageOrder==ORDER::COL_ORDER){
            //COL ORDERING
            if(i<m_rows && j<m_cols){
                //to check if effectively an element is present
                if(cols_indexes[j+1]>0){
                    //how many nnz there are before the col j
                    int cont_nnz=cols_indexes[j];
                    //how many nnz there are inside col j
                    int delta_nnz=cols_indexes[j+1]-cols_indexes[j];
                    for(std::size_t k=cont_nnz;k<cont_nnz+delta_nnz;k++){
                        if(rows_indexes[k]==i){
                            // ELEMENT IS PRESENT
                            return m_data_stat[k];
                        }
                    }
                    return 0.0;
                }
                else{
                    return 0.0;
                }
            }
            else{
                std::cerr<<"Out of bounds"<<std::endl;
                T def_val;
                return def_val;
            }
        }
    } 
};

template<Scalar T,ORDER StorageOrder>
void Matrix<T,StorageOrder>::compress(){
    // if it's already compressed, return
    if(is_compressed()){
        return;
    }
    mat_resize(m_rows,m_cols);
    // choose the compression depending on the Ordering
    flag=true;
    if(StorageOrder==ORDER::ROW_ORDER){
        // ROW ORDERING
        // you need to count the nnz elements for each row
        int cont_nnz=0;
        rows_indexes[0]=0;
        // for all the row indexes
        for(std::size_t i=0;i<m_rows;i++){
            // cross the whole map
            for(auto it=m_data_r.begin();it!=m_data_r.end() && it->first[0]==i;it++){
                cont_nnz++;
                cols_indexes[cont_nnz]=it->first[1];
                m_data_stat[cont_nnz]=it->second;
            }
            rows_indexes[i+1]=cont_nnz;
        } 
        // now empty the map
        for(auto it=m_data_r.begin();it!=m_data_r.end();it++){
            m_data_r.erase(it);
        }
    }
    if(StorageOrder==ORDER::COL_ORDER){
        // COL ORDERING
        // you need to count the nnz elements for each column
        int cont_nnz=0;
        cols_indexes[0]=0;
        // for all the col indexes
        for(std::size_t j=0;j<m_cols;j++){
            // cross the whole map
            for(auto it=m_data_c.begin();it!=m_data_c.end() && it->first[1]==j;it++){
                cont_nnz++;
                rows_indexes[cont_nnz]=it->first[0];
                m_data_stat[cont_nnz]=it->second;
            }
            cols_indexes[j+1]=cont_nnz;
        }
        // now empty the map
        for(auto it=m_data_c.begin();it!=m_data_c.end();it++){
            m_data_c.erase(it);
        }
    }
};

template<Scalar T,ORDER StorageOrder>
void Matrix<T,StorageOrder>::uncompress(){
    if(!is_compressed()){
        return;
    }
    flag=false;
    mat_resize(m_rows,m_cols);
    if(StorageOrder==ORDER::ROW_ORDER){
        // ROW ORDERING
        for(std::size_t i=0;i<m_rows;i++){
            for(std::size_t j=rows_indexes[i];j<rows_indexes[i+1];j++){
                std::array<std::size_t,2> v={i,cols_indexes[j]};
                m_data_r.insert(std::make_pair<std::array<std::size_t,2>,T>(v,m_data_stat[j]));
            }
        }
    }
    if(StorageOrder==ORDER::COL_ORDER){
        // COL ORDERING
        for(std::size_t i=0;i<m_cols;i++){
            for(std::size_t j=cols_indexes[i];j<cols_indexes[i+1];j++){
                std::array<std::size_t,2> v={rows_indexes[j],i};
                m_data_c.insert(std::make_pair<std::array<std::size_t,2>,T>(v,m_data_stat[j]));
            }
        }
    }
    // clear the Compressed vectors
    rows_indexes.clear();
    cols_indexes.clear();
    m_data_stat.clear();
};

template<Scalar T,ORDER StorageOrder>
void Matrix<T,StorageOrder>::readMarketFormat(const std::string& name){
    // we take the file as a list of rows so we need to read row by row
    std::ifstream input(name);
    std::string line;
    std::getline(input,line);

    while(line[0]=='%'){
        // if there are comments, go to the next line
        std::getline(input,line);
    }

    // now transform the first line to read into an istringstream
    std::istringstream datas_line(line);
    std::size_t n_rows,n_cols,n_nnz;
    datas_line >> n_rows >> n_cols >> n_nnz;
    std::getline(input,line);

    //now take every line and memorize datas in an Uncompressed way
    (*this).mat_resize(n_rows,n_cols);
    std::size_t cont=0;
    while(cont<=n_nnz){
        std::istringstream curr_line(line);
        std::size_t i,j;
        T value;
        curr_line >> i >> j >> value;
        std::array<std::size_t,2> v={i-1,j-1};
        if(StorageOrder==ORDER::ROW_ORDER){
            m_data_r[v]=value;
        }
        if(StorageOrder==ORDER::COL_ORDER){
            m_data_c[v]=value;
        }
        std::getline(input,line);
    }
};


#endif //MATRIX_HPP_