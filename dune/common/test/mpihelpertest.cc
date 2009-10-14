// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dune/common/mpihelper.hh>
#include <iostream>
int main(int argc, char** argv)
{
  typedef Dune::MPIHelper Helper;

  {
    Helper& mpi = Helper::instance(argc, argv);

    Helper::MPICommunicator comm = mpi.getCommunicator();
    comm= mpi.getCommunicator();
  }

  {
    Helper& mpi = Helper::instance(argc, argv);

    Helper::MPICommunicator comm= mpi.getCommunicator();
    comm= mpi.getCommunicator();
  }
  std::cout << "We are at the end!"<<std::endl;

}