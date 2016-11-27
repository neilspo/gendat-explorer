///
/// \file
///

#ifndef ID_MANAGER_H
#define ID_MANAGER_H

#include <map>

class id_manager
{
public:
  id_manager(unsigned int num_required);
  ~id_manager();

  unsigned int alloc_id();
  unsigned int first_id() const;
  unsigned int  last_id() const;
  
  static bool set_first_allowed_id(unsigned int id);
  
private:
  static unsigned int first_allowed_id;
  static std::map<unsigned int, unsigned int> id_pool;

  unsigned int my_first_id;
  unsigned int my_num_reserved;
  unsigned int my_num_used;
  
};

#endif
