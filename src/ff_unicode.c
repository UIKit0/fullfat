/*****************************************************************************
 *  FullFAT - High Performance, Thread-Safe Embedded FAT File-System         *
 *  Copyright (C) 2009  James Walmsley (james@worm.me.uk)                    *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 *  IMPORTANT NOTICE:                                                        *
 *  =================                                                        *
 *  Alternative Licensing is available directly from the Copyright holder,   *
 *  (James Walmsley). For more information consult LICENSING.TXT to obtain   *
 *  a Commercial license.                                                    *
 *                                                                           *
 *  See RESTRICTIONS.TXT for extra restrictions on the use of FullFAT.       *
 *                                                                           *
 *  Removing the above notice is illegal and will invalidate this license.   *
 *****************************************************************************
 *  See http://worm.me.uk/fullfat for more information.                      *
 *  Or  http://fullfat.googlecode.com/ for latest releases and the wiki.     *
 *****************************************************************************/

/**
 *	@file		ff_unicode.c
 *	@author		James Walmsley
 *	@ingroup	UNICODE
 *
 *	@defgroup	UNICODE	FullFAT UNICODE Library
 *	@brief		Portable UNICODE Transformation Library for FullFAT
 *
 **/

#include "ff_unicode.h"

// UTF-8 Routines

/*
   UCS-4 range (hex.)           UTF-8 octet sequence (binary)
   0000 0000-0000 007F   0xxxxxxx
   0000 0080-0000 07FF   110xxxxx 10xxxxxx
   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx	-- We don't encode these because we won't receive them. (Invalid UNICODE).
   0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx					-- We don't encode these because we won't receive them. (Invalid UNICODE).
*/

FF_T_UINT FF_GetUtf16SequenceLen(FF_T_UINT16 usLeadChar) {
	if((usLeadChar & 0xFC00) == 0xD800) {
		return 2;
	}

	return 1;
}

/*
	Returns the number of UTF-8 units that a UTF-16 String will consume.
*/
FF_T_SINT32 FF_Utf16GetUtf8Len(const FF_T_UINT16 *utf16String) {

}

/*
	Returns the number of UTF-16 units that a UTF-8 String will consume.
*/
FF_T_SINT32 FF_Utf8GetUtf16Len(const FF_T_UINT8 *utf8String) {

}

/*
	Returns the number of UTF-16 units required to encode the UTF-8 sequence in UTF-16.
	Will not exceed ulSize UTF-16 units. (ulSize * 2 bytes).
*/
FF_T_SINT32 FF_Utf8ctoUtf16c(FF_T_UINT16 *utf16Dest, const FF_T_UINT8 *utf8Source, FF_T_UINT32 ulSize) {

}


/*
	Returns the number of UTF-8 units required to encode the UTF-16 sequence.
	Will not exceed ulSize UTF-8 units. (ulSize  * 1 bytes).
*/
FF_T_SINT32 FF_Utf16ctoUtf8c(FF_T_UINT8 *utf8Dest, const FF_T_UINT16 *utf16Source, FF_T_UINT32 ulSize) {
	FF_T_UINT32	ulUtf32char;

	if((*utf16Source & 0xFC00) == 0xD800) {	// A surrogate sequence was encountered. Must transform to UTF32 first.
		ulUtf32char  = ((FF_T_UINT32) (*(utf16Source + 0) & 0x003FF) << 10) + 0x10000;
	
		if((*(utf16Source + 1) & 0xFC00) != 0xDC00) {
			return FF_ERR_UNICODE_INVALID_SEQUENCE;	// Invalid UTF-16 sequence.
		}
		ulUtf32char |= ((FF_T_UINT32) (*(utf16Source + 1) & 0x003FF));

	} else {
		ulUtf32char = (FF_T_UINT32) *utf16Source;
	}

	// Now convert to the UTF-8 sequence.
	if(ulUtf32char < 0x00000080) {	// Single byte UTF-8 sequence.
		*(utf8Dest + 0) = (FF_T_UINT8) ulUtf32char;
		return 1;
	}

	if(ulUtf32char < 0x00000800) {	// Double byte UTF-8 sequence.
		*(utf8Dest + 0) = (FF_T_UINT8) (0xC0 | ((ulUtf32char >> 6) & 0x1F));
		*(utf8Dest + 1) = (FF_T_UINT8) (0x80 | ((ulUtf32char >> 0) & 0x3F));
		return 2;
	}

	if(ulUtf32char < 0x00010000) {	// Triple byte UTF-8 sequence.
		*(utf8Dest + 0) = (FF_T_UINT8) (0xE0 | ((ulUtf32char >> 12) & 0x0F));
		*(utf8Dest + 1) = (FF_T_UINT8) (0x80 | ((ulUtf32char >> 6 ) & 0x3F));
		*(utf8Dest + 2) = (FF_T_UINT8) (0x80 | ((ulUtf32char >> 0 ) & 0x3F));
		return 3;
	}

	if(ulUtf32char < 0x00200000) {	// Quadruple byte UTF-8 sequence.
		*(utf8Dest + 0) = (FF_T_UINT8) (0xF0 | ((ulUtf32char >> 18) & 0x07));
		*(utf8Dest + 1) = (FF_T_UINT8) (0x80 | ((ulUtf32char >> 12) & 0x3F));
		*(utf8Dest + 2) = (FF_T_UINT8) (0x80 | ((ulUtf32char >> 6 ) & 0x3F));
		*(utf8Dest + 3) = (FF_T_UINT8) (0x80 | ((ulUtf32char >> 0 ) & 0x3F));
		return 4;
	}

	return FF_ERR_UNICODE_INVALID_CODE;	// Invalid Charachter
}


