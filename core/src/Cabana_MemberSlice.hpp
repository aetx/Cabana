#ifndef CABANA_MEMBERSLICE_HPP
#define CABANA_MEMBERSLICE_HPP

#include <Cabana_AoSoA.hpp>

#include <Kokkos_Core.hpp>

#include <cstdlib>
#include <type_traits>

namespace Cabana
{
//---------------------------------------------------------------------------//
/*!
  \class MemberSlice

  \brief A slice of an array-of-structs-of-arrays with data access to a single
  member.

  A slice provides a simple wrapper around a single data member of an
  AoSoA. This does a few convenient things. First, it decouples access of the
  member from the AoSoA meaning that functionality can be implemented using
  multiple slices from potentially multiple AoSoA containers. Second, it
  eliminates the member index template parameter from the AoSoA get function,
  instead giving an operator() syntax for accessing the member data.

  Construction of slices is recommended using the free helper function as:

  \code
  auto slice = Cabana::slice<M>(aosoa);
  \endcode
*/
template<std::size_t M, typename AoSoA_t>
class MemberSlice;

//---------------------------------------------------------------------------//
// Static type checker.
template<typename >
struct is_member_slice
    : public std::false_type {};

// True only if the type is a member slice *AND* the member slice is templated
// on an AoSoA type.
template<std::size_t M, typename AoSoA_t>
struct is_member_slice<MemberSlice<M,AoSoA_t> >
    : public is_aosoa<AoSoA_t>::type {};

template<std::size_t M, typename AoSoA_t>
struct is_member_slice<const MemberSlice<M,AoSoA_t> >
    : public is_aosoa<AoSoA_t>::type {};

//---------------------------------------------------------------------------//
template<std::size_t M, typename AoSoA_t>
class MemberSlice
{
  public:

    // Slice type.
    using slice_type = MemberSlice<M,AoSoA_t>;

    // Member id.
    static constexpr std::size_t member_id = M;

    // AoSoA type this slice wraps.
    using aosoa_type = AoSoA_t;

    // Inner array size.
    static constexpr int array_size = aosoa_type::array_size;

    // Struct member array return type at a given index M.
    using array_type =
        typename aosoa_type::template struct_member_array_type<M>;

    // Struct member array data type at a given index M.
    using data_type =
        typename aosoa_type::template struct_member_data_type<M>;

    // Struct member array element value type at a given index M.
    using value_type =
        typename aosoa_type::template struct_member_value_type<M>;

    // Struct member array element reference type at a given index M.
    using reference_type =
        typename aosoa_type::template struct_member_reference_type<M>;

    // Struct member array element pointer type at a given index M.
    using pointer_type =
        typename aosoa_type::template struct_member_pointer_type<M>;

  public:

    /*!
      \brief Constructor.

      \param aosoa The AoSoA to slice at the member index M.
    */
    MemberSlice( aosoa_type aosoa )
        : _aosoa( aosoa )
    {
        static_assert( is_aosoa<aosoa_type>::value,
                       "MemberSlice can only wrap AoSoA objects!" );
        static_assert( M < aosoa_type::number_of_members,
                       "MemberSlice index is out of range!" );
    }

    /*!
      \brief Returns the number of elements in the container.

      \return The number of elements in the container.
    */
    KOKKOS_FUNCTION
    int size() const { return _aosoa.size(); }

    /*!
      \brief Get the number of structs-of-arrays in the container.

      \return The number of structs-of-arrays in the container.
    */
    KOKKOS_FUNCTION
    int numSoA() const { return _aosoa.numSoA(); }

    /*!
      \brief Get the size of the data array at a given struct member index.

      \param s The struct index to get the array size for.

      \return The size of the array at the given struct index.
    */
    KOKKOS_FUNCTION
    int arraySize( const int s ) const
    { return _aosoa.arraySize(s); }

    // -------------------------------
    // Member data type properties.

    /*!
      \brief Get the rank of the data for this member.

      \return The rank of the data for this member.
    */
    KOKKOS_INLINE_FUNCTION
    int rank() const
    { return _aosoa.rank(M); }

    /*!
      \brief Get the extent of a given member data dimension.

      \param D The member data dimension to get the extent for.

      \return The extent of the given member data dimension.
    */
    KOKKOS_INLINE_FUNCTION
    int extent( const std::size_t D ) const
    { return _aosoa.extent(M,D); }

