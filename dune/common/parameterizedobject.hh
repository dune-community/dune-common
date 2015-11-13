// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
#ifndef PARAMETERIZED_OBJECT_HH
#define PARAMETERIZED_OBJECT_HH

#include <dune/common/std/memory.hh>
#include <dune/common/parametertree.hh>

namespace Dune {


/**
 * @brief A factory class for parameterized objects.
 *
 * It allows the construction of objects adhering to a certain interface that
 * might be constructed quite differently for one another.
 *
 * The Signature parameter defined the "virtual" constructor signature
 * in the form of Interface(Args...), where Interface is the type of
 * the (abstract) interface class and Args... is the set of
 * constrcutor parameters.
 *
 * Each type constructed by this factory is identified by a different key. This class
 * allows for easy registration of type with new keys.
 *
 * @tparam Signature Signature of the "virtual" constructor call in the form for Interface(Args...). For default constructors onecan omit the ()-brackets.
 * @tparam Tag A class tag which allows to have different factories for the same interface [DEFAULT: ParameterizedObjectDefaultTag].
 * @tparam KeyT The type of the objects that are used as keys in the lookup [DEFAULT: std::string].
 */
template<typename Interface,
         typename KeyT,
         typename... Args>
class ParameterizedObjectFactory
{
    public:

        /** @brief The typ of the keys. */
        typedef KeyT Key;
        /** @brief The type of the pointer to the interface. */
        typedef std::unique_ptr<Interface> Type;
        /** @brief The type of the function that creates the object. */
        typedef Type (*Creator)(Args ... );

        /**
         * @brief Creates an object identified by a key from a parameter object.
         * @param key The key the object is registered with @see define.
         * @param d The parameter object used for the construction.
         * @return a shared_pointer to created object.
         */
        Type create(Key const& key, Args ... args) {
            typename Registry::const_iterator i = registry_.find(key);
            if (i == registry_.end()) {
                DUNE_THROW(Dune::InvalidStateException,
                    "ParametrizedObjectFactory: key ``" <<
                    key << "'' not registered");
            }
            else return i->second(args...);
        }

        /**
         * @brief Registers a new type with a key.
         *
         * After registration objects of this type can be constructed with the
         * specified key using the creat function.
         * @tparam Impl The type. It must implement and subclass Interface.
         */
        template<class Impl>
        void define (Key const& key)
        {
            registry_[key] =
                ParameterizedObjectFactory::create_func<Impl>;
        }

    private:

        template<class Impl>
        static
        Type create_func(Args ... args) {
            return Dune::Std::make_unique<Impl>(args...);
        }

        typedef std::map<Key, Creator> Registry;
        Registry registry_;
};



} // end namespace Dune

#endif // PARAMETRIZED_OBJECT_HH
