// ----------------------------------------------------------------------------
/*! 
	Copyright (C) 2002 by the SGADE authors
	For conditions of distribution and use, see copyright notice in SoLicense.txt

	\file		SoSound.c
	\author		Jaap Suter
	\date		Feb 2 2002	
	\ingroup	SoSound

	See the \a SoSound module for more information.
*/
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes;
// ----------------------------------------------------------------------------

#include "SoSound.h"
#include "SoIntManager.h"
#include "SoDMA.h"
#include "SoTimer.h"
#include "SoDebug.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------

// GBA sound registers
#define SO_REG_SOUND1CNT_L	(*(volatile u16*)0x04000060)  	//!< \internal Sound 1 Sweep control  
#define SO_REG_SOUND1CNT_H	(*(volatile u16*)0x04000062)  	//!< \internal Sound 1 Lenght, wave duty and envelope control 
#define SO_REG_SOUND1CNT_X	(*(volatile u16*)0x04000064)  	//!< \internal Sound 1 Frequency, reset and loop control 
#define SO_REG_SOUND2CNT_L	(*(volatile u16*)0x04000068)  	//!< \internal Sound 2 Lenght, wave duty and envelope control 
#define SO_REG_SOUND2CNT_H	(*(volatile u16*)0x0400006C)  	//!< \internal Sound 2 Frequency, reset and loop control 
#define SO_REG_SOUND3CNT_L	(*(volatile u16*)0x04000070)  	//!< \internal Sound 3 Enable and wave ram bank control 
#define SO_REG_SOUND3CNT_H	(*(volatile u16*)0x04000072)  	//!< \internal Sound 3 Sound lenght and output level control 
#define SO_REG_SOUND3CNT_X	(*(volatile u16*)0x04000074)  	//!< \internal Sound 3 Frequency, reset and loop control 
#define SO_REG_SOUND4CNT_L	(*(volatile u16*)0x04000078)  	//!< \internal Sound 4 Lenght, output level and envelope control 
#define SO_REG_SOUND4CNT_H	(*(volatile u16*)0x0400007C)  	//!< \internal Sound 4 Noise parameters, reset and loop control 
#define SO_REG_SOUNDCNT_L	(*(volatile u16*)0x04000080)  	//!< \internal Sound 1-4 Output level and Stereo control 
#define SO_REG_SOUNDCNT_H	(*(volatile u16*)0x04000082)  	//!< \internal Direct Sound control and Sound 1-4 output ratio 
#define SO_REG_SOUNDCNT_X	(*(volatile u16*)0x04000084)  	//!< \internal Master sound enable and Sound 1-4 play status 
#define SO_REG_SOUNDBIAS	(*(volatile u16*)0x04000088)  	//!< \internal Sound bias and Amplitude resolution control 
#define SO_REG_WAVE_RAM0_L	(*(volatile u16*)0x04000090)  	//!< \internal Sound 3 samples 0-3 
#define SO_REG_WAVE_RAM0_H	(*(volatile u16*)0x04000092)  	//!< \internal Sound 3 samples 4-7 
#define SO_REG_WAVE_RAM1_L	(*(volatile u16*)0x04000094)  	//!< \internal Sound 3 samples 8-11 
#define SO_REG_WAVE_RAM1_H	(*(volatile u16*)0x04000096)  	//!< \internal Sound 3 samples 12-15 
#define SO_REG_WAVE_RAM2_L	(*(volatile u16*)0x04000098)  	//!< \internal Sound 3 samples 16-19 
#define SO_REG_WAVE_RAM2_H	(*(volatile u16*)0x0400009A)  	//!< \internal Sound 3 samples 20-23 
#define SO_REG_WAVE_RAM3_L	(*(volatile u16*)0x0400009C)  	//!< \internal Sound 3 samples 23-27 
#define SO_REG_WAVE_RAM3_H	(*(volatile u16*)0x0400009E)  	//!< \internal Sound 3 samples 28-31 
#define SO_REG_FIFO_A_L		(*(volatile u16*)0x040000A0)  	//!< \internal Direct Sound channel A samples 0-1 
#define SO_REG_FIFO_A_H		(*(volatile u16*)0x040000A2)  	//!< \internal Direct Sound channel A samples 2-3 
#define SO_REG_FIFO_B_L		(*(volatile u16*)0x040000A4)  	//!< \internal Direct Sound channel B samples 0-1 
#define SO_REG_FIFO_B_H		(*(volatile u16*)0x040000A6)  	//!< \internal Direct Sound channel B samples 2-3 


// ----------------------------------------------------------------------------
// Functions implementations;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief Initializes the sound system.

	This is the first \a SoSound function you should call.
*/
// ----------------------------------------------------------------------------
void SoSoundInitialize( void )
{
	// Initialize the interrupt manager;
	SoIntManagerInitialize();

	// Enable the sound system;
	SO_REG_SOUNDCNT_X = SO_BIT_7;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/*!
	\brief This function plays a sample at a given frequency on direct sound channel A.

	\param a_WaveData	8 bit signed wave data.
	\param a_Timer		Which timer to use, either 1 or 2.
	\param a_DMAChannel	Which DMA channel to use, either 1 or 2.
	\param a_Frequency	Frequency you want to use.
	
	\todo  Give more detail on this function.

	\todo  Finish this entire function even. Doh! This was just a test.
*/
// ----------------------------------------------------------------------------
void SoSoundPlayDirectSoundA( s8* a_WaveData, u32 a_Timer, u32 a_DMAChannel, u32 a_Frequency )
{
	// Assert the input;
	SO_ASSERT( (a_Timer		 == 1) || (a_Timer		== 2), "Invalid Direct Sound Timer given." );
	SO_ASSERT( (a_DMAChannel == 1) || (a_DMAChannel == 2), "Invalid Direct Sound DMA channel given" );

	// Enable the directsound channels;
	SO_REG_SOUNDCNT_H = 0xB0F; // TODO: explain 101100001111;
							   //				BA9876543210
	
	// Setup the FIFO DMA transfer;
	SoDMA1Transfer( a_WaveData, (void* )&SO_REG_FIFO_A_L, 0, SO_DMA_SOURCE_INC |
															 SO_DMA_DEST_INC   |
															 SO_DMA_START_FIFO |
															 SO_DMA_REPEAT
														);
	// Setup the direct sound timer;
	//SoTimerEnableForDirectSound( 0 );

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
