// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoCamera.h
	\author		Jaap Suter
	\date		Jun 26 2001
	\ingroup	SoCamera

	See the \a SoCamera module for more information.
*/
// ----------------------------------------------------------------------------

#ifndef SO_CAMERA_H
#define SO_CAMERA_H

#ifdef __cplusplus
	extern "C" {
#endif

// ----------------------------------------------------------------------------
/*! 
	\defgroup SoCamera SoCamera
	\brief	  Camera for rendering 3D primitives

	This file contains the definitions of the camera
	class. Cameras are used to project and render 3 dimensional primitives
	onto the GBA screen.
	
*/ //! @{
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "SoVector.h"
#include "SoTransform.h"
#include "SoMatrix.h"
#include "SoMesh.h"
#include "SoPolygon.h"
#include "SoMath.h"
#include "SoDisplay.h"

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
/*! 
	\brief Used to render 3D stuff.

	Cameras are used to project and render 3 dimensional primitives
	onto the GBA screen.

	See the \a SoCamera module for more information.
*/
// ----------------------------------------------------------------------------
typedef struct
{
	SoTransform m_Transform;					//!< \internal 

	SoVector3	m_LeftFrustumPlaneNormal;		//!< \internal 
	SoVector3	m_TopFrustumPlaneNormal;		//!< \internal 

	bool		m_ClipAgainstFarAndNearPlane;	//!< \internal 
	bool		m_ClipAgainstFrustumSidePlanes;	//!< \internal 

	s32			m_NearPlaneDistance;			//!< \internal 
	s32			m_FarPlaneDistance;				//!< \internal 
	s32			m_ProjectionPlaneDistance;		//!< \internal 

} SoCamera;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Public methods;
// ----------------------------------------------------------------------------
void SoCameraInitialize( SoCamera* a_This );

void SoCameraForward(	 SoCamera* a_This, sofixedpoint a_Amount );
void SoCameraRight(		 SoCamera* a_This, sofixedpoint a_Amount );
void SoCameraUp(		 SoCamera* a_This, sofixedpoint a_Amount );

void SoCameraSetPitch(	 SoCamera* a_This, s32 a_Angle );
void SoCameraSetYaw(	 SoCamera* a_This, s32 a_Angle );
void SoCameraSetRoll(	 SoCamera* a_This, s32 a_Angle );

void SoCameraSetTranslation( SoCamera* a_This, sofixedpoint a_X, sofixedpoint a_Y, sofixedpoint a_Z );

void SoCameraDrawMesh(	 SoCamera* a_This, SoMesh* a_Mesh );

void SoCameraSetFarAndNearPlaneClippingEnable( SoCamera* a_This, bool a_Enable );
void SoCameraSetFrustumSidePlanesClippingEnable( SoCamera* a_This, bool a_Enable );

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------

//! @}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
