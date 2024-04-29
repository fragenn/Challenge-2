#ifndef MATRIX_TRAITS_HPP_
#define MATRIX_TRAITS_HPP_

#include <type_traits>
#include <concepts>

// We need to define a type that can be convertible 
// to a scalar as an element of the Matrix 
// (we need to distinguish between zero and non zero)
template<class T>
concept Scalar = std::is_floating_point<std::decay_t<T>>::value;
// we use decay_t in order to dereference (e.g. in case T=double&)

// I need also a struct which does the normal comparison between the array indexes
// if the matrix is Column-ordered
struct CompareByColumns{
    bool compare(const std::array<std::size_t,2>& a , const std::array<std::size_t,2>& b ) const {
        return std::tie(a[1],a[0])<std::tie(b[1],b[0]);
    }
};

// We add also the treatment of the complex numbers
template<class T>
concept Complex = requires (T x){
    using Val_Type=typename T::value_type;
    {x.conj()}->std::same_as<Val_Type>;
    {x.real()}->std::convertible_to<double>;
}

// We consider the concept which admits both Scalar or Complex
template<class T>
concept ValueType = Scalar<T> || Complex<T>;
#endif //MATRIX_TRAITS_HPP_