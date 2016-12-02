///
/// \class id_manager id_manager.h
///
/// \brief Assign globally unique identifiers
///
/// This class should be used in situations where there are a number of interacting objects,
/// each of which requires a block of sequential identifiers that are guaranteed to be globally
/// unique during the lifetime of the object.
///
/// The objects should each contain an instance of the id_manager class. The id_manager object
/// then will allocate the required number of identifiers when it is constructed and release
/// them again when the object is destroyed.
///

#include "id_manager.h"


// Initialize the static variables.

unsigned int                          id_manager::first_allowed_id = 1;
std::map <unsigned int, unsigned int> id_manager::id_pool;



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Default constructor
///
/// This constructor simply initializes the object.  Member function id_manager::reserve must
/// later be called to actually reserve the block of sequential identifiers.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

id_manager::id_manager ()
{
  my_first_id     = 0;
  my_num_reserved = 0;
  my_num_used     = 0;
}  



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// This constructor initializes the object and reserves a block of sequential identifiers.
///
/// \param[in] num_required   number of identifiers required
///
////////////////////////////////////////////////////////////////////////////////////////////////////

id_manager::id_manager (unsigned int num_required)
{
  do_reserve (num_required);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Destructor
///
/// The destructor releases the block of reserved identifiers.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

id_manager::~id_manager ()
{
  id_pool.erase (my_first_id);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Reserve identifiers
///
/// This member function reserves a block of sequential identifiers. It must be use only if
/// the default constructor was used, and then only called once.
///
/// \param[in] num_required   number of identifiers required
///
/// \return   true if the block of identifiers was successfully reserved, false otherwise
///
////////////////////////////////////////////////////////////////////////////////////////////////////

bool id_manager::reserve (unsigned int num_required)
{
  if (my_num_reserved == 0)
    {
      do_reserve (num_required);
      return true;
    }
  else
    {
      return false;
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Allocate the next reserved identifier
///
/// \return   next reserved identifier, otherwise set to zero if not enough reserved identifiers 
///
////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int id_manager::alloc_id()
{
  if (my_num_used < my_num_reserved)
    {
      return my_first_id + my_num_used++;
    }
  else
    {
      return 0;
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get first reserved identifier
///
/// This member function returns the value of the first identifier in the block
/// of sequential identifiers reserved for this object.
///
/// \return   first reserved identifier
///
////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int id_manager::first_id() const
{
  return my_first_id;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get last reserved identifier
///
/// This member function returns the value of the last identifier in the block
/// of sequential identifiers reserved for this object.
///
/// \return   last reserved identifier
///
////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int id_manager::last_id() const
{
  return my_first_id + my_num_reserved - 1;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Set the first allowed identifier
///
/// The first allowed identifier normally is set to one. If that is not acceptable, then this
/// member function can be used to changed it to any non-zero positive integer, as long the
/// change is made before any instance of the id_manager class is created.
///
/// \param[in] id   new value for the first allowed identifier
///
/// \return   true if the first allowed identifier was successfully changed, false otherwise
///
////////////////////////////////////////////////////////////////////////////////////////////////////

bool id_manager::set_first_allowed_id (unsigned int id)
{
  if (id > 0 && id_pool.empty())
    {
      first_allowed_id = id;
      return true;
    }
  else
    {
      return false;
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Reserve identifiers
///
/// This private member function reserves a block of sequential identifiers.
///
/// \param[in] num_required   number of identifiers required
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void id_manager::do_reserve (unsigned int num_required)
{
  if (id_pool.empty())
    {
    
      // If the ID pool is empty, then just insert the new ID requirement.
    
      my_first_id = first_allowed_id;
    }
  else
    {
    
      // If there is room at the beginning of the ID pool, then add the new requirement
      // ahead of whatever is currently in the pool.
    
      auto i = id_pool.begin();
      if ((i->first - first_allowed_id) >= num_required)
        {
          my_first_id = first_allowed_id;
        }
      else
        {
      
          // Search the ID pool for a gap of the required size. If there is a suitable
          // gap, then insert the new requirement there. Otherwise, insert it at the
          // end of the pool.
      
          my_first_id = i->first + i->second;
          while (++i != id_pool.end())
            {
              if ((i->first - my_first_id) >= num_required) break;
              my_first_id = i->first + i->second;
            }
        }
    }

  // Add the new ID requirement to the ID pool.
  
  id_pool[my_first_id] = num_required;

  // Initialize the other private variables.
  
  my_num_reserved = num_required;
  my_num_used     = 0;
}
