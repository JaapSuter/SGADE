// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		Socrates.h
	\author		Jaap Suter, Mark T. Price
	\date		April 3, 2003
	\ingroup	Socrates

	See the \a Socrates module for more information. This file also contains some 
	information on the SGADE documentation in general, and some information
	on the philosophy and codestyle the SGADE uses.
*/
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/*
	Scroll down to see the code this file contains. Cause first this file
	contains some general SGADE information. Sadly this text contains some HTML
	tags to incorporate it into the SGADE online documentation. Just try to
	ignore these, or read this text in the online version (where these tags
	actually improve readability).
*/
// ---------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
/*!
	\mainpage The SGADE Documentation

	<CENTER>
		<B>Version 1.02 dev D</B> This text comes from the \a Socrates.h file.
	</CENTER>
		
	<B> Introduction to the Documentation </B>

	The documentation for the SGADE is divided into several parts. You are now
	reading the largest section. It contains a complete reference to all code,
	and some background information on the SGADE development philosophy and
	codestyle. Other information can be found in the FAQ and the the tutorials.
	These can be found on the 
	<A HREF = "http://www.suddenpresence.com/sgade/">SGADE</A> website. 	
	
	Before you start using the SGADE I strongly suggest reading all information
	below. After that I would browse this reference for a bit, and then take a
	go through the tutorials.

	<B> Philosophy </B>

	I believe in object and component oriented programming. However, unlike some
	people, I also believe that one can use any coding paradigm in any language.
	I don't need C++ or Java to use OOP, and I could do functional programming
	in assembly. Some languages might be better suited for some paradigms, but
	some languages are also better suited for some platforms. This is the
	reason I chose C over C++ for Gameboy Advance development.

	I can see the merits in using C++ for a GBA project, but only for advanced
	management systems. If, for example, I had to write a strategy game for the
	GBA, I would probably use C++ constructs to manage units and resources. But
	for Socrates, C gave me all I ever needed.  Object oriented programming
	without the memory overhead and performance drawbacks of C++.
	
	If anyone wants to discuss this with me, and wants to tell me C++ can be
	just as fast, I'd love a discussion. But not until you've read Scott
	Meyer's books: Effective C++ and More Effective C++.

	So, I would be using C. How then is OOP reflected in my C coding? Two things
	are important here. This pointers and singletons. 
	
	This pointers are used whenever a function acts upon an object. In C++ all
	methods belonging to a class receive an implicit (hidden) this pointer.
	Whenever you invoke a method on a class like this:
		
	\code 
		object.Method();
	\endcode

	What C++ actually does is this:

	\code 
		Method( &object );
	\endcode

	So all we have to do to emulate an object oriented class-method-like system
	is provide the this pointer ourselves. What better than using a short
	example. Take a look at the following C++ code.

	\code
		// Define the class
		class SoClass
		{
			// Attributes;
			int m_Attribute;

			// Methods;
			void Method1( void );
			void Method2( int a_Argument );
			int  Method3( void );
		}

		// Declare an instance;
		SoClass object

  		// Use the class
		object.Method1();
		object.Method2( 10 );
		int i = object.Method3();
	
	\endcode

	This would be written in C (the SGADE) as follows:

	\code
	
		// Define the structure
		typedef struct 
		{
			// Attributes;
			int attribute;
		} SoClass;

		// Define the methods;
		void SoClassMethod1( SoClass* a_This );
		void SoClassMethod2( SoClass* a_This, int a_Argument );
		int  SoClassMethod3( SoClass* a_This );

		// Declare an instance;
		SoClass object

  		// Use the class
		SoClassMethod1( &object );
		SoClassMethod2( &object, 10 );
		int i = SoClassMethod3( &object );

	\endcode

	Once you realize there is actually no difference between the above C++ and
	C code, nothing stops you from doing OOP in C. It is true that the often
	called 'true' OOP concepts like polymorphism and inheritance require more
	complicated constructs, but most of it can be solved using clever casting
	and function pointers.

	What about singletons then? Well, targeting the Gameboy Advance, it turns
	out that a lot of classes are often instantiated only once. A good example
	is an interrupt manager or a mode 4 renderer. All methods belonging to such
	an object will always receive the same this pointer.   This would be
	nonsense ofcourse, and therefore we treat these objects as singletons. In 
	the SGADE a singleton is a group of global functions without this pointers.
	But we treat those as methods belonging to and acting upon a single object
	and not as seperate functions.  Consider the following functions:

	\code
		void SoMode4RendererEnable( void );
		void SoMode4RendererFlip( void );
		void SoMode4RendererClear( void );
			....
			
	\endcode

	All of these functions act upon a global fictional object called
	SoMode4Renderer. There is no SoMode4Renderer type, but the object is there.
	You will never see it, but just remember that whenever you use a singleton. 

	C, as opposed to C++, has no build in information hiding facilities. Every
	attribute of a structure is public (for the compiler). However, you should
	always treat all members of every structure as  private. Use the supplied
	methods to act upon these structures. 
	
	There is a single exception to this. The \a SoVector3 and SoVector2
	structures (because of their simplicity) have public members. Every other
	structure has only private members. These are marked as 'For Internal Use
	Only'. Treat them that way.
	
	<B> Code Style </B>

	I use a fairly strict coding style. Uniform and consistent coding improves
	readability.  I use the following rules for coding.

		- Basic datatypes are:
			- \a bool
			- \a u32
			- \a s32
			- \a u16
			- \a s16
			- \a u8
			- \a s8
			- \a char
			- \a sofixedpoint

		- Global variables start with \a g_

		- Static variables start with \a s_
		- Members of structures start with \a m_
		- Function arguments start with \a a_
		- Local variables start with a lowercase letter

		- Each new word within an identifier starts with an uppercase letter
		
		- Functions start with an uppercase letter.
		- Structures start with an uppercase letter.
		
		- Structures and functions are prefixed with \a So.
		- Functions belonging to a structure (methods) are prefixed with the
		  structure name.

		- The opening curly bracket is always below the previous line.
		- The opening function bracket is always directly after the function
		  name.
		- An opening bracket is always followed by a space, and a closing
		  bracket is always prefixed by a space.

		- Each function, non-local variable, and structure has a Doxygen-
		  compatible description.  Scroll down for more information on Doxygen
		  source documentation.

	The following piece of code gives some examples. Note that the large blocks
	of comments are normally done by using the / * and * / (without the spaces)
	comment style, but I can't use these here as this code example is itself
	embedded in a comment (yes this HTML is generated from a source file, see
	the next section on Doxygen for more information).

	\code

		// A global variable.
		u32 g_GlobalVariable;

		// A static variable.
		static u32 s_StaticVariable;

		// --------------------------------------------------------------------
		//
		//	\brief	A global function.
		//	\param	a_Argument1			First argument of the function
		//	\param	a_Argument2			First argument of the function
		//	
		//	\return  The first argument added to the second argument.
		//	
		//	This function does blah blah blah
		//	          blah blah blah
		//					blah blah blah
		//	
		// --------------------------------------------------------------------
		u32 SoGlobalFunction( u32 a_Argument1, u32 a_Argument2 )
		{
			// A local variable.
			u32	localVariable;

			// Do the calculation
			localVariable = a_Argument1 + a_Argument2;

			// Done;
			return localVariable;
		}
		// --------------------------------------------------------------------
	
		// --------------------------------------------------------------------
		//
		//	\brief This structure represents blah blah;
		//
		//	This structure can be used to do blah blah blah blah...
		//
		// --------------------------------------------------------------------
		typedef struct
		{
			u32 m_MemberAttribute1;		// Attribute that is for blah....
			u32 m_MemberAttribute2;		// Attribute that is for bleh...
		
		} SoSomeStructure
		// --------------------------------------------------------------------

		// --------------------------------------------------------------------
		// 
		//	\brief	Method belonging to the SoSomeStructure class
		//	
		//	\param	a_This	This pointer
		//
		//	This function does blah blah blah blah...
		//
		// --------------------------------------------------------------------
		void SoSomeStructureMethod( SoSomeStructure* a_This )
		{
			// Acces the member of the given this pointer.
			a_This->m_MemberAttribute = 10;
		}
		// --------------------------------------------------------------------

	\endcode

	Just look at the above example and some of the SGADE code and you'll soon
	see the consistency and particular coding practices I use to improve
	development.
	
	<B> On Doxygen Source Commenting </B>

	Doxygen is a tool that extracts comments from source code and creates
	documentation in HTML, Latex, or other formats. The very words you are
	reading now (if you are reading this documentation in HTML format) have
	been extraced by Doxygen.

	More information on Doxygen can be found on its website at 
	<A HREF = "http://www.doxygen.org">http://www.doxygen.org</A> I'll give
	some short tips and advice that might help when you start browsing the
	SGADE source code.

	Doxygen parses the sourcecode and uses the source surrounding comments to
	get information about this code. You can add certain tags to the comments
	to give Doxygen information about what you mean with the code following the
	comments. 
	
	A Doxygen compatible source comment can be given in the following ways:

	- / *!	and a closing * / (without the space in between the *s and the /s)
	        for a compound comment block, referring to the source after the
	        block.
	- //!	For a single line Doxygen comment referring to the source after the
	        comment.
	- //!<	For a single line Doxygen comment referring to the source before
	        the comment.

	Inside a comment the following tags are used as special commands. Each tag
	is prefixed by a \ (backslash).

	- \a a			- This means the single following word after the tag is a
	                  special word and needs special casing. 
	
	- \a file		- This tells Doxygen you want a file documented.
	
	- \a defgroup	- To define a group of functionality (a module).
	- \a ingroup	- To tell Doxygen this belongs to a certain group.
	
	- \a brief		- This gives a short one-line description on the following
	                  structure or function. As opposed to the more detailed
	                  description.
	
	- \a param 		- To give information on a function parameter.
	- \a return		- To give information on a function return value.
	- \a retval		- To give information on a function output parameter.
	
	- \a internal	- To specify something as 'For Internal Use Only'.
	
	There might be some more tags in the source I have forgotten here. But most
	of the time these will explain themselves.

	<B> Where to from here </B>

	Now that you have read something about the philosophy and coding style of
	the SGADE you know what to expect when using the SGADE. I suggest you start
	by browsing this reference, especially the \a modules section at the top of
	this page. After that, browse through some of the code and try some
	tutorials.
*/
// ----------------------------------------------------------------------------

