/*
 * Unitex
 *
 * Copyright (C) 2001-2017 Université Paris-Est Marne-la-Vallée <unitex@univ-mlv.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 *
 */
#ifndef USTRING_H_
#define USTRING_H_
/* ************************************************************************** */
// .h source file

/* ************************************************************************** */
// C system files (order the includes alphabetically)

/* ************************************************************************** */
// C++ system files (order the includes alphabetically)

/* ************************************************************************** */
// Other libraries' .h files (order the includes alphabetically)

/* ************************************************************************** */
// Project's .h files. (order the includes alphabetically)
#include "ELGLib/common.h"
#include "UnitexString.h"
/* ************************************************************************** */
#define EXTENSION_NAME_USTRING        EXTENSION_NAME_2(ELG,USTRING)
#define FUNCTION_PREFIX_USTRING       FUNCTION_PREFIX_2(ELG,USTRING)
#define EXTENSION_VERSION_USTRING     "0.1.0"
/* ************************************************************************** */
namespace unitex {
/* ************************************************************************** */
namespace elg {
/* ************************************************************************** */
namespace ustring {
/* ************************************************************************** */
namespace {   // namespace elg::ustring::{unnamed}, enforce one-definition-rule
// anonymous namespaces in C++ are more versatile and superior to static.
/* ************************************************************************** */
/* static */ int elg_ustring_upper(lua_State* L) {
  UnitexString* str = lua_lightobject_cast(L, 1, UnitexString);

  if(str->is_attached()) {
     str->upper();
     lua_pushlightobject(L, UnitexString)(const_cast<Ustring*>(str->c_ustring()));
  } else {

  }

  //lua_pushlightobject(L, UnitexString)(p)

  return 1;
}
/* ************************************************************************** */
/* static */ int elg_ustring_print(lua_State* L) {
  // check if there is at least an argument on the stack
  if(lua_gettop(L) >= 1) {
    // returns the light userdata pointer. Otherwise, returns NULL
    UnitexString* output= (UnitexString*) lua_touserdata(L, 1);
    output->title();
    u_printf("%S\n",output->c_unichar());
    const char* second = lua_tostring(L, 2);
    if(!output->is_null()) {
      u_printf("%S%s\n",output->c_unichar(),second);
    } else {
      u_printf("%s\n",second);
    }

    *output += "YES";
    output->append(second);

    // the number of results is equal to 0
    return 0;
  }
  // the number of results is equal to 0
  return 0;
}
/* ************************************************************************** */
/* static */ const struct luaL_Reg lua_lib_functions[] = {
  {NULL, NULL}
};
/* ************************************************************************** */
/* static */  const struct luaL_Reg lua_lib_methods[] = {
  DeclFuncEntry(USTRING, upper),
  DeclFuncEntry(USTRING, print),
  DeclGCEntry(UnitexString),
  {NULL, NULL}};
/* ************************************************************************** */
int luaopen_ustring(lua_State *L) {
  // -------------------------------------------
  // create the lib table
  // [-0, +1] > (+1)
  //lua_newtable(L);
  //  luaL_register(L, EXTENSION_FULL_NAME_USTRING, NullReference(lua_lib));
  //  elg_stack_dump(L);

  // register functions into the lib table
  luaL_register(L, EXTENSION_NAME_USTRING, lua_lib_functions);
  elg_stack_dump(L);

  // set the name of the module
  lua_pushliteral(L, EXTENSION_NAME_USTRING);
  lua_setfield(L, -2, "_NAME");
  elg_stack_dump(L);

  // set the version of the module
  lua_pushliteral(L, EXTENSION_VERSION_USTRING);
  lua_setfield(L, -2, "_VERSION");
  elg_stack_dump(L);

  // -------------------------------------------
  // create the module metatable
  luaL_newmetatable(L, EXTENSION_NAME_USTRING);
  elg_stack_dump(L);

  // duplicate the metatable
  lua_pushvalue(L, -1);
  elg_stack_dump(L);

  // mt.__index = mt
  lua_setfield(L, -2, "__index");
  elg_stack_dump(L);

  // register metamethods
  luaL_register(L, NULL, lua_lib_methods);
  elg_stack_dump(L);

  // assign the metatable to the lib table
  lua_setmetatable(L, -2);
  elg_stack_dump(L);

  // -------------------------------------------
  // add the lib table to the elg lib table
  lua_setfield(L, -2,  EXTENSION_NAME_USTRING);
  elg_stack_dump(L);

  return 1;
}
/* ************************************************************************** */
}  // namespace unitex::elg::ustring::{unnamed}
/* ************************************************************************** */
}  // namesapce unitex::elg::ustring
/* ************************************************************************** */
}  // namespace unitex::elg
/* ************************************************************************** */
}  // namespace unitex
/* ************************************************************************** */
#endif // USTRING_H_
