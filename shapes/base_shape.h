#ifndef _BASE_SHAPE_H
#define _BASE_SHAPE_H

class BaseShape
{
	public:
	BaseShape(){}
	virtual BaseShape(){}
	
	void init() = 0;
	void shutdown() = 0;
	void draw() = 0;
};

#endif
