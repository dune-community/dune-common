// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef __DUNE_FUNCTION_HH__
#define __DUNE_FUNCTION_HH__

#include "mapping.hh"


namespace Dune {


  template< class FunctionSpaceType, class FunctionImp>
  class Function : public Mapping <typename FunctionSpaceType::RangeField , typename FunctionSpaceType::Domain, typename FunctionSpaceType::Range > {

  public:
    typedef typename FunctionSpaceType::Domain Domain ;
    typedef typename FunctionSpaceType::Range Range ;
    typedef typename FunctionSpaceType::JacobianRange JacobianRange;
    typedef typename FunctionSpaceType::HessianRange HessianRange;
    typedef FunctionSpaceType FunctionSpace;

    Function ( FunctionSpaceType & f ) : functionSpace_ (f) {} ;

    void evaluate ( const Domain & , Range &) const ;

    FunctionSpaceType &getFunctionSpace() const { return functionSpace_; }

  protected:

    FunctionSpaceType & functionSpace_;

  };


}

#endif