    // -------------------------------
    // Access the data value at a given struct index and array index

    // Rank 0
    template<std::size_t J = M>
    KOKKOS_INLINE_FUNCTION
    typename std::enable_if<
        (0==std::rank<
         typename aosoa_type::template struct_member_data_type<J> >::value),
        reference_type>::type
    operator()( const int particle_index ) const
    {
        static_assert( J == M,
                       "Do not call with different template arguments!" );
        return _aosoa.template get<J>(particle_index);
    }

    // Rank 1
    template<std::size_t J = M>
    KOKKOS_INLINE_FUNCTION
    typename std::enable_if<
        (1==std::rank<
         typename aosoa_type::template struct_member_data_type<J> >::value),
        reference_type>::type
    operator()( const int particle_index,
                const int d0 ) const
    {
        static_assert( J == M,
                       "Do not call with different template arguments!" );
        return _aosoa.template get<J>(particle_index,d0);
    }

    // Rank 2
    template<std::size_t J = M>
    KOKKOS_INLINE_FUNCTION
    typename std::enable_if<
        (2==std::rank<
         typename aosoa_type::template struct_member_data_type<J> >::value),
        reference_type>::type
    operator()( const int particle_index,
                const int d0,
                const int d1 ) const
    {
        static_assert( J == M,
                       "Do not call with different template arguments!" );
        return _aosoa.template get<J>(particle_index,d0,d1);
    }

    // Rank 3
    template<std::size_t J = M>
    KOKKOS_INLINE_FUNCTION
    typename std::enable_if<
        (3==std::rank<
         typename aosoa_type::template struct_member_data_type<J> >::value),
        reference_type>::type
    operator()( const int particle_index,
                const int d0,
                const int d1,
                const int d2 ) const
    {
        static_assert( J == M,
                       "Do not call with different template arguments!" );
        return _aosoa.template get<J>(particle_index,d0,d1,d2);
    }

    // Rank 4
    template<std::size_t J = M>
    KOKKOS_INLINE_FUNCTION
    typename std::enable_if<
        (4==std::rank<
         typename aosoa_type::template struct_member_data_type<J> >::value),
        reference_type>::type
    operator()( const int particle_index,
                const int d0,
                const int d1,
                const int d2,
                const int d3 ) const
    {
        static_assert( J == M,
                       "Do not call with different template arguments!" );
        return _aosoa.template get<J>(particle_index,d0,d1,d2,d3);
    }

    // -------------------------------
    // Raw data access.

    /*!
      \brief Get the stride between SoA data for this member.

      \return The stride at the given member index.

      Note that these strides are computed in the context of the *value_type*
      for each member.
    */
    KOKKOS_INLINE_FUNCTION
    int stride() const
    {
        return _aosoa.stride( M );
    }

    /*!
      \brief Get a non-const raw pointer to the data for this
      member

      \return A non-const raw pointer to the member data at the given index.
    */
    KOKKOS_INLINE_FUNCTION
    pointer_type data()
    {
        return static_cast<pointer_type>( _aosoa.data(M) );
    }

    /*!
      \brief Get a const raw pointer to the data for this member

      \return A const raw pointer to the member data at the given index.
    */
    KOKKOS_INLINE_FUNCTION
    const pointer_type data() const
    {
        return static_cast<pointer_type>( const_cast<void*>(_aosoa.data(M)) );
    }

  private:

    // The array-of-structs-of-arrays this slice wraps. We hold this by value
    // because the underlying block of data in the AoSoA is reference counted.
    aosoa_type _aosoa;
};

//---------------------------------------------------------------------------//
/*!
  \brief Free function so slice an AoSoA over a given member.

  \tparam M The member of the input AoSoA to slice.

  \tparam M The type of AoSoA to slice.

  \param aosoa The aosoa to slice.

  \return A slice for the given member index of the given AoSoA.
*/
template<std::size_t M, typename AoSoA_t>
MemberSlice<M,AoSoA_t> slice( AoSoA_t aosoa )
{ return MemberSlice<M,AoSoA_t>(aosoa); }

//---------------------------------------------------------------------------//

} // end namespace Cabana

#endif // end CABANA_MEMBERSLICE_HPP