#ifndef SOCRATES_H
#define SOCRATES_H

// ----------------------------------------------------------------------------
/*! 
	\defgroup Socrates Socrates
	\brief	  Socrates header that includes all other headers

	Singleton

*/ //! @{
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------

#include "SoAssembly.h"
#include "SoBkg.h"
#include "SoBkgConsole.h"
#include "SoBkgCreditScroll.h"
#include "SoBkgFont.h"
#include "SoBkgManager.h"
#include "SoBkgMap.h"
#include "SoBkgMemManager.h"
#include "SoBkgManager.h"
#include "SoCamera.h"
#include "SoDebug.h"
#include "SoDisplay.h"
#include "SoDMA.h"
#include "SoEffects.h"
#include "SoFlashMem.h"
#include "SoFont.h"
#include "SoIntManager.h"
#include "SoKeys.h"
#include "SoMath.h"
#include "SoMatrix.h"
#include "SoMemManager.h"
#include "SoMesh.h"
#include "SoMode4PolygonRasterizer.h"
#include "SoMode4Renderer.h"
#include "SoMultiPlayer.h"
#include "SoPalette.h"
#include "SoPolygon.h"
#include "SoSound.h"
#include "SoSprite.h"
#include "SoSpriteManager.h"
#include "SoSpriteMemManager.h"
#include "SoSpriteAnimation.h"
#include "SoSystem.h"
#include "SoTables.h"
#include "SoImage.h"
#include "SoTileMap.h"
#include "SoTileSet.h"
#include "SoTimer.h"
#include "SoTransform.h"
#include "SoVector.h"
#include "SoWindow.h"

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------


//! @}

#endif
