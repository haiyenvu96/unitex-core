 /*
  * Unitex
  *
  * Copyright (C) 2001-2009 Universit� Paris-Est Marne-la-Vall�e <unitex@univ-mlv.fr>
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

/* Nom 		: elag-functions.h */
/* Date 	: juin 98 */
/* Auteur(s) 	: MAYER Laurent et al */

#ifndef ElagFunctionsH
#define ElagFunctionsH


#include "Fst2Automaton.h"
#include "Vector.h"
#include "LanguageDefinition.h"

void remove_ambiguities(char* input_tfst,vector_ptr* grammars,char* output_tfst,Encoding,int,language_t* language);
void explode_tfst(char* input_tfst,char* output_tfst,Encoding,int,language_t* language);
vector_ptr* load_elag_grammars(char* filename,language_t* language,char* directory);

#endif
