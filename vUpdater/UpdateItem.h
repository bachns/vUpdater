/**
*   Copyright (C) 2020 by vGeoDev
*   Unauthorized copying of this file via any medium is strictly prohibited
*/
/**
*  File name:    UpdateItem.h
*  Date created: May 23, 2020
*  Written by Bach Nguyen Sy
*  bachns.dev@gmail.com | bachns.github.io
***/

#ifndef UPDATEITEM_H
#define UPDATEITEM_H

#include <QJsonObject>

class UpdateItem
{
public:
	enum class Action
	{
		Add, Remove, None
	};
	
	UpdateItem() { mAction = Action::None; }

	UpdateItem(const UpdateItem& item)
	{
		mAction = item.mAction;
		mUrl = item.mUrl;
		mTarget = item.mTarget;
	}
	
	explicit UpdateItem(const QJsonObject& object)
	{
		loadFromJsonObject(object);
	}

	UpdateItem& operator=(const UpdateItem& item)
	{
		if (&item == this)
			return *this;

		mAction = item.mAction;
		mUrl = item.mUrl;
		mTarget = item.mTarget;
		return *this;
	}

	void loadFromJsonObject(const QJsonObject& object)
	{
		mUrl = object.value("url").toString();
		mTarget = object.value("target").toString();

		QString action = object.value("action").toString();
		if (action.compare("add", Qt::CaseInsensitive) == 0)
			mAction = Action::Add;
		else if (action.compare("remove", Qt::CaseInsensitive) == 0)
			mAction = Action::Remove;
		else
			mAction = Action::None;
	}

	Action mAction = Action::None;
	QString mUrl;
	QString mTarget;
};

#endif // UPDATEITEM_H
