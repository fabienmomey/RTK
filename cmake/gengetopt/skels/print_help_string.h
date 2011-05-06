/*
 * File automatically generated by
 * gengen 1.2 by Lorenzo Bettini 
 * http://www.gnu.org/software/gengen
 */

#ifndef PRINT_HELP_STRING_GEN_CLASS_H
#define PRINT_HELP_STRING_GEN_CLASS_H

#include <string>
#include <iostream>

using std::string;
using std::ostream;

class print_help_string_gen_class
{
 protected:
  string from;
  string full_index;
  string helpstring;
  string index;
  bool last;
  bool shared;
  string target;

 public:
  print_help_string_gen_class() :
    last (false), shared (false)
  {
  }
  
  print_help_string_gen_class(const string &_from, const string &_full_index, const string &_helpstring, const string &_index, bool _last, bool _shared, const string &_target) :
    from (_from), full_index (_full_index), helpstring (_helpstring), index (_index), last (_last), shared (_shared), target (_target)
  {
  }

  static void
  generate_string(const string &s, ostream &stream, unsigned int indent)
  {
    if (!indent || s.find('\n') == string::npos)
      {
        stream << s;
        return;
      }

    string::size_type pos;
    string::size_type start = 0;
    string ind (indent, ' ');
    while ( (pos=s.find('\n', start)) != string::npos)
      {
        stream << s.substr (start, (pos+1)-start);
        start = pos+1;
        if (start+1 <= s.size ())
          stream << ind;
      }
    if (start+1 <= s.size ())
      stream << s.substr (start);
  }

  void set_from(const string &_from)
  {
    from = _from;
  }

  void set_full_index(const string &_full_index)
  {
    full_index = _full_index;
  }

  void set_helpstring(const string &_helpstring)
  {
    helpstring = _helpstring;
  }

  void set_index(const string &_index)
  {
    index = _index;
  }

  void set_last(bool _last)
  {
    last = _last;
  }

  void set_shared(bool _shared)
  {
    shared = _shared;
  }

  void set_target(const string &_target)
  {
    target = _target;
  }

  void generate_print_help_string(ostream &stream, unsigned int indent = 0);
  
};

#endif // PRINT_HELP_STRING_GEN_CLASS_H