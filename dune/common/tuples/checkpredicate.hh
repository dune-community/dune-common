#ifndef DUNE_COMMON_TUPLES_CHECKPREDICATE_HH
#define DUNE_COMMON_TUPLES_CHECKPREDICATE_HH

#include <dune/common/tuples/foreach.hh>
#include <dune/common/tuples/transform.hh>
#include <dune/common/tuples/tuples.hh>

namespace Dune
{

  // CheckPredicateTuple
  // -------------------

  /** \ingroup Tuples_Algorithms
   *
   *  \brief Check a static predicate for all tuple elements.
   *
   *  \tparam  Tuple      Some tuple type
   *  \tparam  Predicate  Unary static predicate
   *  \tparam  N          Implementation internal
   *
   *  The predicate is expected to provide a static boolean member
   *  \c v, i.e.,
   *  @code
   *  template< class T >
   *  struct Predicate
   *  {
   *    static const bool value = ...;
   *  };
   *  @endcode
   */
  template< class Tuple, template< class > class Predicate,
            int N = Dune::tuple_size< Tuple >::value >
  struct CheckPredicateTuple
  {
    static const bool value = ( Predicate< typename Dune::tuple_element< N-1, Tuple >::type >::value
                                && CheckPredicateTuple< Tuple, Predicate, (N-1) >::value );
  };

#ifndef DOXYGEN

  template< class Tuple, template< class > class Predicate >
  struct CheckPredicateTuple< Tuple, Predicate, 0 >
  {
    static const bool value = true;
  };

#endif // #ifndef DOXYGEN



#ifndef DOXYGEN

  namespace
  {

    // CheckPredicateTupleHelper
    // -------------------------

    template< class Tuple >
    class CheckPredicateTupleHelper
    {
      template< class Predicate >
      struct CheckPredicateFunctor
      {
        explicit CheckPredicateFunctor ( const Predicate &predicate )
        : v_( true )
        {}

        template< class T >
        void visit ( T &x )
        {
          v_ &= predicate_( x );
        }

        operator bool() const { return v_; }

      private:
        bool v_;
        Predicate predicate_;
      };

      template< class T >
      struct ConstReferenceTypeEvaluator
      {
        typedef const T & Type;

        static Type apply( T &t ) { return t; }
      };

    public:
      template< class Predicate >
      static bool apply ( const Tuple &tuple, const Predicate &predicate = Predicate() )
      {
        Dune::ForEachValue< const Tuple > forEach( tuple );
        CheckPredicateFunctor< Predicate > check( predicate );
        forEach.apply( check );
        return check;
      }
    };

  } // namespace

#endif // #ifndef DOXYGEN



  // check_predicate_tuple
  // ---------------------

  /** \ingroup Tuples_Algorithms
   *
   *  \brief Check a predicate for all tuple elements.
   *
   *  \tparam  Predicate  A predicate
   *  \tparam  Tuple      Some tuple type
   *
   *  The predicate must be a copyable object and is expected to provide
   *  the following method:
   *  @code
   *  struct Predicate
   *  {
   *    template< class T >
   *    bool operator() ( const &T x );
   *  };
   *  @endcode
   *
   *  \returns \b true if predicate applies to all elements in tuple, \b false otherwise
   */
  template< class Predicate, class Tuple >
  bool check_predicate_tuple ( const Tuple &tuple, const Predicate &predicate = Predicate() )
  {
    return CheckPredicateTupleHelper< Tuple >::apply( tuple, predicate );
  }

} // namespace Dune

#endif // #ifndef DUNE_COMMON_TUPLES_CHECKPREDICATE_HH
