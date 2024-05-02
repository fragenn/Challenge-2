#ifndef MATRIX_IMPLEMENTATION_HPP_
#define MATRIX_IMPLEMENTATION_HPP_

#include "Matrix.hpp"
#include "MatrixTraits.hpp"

template<ValueType T,ORDER StorageOrder>
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

template<ValueType T,ORDER StorageOrder>
T& Matrix<T,StorageOrder>::operator()(std::size_t i,std::size_t j){
    // if UNCOMPRESSED add new element
    if(!is_compressed()){
        if(StorageOrder==ORDER::ROW_ORDER){
            if(i<m_rows && j<m_cols){
                std::array<std::size_t,2> v={i,j};
                auto it=m_data_r.find(v);
                if(it==m_data_r.end()){
                    // CREATE THE NEW ELEMENT
                    T def_val;
                    m_data_r[v]=def_val;
                    return it->second;
                }
                return it->second;
            }
            // INDEXES ARE OUT OF BOUNDS SO IN THE NON-CONST VERSION YOU EXIT THE PROGRAM WITHOUT RETURNING ANYTHING
            throw std::runtime_error("Indexes out of bounds");
        }
        if(StorageOrder==ORDER::COL_ORDER){
            // CHECK COL ORDERING
            if(i<m_rows && j<m_cols){
                std::array<std::size_t,2> v={i,j};
                auto it=m_data_c.find(v);
                if(it==m_data_c.end()){
                    // CREATE THE NEW ELEMENT
                    T def_val;
                    m_data_c[v]=def_val;
                    return it->second;
                }
                return it->second;
            }
            throw std::runtime_error("Indexes out of bounds");
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
                    // ELEMENT NNZ IS NOT PRESENT
                    throw std::runtime_error("Element non zero non present");
                }
                else{
                    // ELEMENT NNZ IS NOT PRESENT
                    throw std::runtime_error("Element non zero non present");
                }
            }
            else{
                // OUT OF BOUNDS
                throw std::runtime_error("Indexes out of bounds");
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
                    // ELEMENT NNZ IS NOT PRESENT
                    throw std::runtime_error("Element non zero non present");
                }
                else{
                    // ELEMENT NNZ IS NOT PRESENT
                    throw std::runtime_error("Element non zero non present");
                }
            }
            else{
                // OUT OF BOUNDS
                throw std::runtime_error("Indexes out of bounds");
            }
        }
    } 
};

template<ValueType T,ORDER StorageOrder>
T Matrix<T,StorageOrder>::operator()(std::size_t i,std::size_t j) const{
    // create a default value (NULL) to return if you haven't got the element
    static T def_val;
    // if UNCOMPRESSED add new element
    if(!is_compressed()){
        if(StorageOrder==ORDER::ROW_ORDER){
            if(i<m_rows && j<m_cols){
                std::array<std::size_t,2> v={i,j};
                auto it = m_data_r.find(v);
                if(it!=m_data_r.end()){
                    return it->second;
                }
                else{
                    // HERE YOU NEED TO RETURN 0
                    return def_val;
                }
            }
            else{
                // OUT OF BOUNDS
                throw std::runtime_error("Indexes out of bounds");
            }
        }
        if(StorageOrder==ORDER::COL_ORDER){
            // CHECK COL ORDERING
            if(i<m_rows && j<m_cols){
                std::array<std::size_t,2> v={i,j};
                auto it = m_data_c.find(v);
                if(it!=m_data_c.end()){
                    return it->second;
                }
                else{
                    // HERE YOU NEED TO RETURN 0
                    return def_val;
                }
            }
            else{
                // OUT OF BOUNDS
                throw std::runtime_error("Indexes out of bounds");
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
                    return def_val;
                }
                else{
                    return def_val;
                }
            }
            else{
                // OUT OF BOUNDS
                throw std::runtime_error("Indexes out of bounds");
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
                    return def_val;
                }
                else{
                    return def_val;
                }
            }
            else{
                // OUT OF BOUNDS
                throw std::runtime_error("Indexes out of bounds");
            }
        }
    } 
};

