// bslstl_unorderedset.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDSET
#define INCLUDED_BSLSTL_UNORDEREDSET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant 'unordered_set' container.
//
//@CLASSES:
//   bsl::unordered_set : STL-compliant 'unordered_set' container
//
//@SEE_ALSO: bsl+stdhdrs
//
//@DESCRIPTION: This component defines a single class template 'unordered_set',
// implementing the standard container holding a collection of unique keys with
// no guarantees on ordering.
//
// An instantiation of 'unordered_set' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of keys) and the set of
// keys the 'unordered_set' contains, without regard to their order.  If
// 'unordered_set' is instantiated with a key type that is not itself
// value-semantic, then it will not retain all of its value-semantic qualities.
// In particular, if the key type cannot be tested for equality, then an
// 'unordered_set' containing that type cannot be tested for equality.  It is
// even possible to instantiate 'unordered_set' with a key type that does not
// have an accessible copy-constructor, in which case the 'unordered_set' will
// not be copyable.  Note that the equality operator for each element is used
// to determine when two 'unordered_set' objects have the same value, and not
// the equality comparator supplied at construction.
//
// An 'unordered_set' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [unord].  The
// 'unordered_set' implemented here adheres to the C++11 standard, except that
// it may rehash when setting the 'max_load_factor' in order to preserve the
// property that the value is always respected (which is a potentially throwing
// operation) and it does not have interfaces that take rvalue references,
// 'initializer_list', 'emplace', or operations taking a variadic number of
// template parameters.  Note that excluded C++11 features are those that
// require (or are greatly simplified by) C++11 compiler support.
//
///Requirements on 'KEY'
///---------------------
// An 'unordered_set' instantiation is a fully "Value-Semantic Type" (see
// {'bsldoc_glossary'}) only if the supplied 'KEY' template parameters is
// fully value-semantic.  It is possible to instantiate an 'unordered_set' with
// 'KEY' parameter arguments that do not provide a full set of
// value-semantic operations, but then some methods of the container may not be
// instantiable.  The following terminology, adopted from the C++11 standard,
// is used in the function documentation of 'unordered_set' to describe a
// function's requirements for the 'KEY' template parameter.  These terms are
// also defined in section [utility.arg.requirements] of the C++11 standard.
// Note that, in the context of an 'unordered_set' instantiation, the
// requirements apply specifically to the 'unordered_set's element type,
// 'value_type', which is an alias for 'KEY'.
//
//: "default-constructible":
//:     The type provides an accessible default constructor.
//:
//: "copy-constructible":
//:     The type provides an accessible copy constructor.
//:
//: "equality-comparable":
//:     The type provides an equality-comparison operator that defines an
//:     equivalence relationship and is both reflexive and transitive.
//
///Requirements on 'HASH' and 'EQUAL'
///----------------------------------
// The (template parameter) types 'HASH' and 'EQUAL' must be copy-constructible
// function-objects.  Note that this requirement is somewhat stronger than the
// requirement currently in the standard; see the discussion for Issue 2215
// (http://cplusplus.github.com/LWG/lwg-active.html#2215);
//
// 'HASH' shall support a function call operator compatible with the following
// statements:
//..
//  HASH        hash;
//  KEY         key;
//  std::size_t result = hash(key);
//..
// where the definition of the called function meets the requirements of a
// hash function, as specified in {'bslstl_hash'|Standard Hash Function}.
//
// 'EQUAL' shall support the a function call operator compatible with the
//  following statements:
//..
//  EQUAL equal;
//  KEY   key1, key2;
//  bool  result = equal(key1, key2);
//..
// where the definition of the called function defines an equivalence
// relationship on keys that is both reflexive and transitive.
//
// 'HASH' and 'EQUAL' function-objects are further constrained, such for any
// two objects whose keys compare equal by the comparator, shall produce the
// same value from the hasher.
//
///Memory Allocation
///-----------------
// The type supplied as a set's 'ALLOCATOR' template parameter determines how
// that set will allocate memory.  The 'unordered_set' template supports
// allocators meeting the requirements of the C++11 standard
// [allocator.requirements], and in addition it supports scoped-allocators
// derived from the 'bslma::Allocator' memory allocation protocol.  Clients
// intending to use 'bslma' style allocators should use the template's default
// 'ALLOCATOR' type: The default type for the 'ALLOCATOR' template parameter,
// 'bsl::allocator', provides a C++11 standard-compatible adapter for a
// 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the parameterized 'ALLOCATOR' type of an 'unordered_set' instantiation
// is 'bsl::allocator', then objects of that set type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a set accepts
// an optional 'bslma::Allocator' argument at construction.  If the address of
// a 'bslma::Allocator' object is explicitly supplied at construction, it will
// be used to supply memory for the 'unordered_set' throughout its lifetime;
// otherwise, the 'unordered_set' will use the default allocator installed at
// the time of the 'unordered_set's construction (see 'bslma_default').  In
// addition to directly allocating memory from the indicated
// 'bslma::Allocator', an 'unordered_set' supplies that allocator's address to
// the constructors of contained objects of the parameterized 'KEY' types with
// the 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'unordered_set':
//..
//  Legend
//  ------
//  'K'             - parameterized 'KEY' type of the unordered set
//  'a', 'b'        - two distinct objects of type 'unordered_set<K>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'w'             - number of buckets of 'a'
//  'value_type'    - unoredered_set<K>::value_type
//  'c'             - comparator providing an ordering for objects of type 'K'
//  'al             - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'k'             - an object of type 'K'
//  'v'             - an object of type 'value_type'
//  'p1', 'p2'      - two iterators belonging to 'a'
//  distance(i1,i2) - the number of elements in the range [i1, i2)
//  distance(p1,p2) - the number of elements in the range [p1, p2)
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | unordered_set<K> a;    (default construction)      | O[1]               |
//  | unordered_set<K> a(al);                            |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_set<K> a(b); (copy construction)         | Average: O[n]      |
//  | unordered_set<K> a(b, al);                         | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | unordered_set<K> a(w);                             | O[n]               |
//  | unordered_set<K> a(w, hf);                         |                    |
//  | unordered_set<K> a(w, hf, eq);                     |                    |
//  | unordered_set<K> a(w, hf, eq, al);                 |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_set<K> a(i1, i2);                        | Average: O[N]      |
//  | unordered_set<K> a(i1, i2, w)                      | Worst:   O[N^2]    |
//  | unordered_set<K> a(i1, i2, w, hf);                 | where N =          |
//  | unordered_set<K> a(i1, i2, w, hf, eq);             |  distance(i1, i2)] |
//  | unordered_set<K> a(i1, i2, w, hf, eq, al);         |                    |
//  |                                                    |                    |
//  +----------------------------------------------------+--------------------+
//  | a.~unordered_set<K>(); (destruction)               | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;          (assignment)                       | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | Best:  O[n]        |
//  |                                                    | Worst: O[n^2]      |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a, b)                              | O[1] if 'a' and    |
//  |                                                    | 'b' use the same   |
//  |                                                    | allocator,         |
//  |                                                    | O[n + m] otherwise |
//  +----------------------------------------------------+--------------------+
//  | a.key_eq()                                         | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.hash_function()                                  | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.get_allocator()                                  | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(v))                                       | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, v))                                   | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.insert(i1, i2)                                   | Average O[         |
//  |                                                    |   distance(i1, i2)]|
//  |                                                    | Worst:  O[ n *     |
//  |                                                    |   distance(i1, i2)]|
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.erase(k)                                         | Average: O[        |
//  |                                                    |         a.count(k)]|
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | Average: O[        |
//  |                                                    |   distance(p1, p2)]|
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.find(k)                                          | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.count(k)                                         | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.equal_range(k)                                   | Average: O[        |
//  |                                                    |         a.count(k)]|
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.bucket_count()                                   | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_bucket_count()                               | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.bucket(k)                                        | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.bucket_size(k)                                   | O[a.bucket_size(k)]|
//  +----------------------------------------------------+--------------------+
//  | a.load_factor()                                    | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_load_factor()                                | O[1]               |
//  | a.max_load_factor(z)                               | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.rehash(k)                                        | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a.resize(k)                                        | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//..
//
///Unordered Set Configuration
///---------------------------
// The unordered set has interfaces that can provide insight into and control
// of its inner workings.  The syntax and semantics of these interfaces for
// 'bslstl_unoroderedset' are identical to those of 'bslstl_unorderedmap'.  See
// the discussion in {'bslstl_unorderedmap'|Unordered Map Configuration} and
// the illustrative material in {'bslstl_unorderedmap'|Example 2}.
//
///Practical Requirements on 'HASH'
///--------------------------------
// An important factor in the performance an unordered set (and any of the
// other unordered containers) is the choice of hash function.  Please see the
// discussion in {'bslstl_unorderedmap'|Practical Requirements on 'HASH'}.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Categorizing Data
/// - - - - - - - - - - - - - -
// Unordered set are useful in situations when there is no meaningful way to
// order key values, when the order of the values is irrelevant to the problem
// domain, and (even if there is a meaningful ordering) the value of ordering
// the results is outweighed by the higher performance provided by unordered
// sets (compared to ordered sets).
//
// Suppose one is analyzing data on a set of customers, and each customer is
// categorized by several attributes: customer type, geographic area, and
// (internal) project code; and that each attribute takes on one of a limited
// set of values.  This data can be handled by creating an enumeration for each
// of the attributes:
//..
//  typedef enum {
//      REPEAT
//    , DISCOUNT
//    , IMPULSE
//    , NEED_BASED
//    , BUSINESS
//    , NON_PROFIT
//    , INSTITUTE
//      // ...
//  } CustomerCode;
//
//  typedef enum {
//      USA_EAST
//    , USA_WEST
//    , CANADA
//    , MEXICO
//    , ENGLAND
//    , SCOTLAND
//    , FRANCE
//    , GERMANY
//    , RUSSIA
//      // ...
//  } LocationCode;
//
//  typedef enum {
//      TOAST
//    , GREEN
//    , FAST
//    , TIDY
//    , PEARL
//    , SMITH
//      // ...
//  } ProjectCode;
//..
// For printing these values in a human-readable form, we define these helper
// functions:
//..
//  static const char *toAscii(CustomerCode value)
//  {
//      switch (value) {
//        case REPEAT:     return "REPEAT";
//        case DISCOUNT:   return "DISCOUNT";
//        case IMPULSE:    return "IMPULSE";
//        case NEED_BASED: return "NEED_BASED";
//        case BUSINESS:   return "BUSINESS";
//        case NON_PROFIT: return "NON_PROFIT";
//        case INSTITUTE:  return "INSTITUTE";
//        // ...
//        default: return "(* UNKNOWN *)";
//      }
//  }
//
//  static const char *toAscii(LocationCode value)
//  {
//      ...
//  }
//
//  static const char *toAscii(ProjectCode  value)
//  {
//      ...
//  }
//..
// The data set (randomly generated for this example) is provided in a
// statically initialized array:
//..
//  static const struct CustomerProfile {
//      CustomerCode d_customer;
//      LocationCode d_location;
//      ProjectCode  d_project;
//  } customerProfiles[] = {
//      { IMPULSE   , CANADA  , SMITH },
//      { NON_PROFIT, USA_EAST, GREEN },
//      ...
//      { INSTITUTE , USA_EAST, TOAST },
//      { NON_PROFIT, ENGLAND , FAST  },
//      { NON_PROFIT, USA_WEST, TIDY  },
//      { REPEAT    , MEXICO  , TOAST },
//  };
//  const int numCustomerProfiles = sizeof  customerProfiles
//                                / sizeof *customerProfiles;
//..
// Suppose, as the first step in analysis, we wish to determine the number of
// unique combinations of customer attributes that exist in our data set.  We
// can do that by inserting each data item into an (unordered) set: the first
// insert of a combination will succeed, the others will fail, but at the end
// of the process, the set will contain one entry for every unique combination
// in our data.
//
// First, as there are no standard methods for hashing or comparing our user
// defined types, we define 'CustomerProfileHash' and 'CustomerProfileEqual'
// classes, each a stateless functor.  Note that there is no meaningful
// ordering of the attribute values, they are merely arbitrary code numbers;
// nothing is lost by using an unordered set instead of an ordered set:
//..
//  class CustomerProfileHash
//  {
//    public:
//      // CREATORS
//      //! CustomerProfileHash() = default;
//          // Create a 'CustomerProfileHash' object.
//
//      //! CustomerProfileHash(const CustomerProfileHash& original) = default;
//          // Create a 'CustomerProfileHash' object.  Note that as
//          // 'CustomerProfileHash' is an empty (stateless) type, this
//          // operation will have no observable effect.
//
//      //! ~CustomerProfileHash() = default;
//          // Destroy this object.
//
//      // ACCESSORS
//      std::size_t operator()(CustomerProfile x) const;
//          // Return a hash value computed using the specified 'x'.
//  };
//..
// The hash function combines the several enumerated values from the class
// (each a small 'int' value) into a single, unique 'int' value, and then
// applying the default hash function for 'int'.  See {Practical Requirements
// on 'HASH'}.
//..
//  // ACCESSORS
//  std::size_t CustomerProfileHash::operator()(CustomerProfile x) const
//  {
//      return bsl::hash<int>()(x.d_location * 100 * 100
//                            + x.d_customer * 100
//                            + x.d_project);
//  }
//
//  class CustomerProfileEqual
//  {
//    public:
//      // CREATORS
//      //! CustomerProfileEqual() = default;
//          // Create a 'CustomerProfileEqual' object.
//
//      //! CustomerProfileEqual(const CustomerProfileEqual& original)
//      //!                                                          = default;
//          // Create a 'CustomerProfileEqual' object.  Note that as
//          // 'CustomerProfileEqual' is an empty (stateless) type, this
//          // operation will have no observable effect.
//
//      //! ~CustomerProfileEqual() = default;
//          // Destroy this object.
//
//      // ACCESSORS
//      bool operator()(const CustomerProfile& lhs,
//                      const CustomerProfile& rhs) const;
//          // Return 'true' if the specified 'lhs' have the same value as the
//          // specified 'rhs', and 'false' otherwise.
//  };
//
//  // ACCESSORS
//  bool CustomerProfileEqual::operator()(const CustomerProfile& lhs,
//                                        const CustomerProfile& rhs) const
//  {
//      return lhs.d_location == rhs.d_location
//          && lhs.d_customer == rhs.d_customer
//          && lhs.d_project  == rhs.d_project;
//  }
//..
// Notice that many of the required methods of the hash and comparator types
// are compiler generated.  (The declaration of those methods are commented out
// and suffixed by an '= default' comment.)
//
// Then, we define the type of the unordered set and a convenience aliases:
//..
//  typedef bsl::unordered_set<CustomerProfile,
//                             CustomerProfileHash,
//                             CustomerProfileEqual> ProfileCategories;
//  typedef ProfileCategories::const_iterator        ProfileCategoriesConstItr;
//..
// Next, we create an unordered set and insert each item of 'data'.
//..
//  ProfileCategories profileCategories;
//
//  for (int idx = 0; idx < numCustomerProfiles; ++idx) {
//     profileCategories.insert(customerProfiles[idx]);
//  }
//
//  assert(numCustomerProfiles >= profileCategories.size());
//..
// Notice that we ignore the status returned by the 'insert' method.  We fully
// expect some operations to fail.
//
// Now, the size of 'profileCategories' matches the number of unique customer
// profiles in this data set.
//..
//  printf("%d %d\n", numCustomerProfiles, profileCategories.size());
//..
// Standard output shows:
//..
//  100 84
//..
// Finally, we can examine the unique set by iterating through the unordered
// set and printing each element.  Note the use of the several 'toAscii'
// functions defined earlier to make the output comprehensible:
//..
//  for (ProfileCategoriesConstItr itr  = profileCategories.begin(),
//                                 end  = profileCategories.end();
//                                 end != itr; ++itr) {
//      printf("%-10s %-8s %-5s\n",
//             toAscii(itr->d_customer),
//             toAscii(itr->d_location),
//             toAscii(itr->d_project));
//  }
//..
// We find on standard output:
//..
//  NON_PROFIT ENGLAND  FAST
//  DISCOUNT   CANADA   TIDY
//  IMPULSE    USA_WEST GREEN
//  ...
//  DISCOUNT   USA_EAST GREEN
//  DISCOUNT   MEXICO   SMITH
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_unorderedset.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>  // Can probably escape with a fwd-decl, but not
#endif                         // very user friendly

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLSTL_EQUALTO
#include <bslstl_equalto.h>
#endif

#ifndef INCLUDED_BSLSTL_HASH
#include <bslstl_hash.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLE
#include <bslstl_hashtable.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLEBUCKETITERATOR
#include <bslstl_hashtablebucketiterator.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLEITERATOR
#include <bslstl_hashtableiterator.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATORUTIL
#include <bslstl_iteratorutil.h>
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>  // result type of 'equal_range' method
#endif

#ifndef INCLUDED_BSLSTL_UNORDEREDSETKEYCONFIGURATION
#include <bslstl_unorderedsetkeyconfiguration.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace bsl {

                        // ===================
                        // class unordered_set
                        // ===================

template <class KEY,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY>,
          class ALLOCATOR = bsl::allocator<KEY> >
class unordered_set
{
    // This class template implements a value-semantic container type holding
    // an unordered set of unique values (of template parameter type 'KEY').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

  private:

    // PRIVATE TYPES
    typedef bsl::allocator_traits<ALLOCATOR> AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    typedef KEY ValueType;
        // This typedef is an alias for the type of values maintained by this
        // set.

    typedef ::BloombergLP::bslstl::UnorderedSetKeyConfiguration<ValueType>
                                                             ListConfiguration;
        // This typedef is an alias for the policy used internally by this
        // container to extract the 'KEY' value from the values maintained by
        // this set.

    typedef ::BloombergLP::bslstl::HashTable<ListConfiguration,
                                             HASH,
                                             EQUAL,
                                             ALLOCATOR> HashTable;
        // This typedef is an alias for the template instantiation of the
        // underlying 'bslstl::HashTable' used to implement this set.

    typedef ::BloombergLP::bslalg::BidirectionalLink HashTableLink;
        // This typedef is an alias for the type of links maintained by the
        // linked list of elements held by the underlying 'bslstl::HashTable'.

    // FRIEND
    template <class KEY2,
              class HASH2,
              class EQUAL2,
              class ALLOCATOR2>
    friend bool operator==(
                const unordered_set<KEY2, HASH2, EQUAL2, ALLOCATOR2>&,
                const unordered_set<KEY2, HASH2, EQUAL2, ALLOCATOR2>&);

  public:
    // PUBLIC TYPES
    typedef KEY                                        key_type;
    typedef KEY                                        value_type;
    typedef HASH                                       hasher;
    typedef EQUAL                                      key_equal;
    typedef ALLOCATOR                                  allocator_type;

    typedef typename allocator_type::reference         reference;
    typedef typename allocator_type::const_reference   const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                    unordered_set,
                    ::BloombergLP::bslmf::IsBitwiseMoveable,
                    ::BloombergLP::bslmf::IsBitwiseMoveable<HashTable>::value);

    typedef ::BloombergLP::bslstl::HashTableIterator<
                                   const value_type, difference_type> iterator;
    typedef ::BloombergLP::bslstl::HashTableBucketIterator<
                             const value_type, difference_type> local_iterator;

    typedef iterator                                            const_iterator;
    typedef local_iterator                                const_local_iterator;

  private:
    // DATA
    HashTable  d_impl;

  public:
    // CREATORS
    explicit unordered_set(size_type        initialNumBuckets = 0,
                      const hasher&         hashFunction = hasher(),
                      const key_equal&      keyEqual = key_equal(),
                      const allocator_type& basicAllocator = allocator_type());
        // Construct an empty unordered set.  Optionally specify an
        // 'initialNumBuckets' indicating the initial size of the array of
        // buckets of this container.  If 'initialNumBuckets' is not supplied,
        // an implementation defined value is used.  Optionally specify a
        // 'hashFunction' used to generate the hash values associated to the
        // keys extracted from the values contained in this object.  If
        // 'hashFunction' is not supplied, a default-constructed object of type
        // 'hasher' is used.  Optionally specify a key-equality functor
        // 'keyEqual' used to verify that two key values are the same.  If
        // 'keyEqual' is not supplied, a default-constructed object of type
        // 'key_equal' is used.  Optionally specify the 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator will be used to supply memory.

    explicit unordered_set(const allocator_type& basicAllocator);
        // Construct an empty unordered set that uses the specified
        // 'basicAllocator' to supply memory.  Use a default-constructed object
        // of type 'hasher' to generate hash values for the key extracted from
        // the values contained in this object.  Also, use a
        // default-constructed object of type 'key_equal' to verify that two
        // key values are the same.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.

    unordered_set(const unordered_set&  original);
    unordered_set(const unordered_set&  original,
                  const allocator_type& basicAllocator);
        // Construct an unordered set having the same value as that of the
        // specified 'original'.  Use a default-constructed object of type
        // 'hasher' to generate hash values for the key extracted from the
        // values contained in this object.  Also, use a default-constructed
        // object of type 'key_equal' to verify that two key values are the
        // same.  Optionally specify the 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not supplied, a default-constructed
        // object of type 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.

    template <class INPUT_ITERATOR>
    unordered_set(INPUT_ITERATOR        first,
                  INPUT_ITERATOR        last,
                  size_type             initialNumBuckets = 0,
                  const hasher&         hashFunction = hasher(),
                  const key_equal&      keyEqual = key_equal(),
                  const allocator_type& basicAllocator = allocator_type());
        // Construct an empty unordered set and insert each 'value_type' object
        // in the sequence starting at the specified 'first' element, and
        // ending immediately before the specified 'last' element, ignoring
        // those pairs having a key that appears earlier in the sequence.
        // Optionally specify an 'initialNumBuckets' indicating the initial
        // size of the array of buckets of this container.  If
        // 'initialNumBuckets' is not supplied, an implementation defined value
        // is used.  Optionally specify a 'hashFunction' used to generate hash
        // values for the keys extracted from the values contained in this
        // object.  If 'hashFunction' is not supplied, a default-constructed
        // object of type 'hasher' is used.  Optionally specify a key-equality
        // functor 'keyEqual' used to verify that two key values are the same.
        // If 'keyEqual' is not supplied, a default-constructed object of type
        // 'key_equal' is used.  Optionally specify the 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.  If the 'allocator_type' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator will be used to supply memory.  The
        // (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type'.  The behavior is undefined unless 'first' and 'last'
        // refer to a sequence of valid values where 'first' is at a position
        // at or before 'last'.  This method requires that the (template
        // parameter) type 'KEY' be "copy-constructible" (see {Requirements on
        // 'KEY'}).

    ~unordered_set();
        // Destroy this object.

    // MANIPULATORS
    unordered_set& operator=(const unordered_set& rhs);
        // Assign to this object the value, hasher, and key-equality functor of
        // the specified 'rhs' object, propagate to this object the allocator
        // of 'rhs' if the 'ALLOCATOR' type has trait
        // 'propagate_on_container_copy_assignment', and return a reference
        // providing modifiable access to this object.  This method requires
        // that the (template parameter) type 'KEY' be "copy-constructible"
        // (see {Requirements on 'KEY'}).

    iterator begin();
        // Return an iterator providing modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this set, or the 'end' iterator if this set is empty.

    iterator end();
        // Return an iterator providing modifiable access to the past-the-end
        // element in the sequence of 'value_type' objects maintained by this
        // set.

    local_iterator begin(size_type index);
        // Return a local iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects of the
        // bucket having the specified 'index', in the array of buckets
        // maintained by this set, or the 'end(index)' otherwise.

    local_iterator end(size_type index);
        // Return a local iterator providing modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects of the
        // bucket having the specified 'index's, in the array of buckets
        // maintained by this set.

    void clear();
        // Remove all entries from this set.  Note that the container is empty
        // after this call, but allocated memory may be retained for future
        // use.

    pair<iterator, iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this unordered set having the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  If this unordered set contains no 'value_type'
        // objects having 'key', then the two returned iterators will have the
        // same value.  Note that since a set maintains unique keys, the range
        // will contain at most one element.

    size_type erase(const key_type& key);
        // Remove from this set the 'value_type' object having the specified
        // 'key', if it exists, and return 1; otherwise, if there is no
        // 'value_type' object having 'key', return 0 with no other effect.

    iterator erase(const_iterator position);
        // Remove from this unordered set the 'value_type' object at the
        // specified 'position', and return an iterator referring to the
        // element immediately following the removed element, or to the
        // past-the-end position if the removed element was the last element in
        // the sequence of elements maintained by this set.  The behavior is
        // undefined unless 'position' refers to a 'value_type' object in this
        // unordered set.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this set the 'value_type' objects starting at the
        // specified 'first' position up to, but including the specified 'last'
        // position, and return 'last'.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this set or are the
        // 'end' iterator, and the 'first' position is at or before the 'last'
        // position in the ordered sequence provided by this container.

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this set having the specified 'key', if such an entry
        // exists, and the past-the-end ('end') iterator otherwise.

    pair<iterator, bool> insert(const value_type& value);
        // Insert the specified 'value' into this set if the key (the 'first'
        // element) of the 'value' does not already exist in this set;
        // otherwise, if a 'value_type' object having the same key (according
        // to 'key_equal') as 'value' already exists in this set, this method
        // has no effect.  Return a pair whose 'first' member is an iterator
        // referring to the (possibly newly inserted) 'value_type' object in
        // this set whose key is the same as that of 'value', and whose
        // 'second' member is 'true' if a new value was inserted, and 'false'
        // if the value was already present.  This method requires that the
        // (template parameter) type 'KEY' be "copy-constructible" (see
        // {Requirements on 'KEY'}).

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this set (in constant time if the
        // specified 'hint' is a valid element in the bucket to which 'value'
        // belongs), if the key ('value' itself) of the 'value' does not
        // already exist in this set; otherwise, if a 'value_type' object
        // having the same key (according to 'key_equal') as 'value' already
        // exists in this set, this method has no effect.  Return an iterator
        // referring to the (possibly newly inserted) 'value_type' object in
        // this set whose key is the same as that of 'value'.  If 'hint' is not
        // a valid immediate successor to the key of 'value', this operation
        // will have worst case O[N] and average case constant time complexity,
        // where 'N' is the size of this set.  The behavior is undefined unless
        // 'hint' is a valid iterator into this unordered set.  This method
        // requires that the (template parameter) type 'KEY' be
        // "copy-constructible" (see {Requirements on 'KEY'}).

    template <class INPUT_ITERATOR> void insert(INPUT_ITERATOR first,
            INPUT_ITERATOR last);
        // Insert into this set the value of each 'value_type' object in the
        // range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator, whose key is not
        // already contained in this set.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type'.  This method requires that the
        // (template parameter) type 'KEY' be "copy-constructible" (see
        // {Requirements on 'KEY'}).

    void max_load_factor(float newLoadFactor);
        // Set the maximum load factor of this container to the specified
        // 'newLoadFactor'.

    void rehash(size_type numBuckets);
        // Change the size of the array of buckets maintained by this container
        // to the specified 'numBuckets', and redistribute all the contained
        // elements into the new sequence of buckets, according to their hash
        // values.  Note that this operation has no effect if rehashing the
        // elements into 'numBuckets' would cause this set to exceed its
        // 'max_load_factor'.

    void reserve(size_type numElements);
        // Increase the number of buckets of this set to a quantity such that
        // the ratio between the specified 'numElements' and this quantity does
        // not exceed 'max_load_factor', and allocate footprint memory
        // sufficient to grow the table to contain 'numElements' elements.
        // Note that this guarantees that, after the reserve, elements can be
        // inserted to grow the container to 'size() == numElements' without
        // any further allocation, unless the 'KEY' type itself or the hash
        // function allocate memory.  Also note that this operation has no
        // effect if 'numElements <= size()'.

    void swap(unordered_set& other);
        // Exchange the value of this object as well as its hasher and
        // key-equality functor with those of the specified 'other' object.
        // Additionally if
        // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true' then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee and
        // guarantees O[1] complexity.  The behavior is undefined is unless
        // either this object was created with the same allocator as 'other' or
        // 'propagate_on_container_swap' is 'true'.

    // ACCESSORS
    const_iterator begin() const; const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this set, or the 'end' iterator if this set is empty.

    const_iterator end() const;
    const_iterator cend() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects)
        // maintained by this set.

    const_local_iterator begin(size_type index) const;
    const_local_iterator cbegin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects) of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this set, or the 'end(index)' otherwise.

    const_local_iterator end(size_type index) const;
    const_local_iterator cend(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects) of
        // the bucket having the specified 'index' in the array of buckets
        // maintained by this set.

    size_type bucket(const key_type& key) const;
        // Return the index of the bucket, in the array of buckets of this
        // container, where values having the specified 'key' would be
        // inserted.

    size_type bucket_count() const;
        // Return the number of buckets in the array of buckets maintained by
        // this set.

    size_type bucket_size(size_type index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this
        // container.

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects within this map having the
        // specified 'key'.  Note that since an unordered set maintains unique
        // keys, the returned value will be either 0 or 1.

    bool empty() const;
        // Return 'true' if this set contains no elements, and 'false'
        // otherwise.

    pair<const_iterator, const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this container having the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence and the second iterator is positioned one past
        // the end of the sequence.  If this set contains no 'value_type'
        // objects having 'key' then the two returned iterators will have the
        // same value.  Note that since a set maintains unique keys, the range
        // will contain at most one element.

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this set having the specified 'key', if such
        // an entry exists, and the past-the-end ('end') iterator otherwise.

    allocator_type get_allocator() const;
        // Return (a copy of) the allocator used for memory allocation by this
        // set.

    key_equal key_eq() const;
        // Return (a copy of) the key-equality binary functor that returns
        // 'true' if the value of two 'key_type' objects is the same, and
        // 'false' otherwise.

    hasher hash_function() const;
        // Return (a copy of) the hash unary functor used by this set to
        // generate a hash value (of type 'size_t') for a 'key_type' object.

    float load_factor() const;
        // Return the current ratio between the 'size' of this container and
        // the number of buckets.  The 'load_factor' is a measure of how full
        // the container is, and a higher load factor leads to an increased
        // number of collisions, thus resulting in a loss performance.

    size_type max_bucket_count() const;
        // Return a theoretical upper bound on the largest number of buckets
        // that this container could possibly manage.  Note that there is no
        // guarantee that the set can successfully grow to the returned size,
        // or even close to that size without running out of resources.

    float max_load_factor() const;
        // Return the maximum load factor allowed for this container.  If an
        // insert operation would cause 'load_factor' to exceed the
        // 'max_load_factor', that same insert operation will increase the
        // number of buckets and rehash the elements of the container into
        // those buckets the (see rehash).

    size_type max_size() const;
        // Return a theoretical upper bound on the largest number of elements
        // that this set could possibly hold.  Note that there is no guarantee
        // that the set can successfully grow to the returned size, or even
        // close to that size without running out of resources.

    size_type size() const;
        // Return the number of elements in this set.
};

// FREE FUNCTIONS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
bool operator==(const unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'unordered_set' objects have the same
    // value if they have the same number of value-elements, and for each
    // value-element that is contained in 'lhs' there is a value-element
    // contained in 'rhs' having the same value, and vice-versa.  This method
    // requires that the (template parameter) type 'KEY' be
    // "equality-comparable" (see {Requirements on 'KEY'}).


template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(const unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'unordered_set' objects do not
    // have the same value if they do not have the same number of
    // value-elements, or that for some value-element contained in 'lhs' there
    // is not a value-element in 'rhs' having the same value, and vice-versa.
    // This method requires that the (template parameter) type 'KEY' and be
    // "equality-comparable" (see {Requirements on 'KEY'}).


template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
void swap(unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& x,
          unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& y);
    // Swap both the value and the comparator of the specified 'a' object with
    // the value and comparator of the specified 'b' object.  Additionally if
    // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true' then exchange the allocator of 'a' with that of 'b', and do not
    // modify either allocator otherwise.  This method provides the no-throw
    // exception-safety guarantee and guarantees O[1] complexity.  The behavior
    // is undefined is unless either this object was created with the same
    // allocator as 'other' or 'propagate_on_container_swap' is 'true'.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //--------------------
                        // class unordered_set
                        //--------------------

// CREATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                       size_type             initialNumBuckets,
                                       const hasher&         hashFunction,
                                       const key_equal&      keyEqual,
                                       const allocator_type& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                       INPUT_ITERATOR        first,
                                       INPUT_ITERATOR        last,
                                       size_type             initialNumBuckets,
                                       const hasher&         hashFunction,
                                       const key_equal&      keyEqual,
                                       const allocator_type& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                          const allocator_type& basicAllocator)
: d_impl(basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                          const unordered_set&  original,
                                          const allocator_type& basicAllocator)
: d_impl(original.d_impl, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::unordered_set(
                                                 const unordered_set& original)
: d_impl(original.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                     original.get_allocator()))
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::~unordered_set()
{
    // All memory management is handled by the base 'd_impl' member.
}

// MANIPULATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>&
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::operator=(const unordered_set& rhs)
{
    unordered_set(rhs, get_allocator()).swap(*this);
    return *this;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::begin()
{
    return iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::end()
{
    return iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::begin(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::end(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::clear()
{
    d_impl.removeAll();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bsl::pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator,
          typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::equal_range(const key_type& key)
{
    typedef bsl::pair<iterator, iterator> ResultType;

    iterator first  = this->find(key);
    if (first == this->end()) {
        return ResultType(first, first);                              // RETURN
    }
    else {
        iterator next = first;
        return ResultType(first, ++next);                             // RETURN
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT(position != this->end());

    return iterator(d_impl.remove(position.node()));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::erase(const key_type& key)
{
    if (HashTableLink *target = d_impl.find(key)) {
        d_impl.remove(target);
        return 1;                                                     // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::erase(const_iterator first,
                                                  const_iterator last)
{
#if defined BDE_BUILD_TARGET_SAFE_2
    if (first != last) {
        iterator it        = this->begin();
        const iterator end = this->end();
        for (; it != first; ++it) {
            BSLS_ASSERT(last != it);
            BSLS_ASSERT(end  != it);
        }
        for (; it != last; ++it) {
            BSLS_ASSERT(end  != it);
        }
    }
#endif

    while (first != last) {
        first = this->erase(first);
    }

    return iterator(first.node());          // convert from const_iterator
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::find(const key_type& key)
{
    return iterator(d_impl.find(key));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bsl::pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator, bool>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::insert(const value_type& value)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;

    HashTableLink *result = d_impl.insertIfMissing(&isInsertedFlag, value);

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::insert(const_iterator,
                                                   const value_type& value)
{
    // There is no realistic use-case for the 'hint' in an unordered_set of
    // unique values.  We could quickly test for a duplicate key, and have a
    // fast return path for when the method fails, but in the typical use case
    // where a new element is inserted, we are adding an extra key-check for no
    // benefit.  In order to insert an element into a bucket, we need to walk
    // the whole bucket looking for duplicates, and the hint is no help in
    // finding the start of a bucket.

    return this->insert(value).first;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::insert(INPUT_ITERATOR first,
                                                        INPUT_ITERATOR last)
{
    if (size_type maxInsertions =
            ::BloombergLP::bslstl::IteratorUtil::insertDistance(first, last)) {
        this->reserve(this->size() + maxInsertions);
    }

    bool isInsertedFlag;  // value is not used

    while (first != last) {
        d_impl.insertIfMissing(&isInsertedFlag, *first);
        ++first;
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::max_load_factor(
                                                           float newLoadFactor)
{
    d_impl.setMaxLoadFactor(newLoadFactor);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::rehash(size_type numBuckets)
{
    d_impl.rehashForNumBuckets(numBuckets);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::reserve(size_type numElements)
{
    d_impl.reserveForNumElements(numElements);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::swap(unordered_set& other)
{
    BSLS_ASSERT_SAFE(this->get_allocator() == other.get_allocator());

    d_impl.swap(other.d_impl);
}

// ACCESSORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::begin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::end() const
{
    return const_iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::begin(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::end(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::cbegin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::cend() const
{
    return const_iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::bucket(const key_type& key) const
{
    BSLS_ASSERT_SAFE(this->bucket_count() > 0);

    return d_impl.bucketIndexForKey(key);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::bucket_count() const
{
    return d_impl.numBuckets();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::count(const key_type& key) const
{
    return 0 != d_impl.find(key);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::empty() const
{
    return 0 == d_impl.size();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::find(const key_type& key) const
{
    return const_iterator(d_impl.find(key));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bsl::pair<typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator,
          typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator>
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                     const key_type& key) const
{
    typedef bsl::pair<const_iterator, const_iterator> ResultType;

    const_iterator first = this->find(key);
    if (first == this->end()) {
        return ResultType(first, first);                              // RETURN
    }
    else {
        const_iterator next = first;
        return ResultType(first, ++next);                             // RETURN
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
ALLOCATOR unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::get_allocator() const
{
    return d_impl.allocator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::hasher
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::key_equal
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::key_eq() const
{
    return d_impl.comparator();
}


template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::bucket_size(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return d_impl.countElementsInBucket(index);
}


template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::cbegin(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::cend(size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::load_factor() const
{
    return d_impl.loadFactor();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::max_bucket_count() const
{
    return d_impl.maxNumBuckets();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::max_load_factor() const
{
    return d_impl.maxLoadFactor();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size() const
{
    return d_impl.size();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_set<KEY, HASH, EQUAL, ALLOCATOR>::max_size() const
{
    return AllocatorTraits::max_size(get_allocator());
}

}  // close namespace bsl

// FREE FUNCTIONS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator==(
                    const bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                    const bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return lhs.d_impl == rhs.d_impl;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator!=(
                    const bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                    const bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void bsl::swap(bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& x,
               bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR>& y)
{
    x.swap(y);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *unordered* *associative* containers:
//: o An unordered associative container defines STL iterators.
//: o An unordered associative container is bitwise moveable if the both
//:      functors and the allocator are bitwise moveable.
//: o An unordered associative container uses 'bslma' allocators if the
//:      parameterized 'ALLOCATOR' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
struct HasStlIterators<bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR> >
     : bsl::true_type
{};

}  // close package namespace

namespace bslma {

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::unordered_set<KEY, HASH, EQUAL, ALLOCATOR> >
     : bsl::is_convertible<Allocator*, ALLOCATOR>::type
{};

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
