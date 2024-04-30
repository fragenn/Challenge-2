#ifndef MATRIX_IMPLEMENTATION_HPP_
#define MATRIX_IMPLEMENTATION_HPP_

#include "Matrix.hpp"
#include "MatrixTraits.hpp"

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

#endif //MATRIX_IMPLEMENTATION_HPP_