 /* ===================================================================================================================
	* Project:			PandoraLoader
	* File:					errors.h
	* Description:	Global error-code header
	*	Copyright:		(c) 2017, Pandora CNC Development Team; Consult the LICENSE file for further licensing information.
	* =================================================================================================================== */

#ifndef ERRORS_H
#define ERRORS_H

// =====================
// Main error-code enum
// =====================

// ===============================================
// The main, project-wide, error-code enumeration
// ================================================
// ERR_NONE is to be returned on success and a matching error-code on failure.
// The errors should not be referred to by their numeric value, but always by their name to allow for easier maintenance.

enum {
	ERR_NONE,
	ERR_ARGNULLP,
	ERR_NOINIT,
	ERR_NODATA,
	ERR_INTERR
};


#endif
