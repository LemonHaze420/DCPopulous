// This file contains the definition of several useful basic types.

#ifndef _BASICTYPES_H_
#define _BASICTYPES_H_

// ----
struct	_IntCouple
{
	int	A;
	int B;
};

// ----
// Doesn't use as much memory as a RECT;
struct _SmallRect
{
	SHORT	top;
	SHORT	left;
	SHORT	bottom;
	SHORT	right;
};


// ----
// Typedefs
typedef _IntCouple	IntCouple;
typedef _SmallRect	SmallRect;

#endif