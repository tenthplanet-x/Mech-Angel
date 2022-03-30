// Copyright (C) 2000 Stephen Cleary
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org for updates, documentation, and revision history.

#ifndef STATIC_SINGLETON_HPP_GFHDGF4564564
#define STATIC_SINGLETON_HPP_GFHDGF4564564

//
// The following helper classes are placeholders for a generic "singleton"
//  class.  The classes below support usage of singletons, including use in
//  program startup/shutdown code, AS LONG AS there is only one thread
//  running before main() begins, and only one thread running after main()
//  exits.
//
// Don't use it as dynamic library interface.
//
// This class is also limited in that it can only provide singleton usage for
//  classes with default constructors.
//

// The design of this class is somewhat twisted, but can be followed by the
//  calling inheritance.  Let us assume that there is some user code that
//  calls "singleton_default<T>::instance()".  The following (convoluted)
//  sequence ensures that the same function will be called before main():
//    instance() contains a call to create_object.do_nothing()
//    Thus, object_creator is implicitly instantiated, and create_object
//      must exist.
//    Since create_object is a static member, its constructor must be
//      called before main().
//    The constructor contains a call to instance(), thus ensuring that
//      instance() will be called before main().
//    The first time instance() is called (i.e., before main()) is the
//      latest point in program execution where the object of type T
//      can be created.
//    Thus, any call to instance() will auto-magically result in a call to
//      instance() before main(), unless already present.
//  Furthermore, since the instance() function contains the object, instead
//  of the singleton_default class containing a static instance of the
//  object, that object is guaranteed to be constructed (at the latest) in
//  the first call to instance().  This permits calls to instance() from
//  static code, even if that code is called before the file-scope objects
//  in this file have been initialized.

namespace Utility
{
  // T must be: no-throw default constructible and no-throw destructible
  template <typename T>
  struct static_singleton
  {
  private:
    struct creator_helper
    {
      // This constructor does nothing more than ensure that instance()
      //  is called before main() begins, thus creating the static
      //  T object before multithreading race issues can come up.
      creator_helper() { static_singleton<T>::instance(); }
      inline void do_nothing() const { }
    };
    static creator_helper create_object;

    static_singleton();

  public:
    typedef T object_type;

    static object_type & instance()
    {
      static object_type obj;

      create_object.do_nothing();

      return obj;
    }
  };
  template <typename T>
  typename static_singleton<T>::creator_helper static_singleton<T>::create_object;

}

#endif

