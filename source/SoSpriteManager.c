// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSpriteManager.c
	\author		Jaap Suter
	\date		Sep 19 2001
	\ingroup	SoSpriteManager

	See the \a SoSpriteManager module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------
#include "SoSpriteManager.h"
#include "SoIntManager.h"
#include "SoMath.h"
#include "SoDebug.h"
#include "SoTables.h"
#include "SoDisplay.h"
#include "SoDMA.h"

// DEBUG TEST;
#include "SoKeys.h"

// ----------------------------------------------------------------------------
// Defines;
// ----------------------------------------------------------------------------

//! \internal Start of the GBA's real object attribute memory (as opposed to the 
//!			  shadow OAM we also use).
#define SO_REAL_OAM			((u16*)0x7000000)

//! \internal Total number of sprites available in OAM;
#define SO_NUM_SPRITES	(128)

// ----------------------------------------------------------------------------
// Typedefs;
// ----------------------------------------------------------------------------

/*!
	\brief Sprite rotation and scaling structure.

	\internal

	This structure contains the neccesary attributes to set the scale and rotation
	for sprites. We put fillers in between each attribute. This way, we can overlay an
	array of this structure over the GBA's OAM, and acces each scale and rotation struct
	the easy way.
*/
typedef struct 
{
	u16 m_Filler0[ 3 ]; //!< \internal This member is never used; It is for OAM-lay-over purposes;
	u16 m_Attribute0;	//!< \internal Rotation and scaling attribute 0
	u16 m_Filler1[ 3 ]; //!< \internal This member is never used; It is for OAM-lay-over purposes;
	u16 m_Attribute1;	//!< \internal Rotation and scaling attribute 1
	u16 m_Filler2[ 3 ]; //!< \internal This member is never used; It is for OAM-lay-over purposes;
	u16 m_Attribute2;	//!< \internal Rotation and scaling attribute 2
	u16 m_Filler3[ 3 ]; //!< \internal This member is never used; It is for OAM-lay-over purposes;
	u16 m_Attribute3;	//!< \internal Rotation and scaling attribute 3

} SoRotationAndScale;


// ----------------------------------------------------------------------------
// Static variables;
// ----------------------------------------------------------------------------

/*!
	\brief Shadow object attribute memory.

	\internal

	Because the real OAM can only be updated during VBlank (and HBlank if enabled) we use a
	shadowing system. We keep an array of shadow-sprites in regular RAM, and copy this to
	OAM (VRam) each VBlank. This way we can update the sprite attributes whenever we like, and
	we are always sure the OAM will be updated accordingly.
*/
static SoSprite s_ShadowOAM[ SO_NUM_SPRITES ];

/*!
	\brief Stack to keep track of which sprites are free and which are not.

	\internal

	We use a stack to keep track of free sprite indices. This allows for speedy sprite 
	requests. The stack grows upwards, and is push -> increment (the stack pointer points
	to the free slot).

	As a consequence \a s_FreeSpriteIndexStack[ \a s_TopOfFreeSpriteIndexStack ] 
	always points to a free sprite location except when \a s_TopOfFreeSpriteIndexStack
	equals \a SO_NUM_SPRITES;
*/
static u8 s_FreeSpriteIndexStack[ SO_NUM_SPRITES ];

//! \internal Stack pointer.
static u8 s_TopOfFreeSpriteIndexStack;	

//! \internal This variable becomes true, once the sprite manager is
//!			  initialized. This is to prevent subsequent re-initializing;
static bool	s_Initialized = false;

/*! 
	\brief Rotation and Scale overlay array.

	We overlay an array of 32 rotation and scale settings over the shadow OAM. This way we 
	can easily acces individual rotation and scale attributes.
*/  
static volatile SoRotationAndScale* s_RotationAndScales = (SoRotationAndScale*) s_ShadowOAM;

// ----------------------------------------------------------------------------
// Function implementations;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Updates the sprite attributes in real object attribute memory.

	This function copied the shadow object attribute memory to the real object attribute 
	memory. Without calling this function you won't ever see your sprites.

	\warning	Call this only during VBlank. You can either call it in a VBlank interrupt,
				or right after a \a SoDisplayWaitForVBlankStart call.

	\warning	If in the future we are going to do multiplayer stuff, we have to 
				be carefull about DMA transfers cause they block interrupts. We might
				therefore consider using a CPUCopy for this function instead;
