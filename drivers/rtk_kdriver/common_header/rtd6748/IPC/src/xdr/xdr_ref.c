/* @(#)xdr_reference.c	2.1 88/07/29 4.0 RPCSRC */
/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */
#if !defined(lint) && defined(SCCSIDS)
static char sccsid[] = "@(#)xdr_reference.c 1.11 87/08/11 SMI";
#endif

/*
 * xdr_reference.c, Generic XDR routines implementation.
 *
 * Copyright (C) 1987, Sun Microsystems, Inc.
 *
 * These are the "non-trivial" xdr primitives used to serialize and de-serialize
 * "pointers".  See xdr.h for more info on the interface to xdr.
 */

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "xdr.h"

#ifdef USE_IN_LIBIO
# include <wchar.h>
# include <libio/iolibio.h>
#endif


#ifdef SHARED
#define INTUSE(name) name##_internal
#else
#define INTUSE(name) name
#endif

#define LASTUNSIGNED	((u_int)0-1)

/*
 * XDR an indirect pointer
 * xdr_reference is for recursively translating a structure that is
 * referenced by a pointer inside the structure that is currently being
 * translated.  pp references a pointer to storage. If *pp is null
 * the  necessary storage is allocated.
 * size is the size of the referneced structure.
 * proc is the routine to handle the referenced structure.
 */
bool_t
xdr_reference (xdrs, pp, size, proc)
     XDR *xdrs;
     caddr_t *pp;		/* the pointer to work on */
     u_int size;		/* size of the object pointed to */
     xdrproc_t proc;		/* xdr routine to handle the object */
{
  caddr_t loc = *pp;
  bool_t stat;

  if (loc == NULL)
    switch (xdrs->x_op)
      {
      case XDR_FREE:
	return TRUE;

      case XDR_DECODE:
	*pp = loc = (caddr_t) mem_alloc (size);
	if (loc == NULL)
	  {
#ifdef USE_IN_LIBIO
	    if (_IO_fwide (stderr, 0) > 0)
	      (void) __fwprintf (stderr, L"%s",
				 _("xdr_reference: out of memory\n"));
	    else
#endif
	      (void) fputs ("xdr_reference: out of memory\n", stderr);
	    return FALSE;
	  }
#if __BIONIC__ == 0
       bzero (loc, (int) size);
#else
       memset(loc, 0, size);
#endif
	break;
      default:
	break;
      }

  stat = (*proc) (xdrs, loc, LASTUNSIGNED);

  if (xdrs->x_op == XDR_FREE)
    {
      mem_free (loc, size);
      *pp = NULL;
    }
  return stat;
}


/*
 * xdr_pointer():
 *
 * XDR a pointer to a possibly recursive data structure. This
 * differs with xdr_reference in that it can serialize/deserialize
 * trees correctly.
 *
 *  What's sent is actually a union:
 *
 *  union object_pointer switch (boolean b) {
 *  case TRUE: object_data data;
 *  case FALSE: void nothing;
 *  }
 *
 * > objpp: Pointer to the pointer to the object.
 * > obj_size: size of the object.
 * > xdr_obj: routine to XDR an object.
 *
 */
bool_t
xdr_pointer (xdrs, objpp, obj_size, xdr_obj)
     XDR *xdrs;
     char **objpp;
     u_int obj_size;
     xdrproc_t xdr_obj;
{

  bool_t more_data;

  more_data = (*objpp != NULL);
  xdrs->x_handy += 4;    // add by ycyang
  if (!INTUSE(xdr_bool) (xdrs, &more_data))
    {
      return FALSE;
    }
  if (!more_data)
    {
      *objpp = NULL;
      return TRUE;
    }
  return INTUSE(xdr_reference) (xdrs, objpp, obj_size, xdr_obj);
}