// UTF-16 Support Functions

// Converts a UTF-32 Charachter into its equivalent UTF-16 sequence.
FF_T_SINT32 FF_Utf32ctoUtf16c(FF_T_UINT16 *utf16Dest, FF_T_UINT32 utf32char, FF_T_UINT32 ulSize) {

	// Check that its a valid UTF-32 wide-char!	

	if(utf32char >= 0xD800 && utf32char <= 0xDFFF) {	// This range is not a valid Unicode code point.
		return FF_ERR_UNICODE_INVALID_CODE; // Invalid charachter.
	}

	if(utf32char <= 0xFFFF) {
		*utf16Dest = (FF_T_UINT16) utf32char; // Simple conversion! Char comes within UTF-16 space (without surrogates).
		return 1;
	}

	if(ulSize < 2) {
		return FF_ERR_UNICODE_DEST_TOO_SMALL;	// Not enough UTF-16 units to record this charachter.
	}

	if(utf32char < 0x00200000) {
		// Conversion to a UTF-16 Surrogate pair!
		//valueImage = utf32char - 0x10000;
		
		*(utf16Dest + 0) = (FF_T_UINT16) (((utf32char - 0x10000) & 0xFFC00) >> 10) | 0xD800;
		*(utf16Dest + 1) = (FF_T_UINT16) (((utf32char - 0x10000) & 0x003FF) >> 00) | 0xDC00;
		
		return 2;	// Surrogate pair encoded value.
	}
	
	return FF_ERR_UNICODE_INVALID_CODE;	// Invalid Charachter
}

// Converts a UTF-16 sequence into its equivalent UTF-32 code point.
FF_T_SINT32 FF_Utf16ctoUtf32c(FF_T_UINT32 *utf32Dest, const FF_T_UINT16 *utf16Source) {
	
	if((*utf16Source & 0xFC00) != 0xD800) {	// Not a surrogate sequence.
		*utf32Dest = (FF_T_UINT32) *utf16Source;
		return 1;	// A single UTF-16 item was used to represent the charachter.
	}
	
	*utf32Dest  = ((FF_T_UINT32) (*(utf16Source + 0) & 0x003FF) << 10) + 0x10000;
	
	if((*(utf16Source + 1) & 0xFC00) != 0xDC00) {
		return FF_ERR_UNICODE_INVALID_SEQUENCE;	// Invalid UTF-16 sequence.
	}
	*utf32Dest |= ((FF_T_UINT32) (*(utf16Source + 1) & 0x003FF));
	return 2;	// 2 utf-16 units make up the Unicode code-point.
}


/*
	Returns the total number of UTF-16 items required to represent
	the provided UTF-32 string in UTF-16 form.
*/
FF_T_UINT FF_Utf32GetUtf16Len(const FF_T_UINT32 *utf32String) {
	FF_T_UINT utf16len = 0;

	while(*utf32String) {
		if(*utf32String++ <= 0xFFFF) {
			utf16len++;
		} else {
			utf16len += 2;
		}
	}
	
	return utf16len;
}