*/
// ----------------------------------------------------------------------------
void SoSpriteManagerUpdate( void )
{	
	// Copy the shadow OAM to the real OAM;
	// We divide the amount of bytes by 4 to get the 
	// amount of 32 bit transfers.
	SoDMATransfer( 3, s_ShadowOAM, 
	  SO_REAL_OAM, 
	  SO_NUM_SPRITES * sizeof( SoSprite ) >> 2,	
	  SO_DMA_DEST_INC | SO_DMA_SOURCE_INC | SO_DMA_32 | SO_DMA_START_NOW );
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Initializes the sprite manager.

	Should be the first function you call before you call any other \a SoSpriteManager 
	function. 

	Make sure you have already called \a SoDisplayInitialize too.
*/
// ----------------------------------------------------------------------------
void SoSpriteManagerInitialize( void )
{
	// Dummy counter;
	u32 i;

	// Pointer to start of real OAM;
	u16* realOAM = SO_REAL_OAM;	

	// Return if we are already initialized;
	if ( s_Initialized ) return;

	// Initialize the shadow and real OAM array to disabled sprites, 
	// and also initialize the free-sprite stack;
	for ( i = 0; i < SO_NUM_SPRITES; i++ )
	{
		// Disable the sprite;
		SoSpriteDisable( &s_ShadowOAM[ i ] );

		// Initialize the free sprite stack;
		s_FreeSpriteIndexStack[ i ] = i;

		// Set the real OAM sprites to all disabled.
		*realOAM = SO_BIT_9; 
		 realOAM += 4;
	}

	// Set the top of the stack;
	s_TopOfFreeSpriteIndexStack = 0;

	// Enable 1D sprite mode;
	SO_REG_DISP_CNT |= SO_BIT_6;

	// From now on we are initialized;
	s_Initialized = true;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Returns a pointer to an available sprite. 

	This is the only way to obtain a sprite. You can instantiate a sprite structure yourself
	but it will be useless. You have to ask the sprite manager for a sprite by using 
	this function.

	This routine will never fail. In debug mode an assertion will be triggered when 
	there are no more sprites available. Because of the fixed memory environment you can safely
	assume that if it works in debug, it'll work in release.
*/
// ----------------------------------------------------------------------------
SoSprite* SoSpriteManagerRequestSprite( void )
{
	// Assert that there is still room on the stack;
	SO_ASSERT( s_TopOfFreeSpriteIndexStack < SO_NUM_SPRITES,
			   "No more room for sprites. Change sprite layout or use less sprites." );
			
	// Increase the stack;
	s_TopOfFreeSpriteIndexStack++;

	// Return;
	return &s_ShadowOAM[ s_FreeSpriteIndexStack[ s_TopOfFreeSpriteIndexStack - 1 ] ];
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Releases a sprite.

	\param a_Sprite	Sprite you wish to release.

	Use this routine when you no longer need a certain sprite. The sprite manager
	reclaims it and after this it can be used for new sprite purposes.

	\warning	Do not use the sprite after you have released it. There are no garantuees
				on what will happen. Well, your GBA probably won't explode, but you never 
				know, eh :)....
*/
// ----------------------------------------------------------------------------
void SoSpriteManagerRelease( SoSprite* a_Sprite )
{
	// Calculate the index of this sprite;
	u32 spriteIndex = (a_Sprite - s_ShadowOAM);

	// Assert that the stack is not completely empty;
	SO_ASSERT( s_TopOfFreeSpriteIndexStack > 0, "You tried to remove a sprite that wasn't there" );	

	// Go down the stack;
	s_TopOfFreeSpriteIndexStack--;

	// Set the index;
	s_FreeSpriteIndexStack[ s_TopOfFreeSpriteIndexStack ] = spriteIndex;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Enables sprite rendering.

	Ofcourse you have to have a sprite and some character data the sprite uses, in order 
	to see a sprite, but without this routine the sprites would all be invisible.

	This also enables the shadow-to-real-oam copy routine in VBlank.
*/
// ----------------------------------------------------------------------------
void SoSpriteManagerEnableSprites( void )
{
	// Enable sprites;
	SO_REG_DISP_CNT |= SO_BIT_12;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Disables sprites rendering.

	Disables the visibility of all sprites and deinstalls the shadow-to-real-oam copy
	routine during VBlank.
*/
// ----------------------------------------------------------------------------
void SoSpriteManagerDisableSprites( void )
{
	// Disable sprites;
	SO_REG_DISP_CNT &= ~SO_BIT_12;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the rotation and scale values for the given index.

	\param a_Index			Index of the rotation and scale setting; [0..31] inclusive.
	\param a_Angle			Angle of the rotation, (256 is a full circle).
	\param a_FixedScaleX	Horizontal scale value in the SGADE fixed (17.15) format.
							Smaller value decrease the size of the sprite, 1.0 does nothing, 
							and bigger values largen the sprite.
	\param a_FixedScaleY	Vertical scale value in the SGADE fixed (17.15) format.
	
	The GBA's sprite rotation and scale system is a bit weird. Not every sprite has his own
	rotation and scale values. Instead there are several rotation and scale settings, and a
	sprite can refer to one of these settings and use it.

	With this method you can set the rotation and scale for the given index. After that, tell
	a sprite to use this index, and he'll rotate and scale accordingly (if you have sprite
	rotation and scaling enabled for that sprite).
	
	See the Pern Project (http://www.thepernproject.com) or other Nintendo references for more
	information on this system.
*/
// ----------------------------------------------------------------------------
void SoSpriteManagerSetRotationAndScale( u32 a_Index, s32 a_Angle, 
										 u32 a_FixedScaleX, u32 a_FixedScaleY )
{
	// Assert the input;
	SO_ASSERT( a_Index < 32, "Index out of bounds, valid range is [0..31]." );

	// Calculate the four rotation and scale attributes;
	s_RotationAndScales[ a_Index ].m_Attribute0 = (SO_FIXED_TO_N_8_FORMAT( a_FixedScaleX )
												*  SO_FIXED_TO_N_8_FORMAT( SO_COSINE( a_Angle ) ) ) 
												>> 8;
	s_RotationAndScales[ a_Index ].m_Attribute1 = (SO_FIXED_TO_N_8_FORMAT( a_FixedScaleY ) 
												*  SO_FIXED_TO_N_8_FORMAT( SO_SINE( a_Angle ) ) ) 
												>> 8;
	s_RotationAndScales[ a_Index ].m_Attribute2 = (SO_FIXED_TO_N_8_FORMAT( a_FixedScaleX ) 
												*  SO_FIXED_TO_N_8_FORMAT( -SO_SINE( a_Angle ) ) ) 
												>> 8;
	s_RotationAndScales[ a_Index ].m_Attribute3 = (SO_FIXED_TO_N_8_FORMAT( a_FixedScaleY ) 
												*  SO_FIXED_TO_N_8_FORMAT( SO_COSINE( a_Angle ) ) ) 
												>> 8;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief	Sets the sprite mosaic values
	\param	a_HorizontalSize Horizontal Mosaic value, 0 is no mosaic value, 
							 15 is maximum value.
	\param	a_VerticalSize	 Vertical Mosaic value, 0 is no mosaic value, 
							 15 is maximum value.

	Sets the global horizontal / vertical mosiac values for sprites.  These
	values are used in the rendering of all sprites which have mosaic mode
	enabled.
*/
// ----------------------------------------------------------------------------
void SoSpriteManagerSetMosaic( u32 a_HorizontalSize, u32 a_VerticalSize )
{
	// Assert the input range;
	SO_ASSERT( a_HorizontalSize < 16, "Mosaic size can't be bigger than 15" );
	SO_ASSERT( a_VerticalSize   < 16, "Mosaic size can't be bigger than 15" );

	// Clear any previous value;
	SO_REG_DISP_MOSAIC_RW &= ~(SO_8_BITS << 8);

	// Set the new value;
	SO_REG_DISP_MOSAIC_RW |= ((a_HorizontalSize | (a_VerticalSize << 4)) << 8);

	// Copy it into the write only hardware register;
	SO_REG_DISP_MOSAIC_W = SO_REG_DISP_MOSAIC_RW;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!	
	\brief Returns the horizontal sprite mosaic value;

	\return The horizontal sprite mosaic value (Range is [0, 15]).
*/
// ----------------------------------------------------------------------------
u32  SoSpriteManagerGetHorizontalMosaic( void )
{
	return ((SO_REG_DISP_MOSAIC_RW & (SO_4_BITS << 8)) >> 8);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!	
	\brief Returns the vertical sprite mosaic value;

	\return The vertical sprite mosaic value (Range is [0, 15]).
*/
// ----------------------------------------------------------------------------
u32  SoSpriteManagerGetVerticalMosaic( void )
{
	return ((SO_REG_DISP_MOSAIC_RW & (SO_4_BITS << 12)) >> 12);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
