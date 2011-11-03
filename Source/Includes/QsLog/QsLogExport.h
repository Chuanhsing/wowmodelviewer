/*
	Export File for QsLog so it will work across mutliple projects.

	Copyright (c) 2011 Kjasi

	This Addendum to QsLog is free to use, and may be redistributed and/or modified,
	provided this entire copyright comment block is included and left unaltered.

	QsLog is copyright (c) 2010, Razvan Petru, All rights reserved.
*/

#ifndef QSLOG_EXPORT_H
#define QSLOG_EXPORT_H

#ifdef _WIN32	// Even 64bit programs have _WIN32 defined.
	#ifdef QSLOG_EXPORTS
		#define QSLOG_EXPORT __declspec(dllexport)
	#else
		#define QSLOG_EXPORT __declspec(dllimport)
	#endif
#else
	#define QSLOG_EXPORT
#endif

#endif