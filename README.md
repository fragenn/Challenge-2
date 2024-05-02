# Challenge-2

## Abstract
Here I have uploaded my solution for the first Challenge of the Advanced Programming and Scientific Computing (PACS) course in Politecnico di Milano.
The main purpose of the challenge is to implement a program which compares the different performances of the Matrix-Vector product depending on the choice of matrix ordering (`ROW_ORDER` or `COL_ORDER`) and on the type of storage of the matrix inside the memory (Uncompressed (known as `COOmap`) or Compressed (depending on the order `CSR` or `CSC`).
In particular, it was asked to create a matrix class which contains the different data structures needed to work on all the cases listed above and the various methods which define the operations of compressing and uncompressing the matrix, a reader method to work with matrices in the *MatrixMarket* format and all the overloaded call operator and product operator.

## Rules
In order to run and execute correctly the program, simply clone my repository using and write:
```bash
git clone git@github.com:fragenn/Challenge-2.git
make
./main
```
The final output will be the list of the different chronometer times (in microseconds) of the different performances.


## General overview
My practical approach was to use concepts (NOTE: in Makefile I have used the STD=C++20 version in order to work with them) so as to know already since compile-time that the only type supported was the Scalar one (created in order to accept any floating point, which admits to distinguish between zero and non-zero elements). Inside `MatrixTraits.hpp`, indeed, I have explicitly required to have a Scalar or a Complex with necessarily the option to be assignable values (in order to perform the assignments with the operator `=`, without any trouble. Moreover, inside the `Matrix_implementation.hpp` file, every method was implemented considering every of the 4 possible cases that a user could choose (CSR - CSC - COOmapRows - COOmapCols), where the last two ones were implemented and memorized as two different maps, where the key was the `std::array<std::size_t,2>` vector containing the indexes *(i,j)*; furthermore, the different map types are used relying on the chance given by STL containers to work with different comparison operators (by row-ordering or column-ordering), so in `MatrixTraits.hpp`, there can be found the struct `CompareByColumns` which allows the private member of Matrix (`m_data_c`) to compare elements by columns when necessary. Dealing with all those cases brought the asked results of noticing that the product performed with compressed matrices is faster than the one with the uncompressed version.
