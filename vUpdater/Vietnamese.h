/********************************************************************************
*   Copyright (C) 2019 by Bach Nguyen Sy                                       *
*   Unauthorized copying of this file via any medium is strictly prohibited    *
*                                                                              *
*   <bachns.dev@gmail.com>                                                     *
*   https://bachns.wordpress.com                                               *
*   https://www.facebook.com/bachns.dev                                        *
*                                                                              *
********************************************************************************/

/**
* File name: /Vietnamese.h
* Date created: September 23, 2019
* Written by Bach Nguyen Sy
*/
#ifndef VIETNAMESE_H
#define VIETNAMESE_H

class QString;

class Vietnamese
{
public:
	Vietnamese();
	~Vietnamese();
	static QString removeTone(const QString& text);
	static QString str(const wchar_t* text);
	static QString red(const wchar_t* text);
	static QString green(const wchar_t* text);
	static QString blue(const wchar_t* text);
};

#endif // VIETNAMESE_H