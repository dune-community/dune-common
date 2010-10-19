// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#define DUNE_ISTL_WITH_CHECKING
#include "../fmatrix.hh"
#include <iostream>
#include <algorithm>
#include <vector>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

using namespace Dune;

template <class T>
std::string className(T &t)
{
#ifdef __GNUC__
  int status;
  return abi::__cxa_demangle(typeid(t).name(),0,0,&status);
#else
  return typeid(t).name();
#endif
};

template<typename T, std::size_t n>
int test_invert_solve(T A_data[n*n], T inv_data[n*n],
                      T x_data[n], T b_data[n])
{
  int ret=0;

  std::cout <<"Checking inversion of:"<<std::endl;

  FieldMatrix<T,n,n> A, inv, calced_inv;
  FieldVector<T,n> x, b, calced_x;

  for(size_t i =0; i < n; ++i) {
    x[i]=x_data[i];
    b[i]=b_data[i];
    for(size_t j=0; j <n; ++j) {
      A[i][j] = A_data[i*n+j];
      inv[i][j] = inv_data[i*n+j];
    }
  }

  std::cout<<A<<std::endl;

  // Check whether given inverse is correct
  FieldMatrix<T,n,n> prod = A;
  prod.rightmultiply(inv);
  for (size_t i=0; i<n; i++)
    prod[i][i] -= 1;

  bool equal=true;
  if (prod.infinity_norm() > 1e-6) {
    std::cerr<<"Given inverse wrong"<<std::endl;
    equal=false;
  }

  FieldMatrix<T,n,n> copy(A);
  A.invert();

  calced_inv = A;
  A-=inv;


  double singthres = FMatrixPrecision<>::singular_limit()*10;
  for(size_t i =0; i < n; ++i)
    for(size_t j=0; j <n; ++j)
      if(std::abs(A[i][j])>singthres) {
        std::cerr<<"calculated inverse wrong at ("<<i<<","<<j<<")"<<std::endl;
        equal=false;
      }

  if(!equal) {
    ret++;
    std::cerr<<"Calculated inverse was:"<<std::endl;
    std::cerr <<calced_inv<<std::endl;
    std::cerr<<"Should have been"<<std::endl;
    std::cerr<<inv << std::endl;
  }else
    std::cout<<"Result is"<<std::endl<<calced_inv<<std::endl;


  std::cout<<"Checking solution for rhs="<<b<<std::endl;

  // Check whether given solution is correct
  FieldVector<T,n> trhs=b;

  copy.mmv(x,trhs);
  equal=true;

  if (trhs.infinity_norm() > 1e-6) {
    std::cerr<<"Given rhs does not fit solution"<<std::endl;
    equal=false;
  }
  copy.solve(calced_x, b);
  FieldVector<T,n> xcopy(calced_x);
  xcopy-=x;

  equal=true;

  for(size_t i =0; i < n; ++i)
    if(std::abs(xcopy[i])>singthres) {
      std::cerr<<"calculated isolution wrong at ("<<i<<")"<<std::endl;
      equal=false;
    }

  if(!equal) {
    ret++;
    std::cerr<<"Calculated solution was:"<<std::endl;
    std::cerr <<calced_x<<std::endl;
    std::cerr<<"Should have been"<<std::endl;
    std::cerr<<x<<std::endl;
    std::cerr<<"difference is "<<xcopy<<std::endl;
  }else
    std::cout<<"Result is "<<calced_x<<std::endl;

  return ret;
}


int test_invert_solve()
{
  int ret=0;

  double A_data[9] = {1, 5, 7, 2, 14, 15, 4, 40, 39};
  double inv_data[9] = {-9.0/4, 85.0/24, -23.0/24, -3.0/4, 11.0/24, -1.0/24, 1, -5.0/6, 1.0/6};
  double b[3] = {32,75,201};
  double x[3] = {1,2,3};

  ret += test_invert_solve<double,3>(A_data, inv_data, x, b);

  double A_data0[9] = {-0.5, 0, -0.25, 0.5, 0, -0.25, 0, 0.5, 0};
  double inv_data0[9] = {-1, 1, 0, 0, 0, 2, -2, -2, 0};
  double b0[3] = {32,75,201};
  double x0[3] = {43, 402, -214};

  ret += test_invert_solve<double,3>(A_data0, inv_data0, x0, b0);

  double A_data1[9] = {0, 1, 0, 1, 0, 0, 0, 0, 1};
  double b1[3] = {0,1,2};
  double x1[3] = {1,0,2};

  ret += test_invert_solve<double,3>(A_data1, A_data1, x1, b1);

  double A_data2[9] ={3, 1, 6, 2, 1, 3, 1, 1, 1};
  double inv_data2[9] ={-2, 5, -3, 1, -3, 3, 1, -2, 1};
  double b2[3] = {2, 7, 4};
  double x2[3] = {19,-7,-8};

  return ret + test_invert_solve<double,3>(A_data2, inv_data2, x2, b2);
}