template<ValueType T,ORDER StorageOrder>
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
        // at first resize the vector of rows indexes
        rows_indexes.resize(m_rows+1);
        // you need to count the nnz elements for each row
        int cont_nnz=0;
        rows_indexes.push_back(0);
        // for all the row indexes
        for(std::size_t i=0;i<m_rows;i++){
            // cross the whole map
            for(auto it=m_data_r.begin();it!=m_data_r.end() && it->first[0]==i;it++){
                cont_nnz++;
                cols_indexes.push_back(it->first[1]);
                m_data_stat.push_back(it->second);
            }
            rows_indexes.push_back(cont_nnz);
        } 
        // now empty the map
        m_data_r.clear();
    }
    if(StorageOrder==ORDER::COL_ORDER){
        // COL ORDERING
        // at first resize the vector of columns indexes
        cols_indexes.resize(m_cols+1);
        // you need to count the nnz elements for each column
        int cont_nnz=0;
        cols_indexes.push_back(0);
        // for all the col indexes
        for(std::size_t j=0;j<m_cols;j++){
            // cross the whole map
            for(auto it=m_data_c.begin();it!=m_data_c.end() && it->first[1]==j;it++){
                cont_nnz++;
                rows_indexes.push_back(it->first[0]);
                m_data_stat.push_back(it->second);
            }
            cols_indexes.push_back(cont_nnz);
        }
        // now empty the map
        m_data_c.clear();
    }
};

template<ValueType T,ORDER StorageOrder>
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
                std::array<std::size_t,2> v={i,j};
                std::pair<std::array<std::size_t,2>,T> pair(v,m_data_stat[j]);
                m_data_r.insert(pair);
            }
        }
    }
    if(StorageOrder==ORDER::COL_ORDER){
        // COL ORDERING
        for(std::size_t i=0;i<m_cols;i++){
            for(std::size_t j=cols_indexes[i];j<cols_indexes[i+1];j++){
                std::array<std::size_t,2> v={i,j};
                std::pair<std::array<std::size_t,2>,T> pair(v,m_data_stat[j]);
                m_data_c.insert(pair);
            }
        }
    }
    // clear the Compressed vectors
    rows_indexes.clear();
    cols_indexes.clear();
    m_data_stat.clear();
};

template<ValueType T,ORDER StorageOrder>
void Matrix<T,StorageOrder>::readMarketFormat(const std::string& name){
    // we take the file as a list of rows so we need to read row by row
    std::ifstream input(name);
    std::string line;
    std::getline(input,line);

    while(line[0]=='%'){
        // we don't want the commented lines
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
        //(*this)(i-1,j-1)=value;
        //std::cout<<(*this)(i-1,j-1)<<std::endl;
        std::array<std::size_t,2> v={i-1,j-1};
        if(StorageOrder==ORDER::ROW_ORDER){
            m_data_r[v]=value;
        }
        if(StorageOrder==ORDER::COL_ORDER){
            m_data_c[v]=value;
        }
        std::getline(input,line);
        cont++;
        /*if(StorageOrder==ORDER::ROW_ORDER){
            std::cout<<m_data_r[v]<<std::endl;
        }
        if(StorageOrder==ORDER::COL_ORDER){
            std::cout<<m_data_c[v]<<std::endl;
        }
        */
    }
};

template<typename T,ORDER StorageOrder>
void Matrix<T,StorageOrder>::clear(){
    if(is_compressed()){
        rows_indexes.clear();
        cols_indexes.clear();
        m_data_stat.clear();
        return;
    }
    else{
        if(StorageOrder==ORDER::ROW_ORDER){
            m_data_r.clear();
        }
        if(StorageOrder==ORDER::COL_ORDER){
            m_data_c.clear();
        }
        return;
    }
};

#endif //MATRIX_IMPLEMENTATION_HPP_