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
* File name: /Vietnamese.cpp
* Date created: September 23, 2019
* Written by Bach Nguyen Sy
*/

#include "Vietnamese.h"
#include <regex>
#include <QString>

Vietnamese::Vietnamese() = default;

Vietnamese::~Vietnamese() = default;

QString Vietnamese::removeTone(const QString & text)
{
	auto result(text.toStdWString());
	result = std::regex_replace(result, std::wregex(L"à|á|ạ|ả|ã|â|ầ|ấ|ậ|ẩ|ẫ|ă|ằ|ắ|ặ|ẳ|ẵ|/g"), L"a");
	result = std::regex_replace(result, std::wregex(L"À|Á|Ạ|Ả|Ã|Â|Ầ|Ấ|Ậ|Ẩ|Ẫ|Ă|Ằ|Ắ|Ặ|Ẳ|Ẵ|/g"), L"A");
	result = std::regex_replace(result, std::wregex(L"è|é|ẹ|ẻ|ẽ|ê|ề|ế|ệ|ể|ễ|/g"), L"e");
	result = std::regex_replace(result, std::wregex(L"È|É|Ẹ|Ẻ|Ẽ|Ê|Ề|Ế|Ệ|Ể|Ễ|/g"), L"E");
	result = std::regex_replace(result, std::wregex(L"ì|í|ị|ỉ|ĩ|/g"), L"i");
	result = std::regex_replace(result, std::wregex(L"Ì|Í|Ị|Ỉ|Ĩ|/g"), L"I");
	result = std::regex_replace(result, std::wregex(L"ò|ó|ọ|ỏ|õ|ô|ồ|ố|ộ|ổ|ỗ|ơ|ờ|ớ|ợ|ở|ỡ|/g"), L"o");
	result = std::regex_replace(result, std::wregex(L"Ò|Ó|Ọ|Ỏ|Õ|Ô|Ồ|Ố|Ộ|Ổ|Ỗ|Ơ|Ờ|Ớ|Ợ|Ở|Ỡ|/g"), L"O");
	result = std::regex_replace(result, std::wregex(L"ù|ú|ụ|ủ|ũ|ư|ừ|ứ|ự|ử|ữ|/g"), L"u");
	result = std::regex_replace(result, std::wregex(L"Ù|Ú|Ụ|Ủ|Ũ|Ư|Ừ|Ứ|Ự|Ử|Ữ|/g"), L"U");
	result = std::regex_replace(result, std::wregex(L"ỳ|ý|ỵ|ỷ|ỹ|/g"), L"y");
	result = std::regex_replace(result, std::wregex(L"Ỳ|Ý|Ỵ|Ỷ|Ỹ|/g"), L"y");
	result = std::regex_replace(result, std::wregex(L"đ"), L"d");
	result = std::regex_replace(result, std::wregex(L"Đ"), L"D");
	return QString::fromStdWString(result);
}

QString Vietnamese::str(const wchar_t* text)
{
	return QString::fromWCharArray(text);
}

QString Vietnamese::red(const wchar_t* text)
{
	return QString("<font color='#ff007f'>%1</font>").arg(str(text));
}

QString Vietnamese::green(const wchar_t* text)
{
	return QString("<font color='#55aa00'>%1</font>").arg(str(text));
}

QString Vietnamese::blue(const wchar_t* text)
{
	return QString("<font color='#00AAFF'>%1</font>").arg(str(text));
}