template<class K, int n, int m, class X, class Y>
void test_mult(FieldMatrix<K, n, m>& A,
               X& v, Y& f)
{
  // test the various matrix-vector products
  A.mv(v,f);
  A.mtv(f,v);
  A.umv(v,f);
  A.umtv(f,v);
  A.umhv(f,v);
  A.mmv(v,f);
  A.mmtv(f,v);
  A.mmhv(f,v);
  A.usmv(0.5,v,f);
  A.usmtv(0.5,f,v);
  A.usmhv(0.5,f,v);
}


template<class K, int n, int m>
void test_matrix()
{
  typedef typename FieldMatrix<K,n,m>::size_type size_type;

  FieldMatrix<K,n,m> A;
  FieldVector<K,n> f;
  FieldVector<K,m> v;

  // assign matrix
  A=K();
  // random access matrix
  for (size_type i=0; i<n; i++)
    for (size_type j=0; j<m; j++)
      A[i][j] = i*j;
  // iterator matrix
  typename FieldMatrix<K,n,m>::RowIterator rit = A.begin();
  for (; rit!=A.end(); ++rit)
  {
    rit.index();
    typename FieldMatrix<K,n,m>::ColIterator cit = rit->begin();
    for (; cit!=rit->end(); ++cit)
    {
      cit.index();
      (*cit) *= 2;
    }
  }

  // assign vector
  f = 1;

  // random access vector
  for (size_type i=0; i<v.dim(); i++)
    v[i] = i;
  // iterator vector
  typename FieldVector<K,m>::iterator it = v.begin();
  typename FieldVector<K,m>::ConstIterator end = v.end();
  for (; it!=end; ++it)
  {
    it.index();
    (*it) *= 2;
  }
  // reverse iterator vector
  it = v.rbegin();
  end = v.rend();
  for (; it!=end; --it)
    (*it) /= 2;
  // find vector
  for (size_type i=0; i<v.dim(); i++)
  {
    it = v.find(i);
    (*it) += 1;
  }

  // matrix vector product
  A.umv(v,f);
  // check that mv and umv are doing the same thing
  {
    FieldVector<K,n> res2(0);
    FieldVector<K,n> res1;

    FieldVector<K,m> b(1);

    A.mv(b, res1);
    A.umv(b, res2);

    if( (res1 - res2).two_norm() > 1e-12 )
    {
      DUNE_THROW(FMatrixError,"mv and umv are not doing the same!");
    }
  }

  {
    FieldVector<K,m> v0 ( v );
    test_mult(A, v0, f );
  }

  {
    std::vector<K> v1( m ) ;
    std::vector<K> f1( n, 1 ) ;
    // random access vector
    for (size_type i=0; i<v1.size(); i++) v1[i] = i;
    test_mult(A, v1, f1 );
  }
  {
    K v2[ m ];
    K f2[ n ];
    // random access vector
    for (size_type i=0; i<m; ++i) v2[i] = i;
    for (size_type i=0; i<n; ++i) f2[i] = 1;
    test_mult(A, v2, f2 );
  }

  // Test the different matrix norms
  assert( A.frobenius_norm() >= 0 );
  assert( A.frobenius_norm2() >= 0 );
  assert( A.infinity_norm() >= 0 );
  assert( A.infinity_norm_real() >= 0);

  // print matrix
  std::cout << A << std::endl;
  // print vector
  std::cout << f << std::endl;


  {
    FieldMatrix<K,n,m> A2 = A;
    A2 *= 2;

    FieldMatrix<K,n,m> B = A;
    B += A;
    B -= A2;
    if (std::abs(B.infinity_norm()) > 1e-12)
      DUNE_THROW(FMatrixError,"Operator +=/-= test failed!");
  }
  {
    FieldMatrix<K,n,m> A3 = A;
    A3 *= 3;

    FieldMatrix<K,n,m> B = A;
    B.axpy( K( 2 ), B );
    B -= A3;
    if (std::abs(B.infinity_norm()) > 1e-12)
      DUNE_THROW(FMatrixError,"Axpy test failed!");
  }
  {
    FieldMatrix<K,n,n+1> A;
    for(size_type i=0; i<A.N(); ++i)
      for(size_type j=0; j<A.M(); ++j)
        A[i][j] = i;
    const FieldMatrix<K,n,n+1>& Aref = A;


    FieldMatrix<K,n+1,n+1> B;
    for(size_type i=0; i<B.N(); ++i)
      for(size_type j=0; j<B.M(); ++j)
        B[i][j] = i;
    const FieldMatrix<K,n+1,n+1>& Bref = B;

    FieldMatrix<K,n,n> C;
    for(size_type i=0; i<C.N(); ++i)
      for(size_type j=0; j<C.M(); ++j)
        C[i][j] = i;
    const FieldMatrix<K,n,n>& Cref = C;

    FieldMatrix<K,n,n+1> AB = Aref.rightmultiplyany(B);
    for(size_type i=0; i<AB.N(); ++i)
      for(size_type j=0; j<AB.M(); ++j)
        if (std::abs(AB[i][j] - K(i*n*(n+1)/2)) > 1e-10)
          DUNE_THROW(FMatrixError,"Rightmultiplyany test failed!");

    FieldMatrix<K,n,n+1> AB2 = A;
    AB2.rightmultiply(B);
    AB2 -= AB;
    if (std::abs(AB2.infinity_norm()) > 1e-10)
      DUNE_THROW(FMatrixError,"Rightmultiply test failed!");

    FieldMatrix<K,n,n+1> AB3 = Bref.leftmultiplyany(A);
    AB3 -= AB;
    if (std::abs(AB3.infinity_norm() > 1e-10))
      DUNE_THROW(FMatrixError,"Leftmultiplyany test failed!");

    FieldMatrix<K,n,n+1> CA = Aref.leftmultiplyany(C);
    for(size_type i=0; i<CA.N(); ++i)
      for(size_type j=0; j<CA.M(); ++j)
        if (std::abs(CA[i][j] - K(i*n*(n-1)/2)) > 1e-10)
          DUNE_THROW(FMatrixError,"Leftmultiplyany test failed!");

    FieldMatrix<K,n,n+1> CA2 = A;
    CA2.leftmultiply(C);
    CA2 -= CA;
    if (std::abs(CA2.infinity_norm()) > 1e-10)
      DUNE_THROW(FMatrixError,"Leftmultiply test failed!");

    FieldMatrix<K,n,n+1> CA3 = Cref.rightmultiplyany(A);
    CA3 -= CA;
    if (std::abs(CA3.infinity_norm()) > 1e-10)
      DUNE_THROW(FMatrixError,"Rightmultiplyany test failed!");
  }
}

int test_determinant()
{
  int ret = 0;

  FieldMatrix<double, 4, 4> B;
  B[0][0] =  3.0; B[0][1] =  0.0; B[0][2] =  1.0; B[0][3] =  0.0;
  B[1][0] = -1.0; B[1][1] =  3.0; B[1][2] =  0.0; B[1][3] =  0.0;
  B[2][0] = -3.0; B[2][1] =  0.0; B[2][2] = -1.0; B[2][3] =  2.0;
  B[3][0] =  0.0; B[3][1] = -1.0; B[3][2] =  0.0; B[3][3] =  1.0;
  if (std::abs(B.determinant() + 2.0) > 1e-12)
  {
    std::cerr << "Determinant 1 test failed" << std::endl;
    ++ret;
  }

  B[0][0] =  3.0; B[0][1] =  0.0; B[0][2] =  1.0; B[0][3] =  0.0;
  B[1][0] = -1.0; B[1][1] =  3.0; B[1][2] =  0.0; B[1][3] =  0.0;
  B[2][0] = -3.0; B[2][1] =  0.0; B[2][2] = -1.0; B[2][3] =  2.0;
  B[3][0] = -1.0; B[3][1] =  3.0; B[3][2] =  0.0; B[3][3] =  2.0;
  if (B.determinant() != 0.0)
  {
    std::cerr << "Determinant 2 test failed" << std::endl;
    ++ret;
  }

  return 0;
}

template<class ft>
struct ScalarOperatorTest
{
  ScalarOperatorTest()
  {
    ft a = 1;
    ft c = 2;
    FieldMatrix<ft,1,1> v(2);
    FieldMatrix<ft,1,1> w(2);
    bool b;

    std::cout << __func__ << "\t ( " << className(v) << " )" << std::endl;

    a = a * c;
    a = a + c;
    a = a / c;
    a = a - c;

    v = a;
    v = w = v;
    a = v;

    a = v + a;
    a = v - a;
    a = v * a;
    a = v / a;

    v = v + a;
    v = v - a;
    v = v * a;
    v = v / a;

    a = a + v;
    a = a - v;
    a = a * v;
    a = a / v;

    v = a + v;
    v = a - v;
    v = a * v;
    v = a / v;

    v -= v;
    v -= a;
    v += v;
    v += a;
    v *= a;
    v /= a;

    b = (v == a);
    b = (v != a);
    b = (a == v);
    b = (a != v);

  }
};

int main()
{
  try {
    // test 1 x 1 matrices
    test_matrix<float, 1, 1>();
    ScalarOperatorTest<float>();
    test_matrix<double, 1, 1>();
    ScalarOperatorTest<double>();
    // test n x m matrices
    test_matrix<int, 10, 5>();
    test_matrix<double, 5, 10>();
    // test complex matrices
    test_matrix<std::complex<float>, 1, 1>();
    test_matrix<std::complex<double>, 5, 10>();
    // test high level methods
    test_determinant();
    Dune::FieldMatrix<double, 34, 34> A(1e-15);
    for (int i=0; i<34; i++) A[i][i] = 1;
    A.invert();
    return test_invert_solve();
  }
  catch (Dune::Exception & e)
  {
    std::cerr << "Exception: " << e << std::endl;
  }
}
