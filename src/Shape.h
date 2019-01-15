
#pragma once

#ifndef LAB471_SHAPE_H_INCLUDED
#define LAB471_SHAPE_H_INCLUDED

#include <string>
#include <vector>
#include <memory>

class Program;

class Shape
{

public:

	void loadMesh(const std::string &meshName);
	void init();
	void resize();
	void draw(const std::shared_ptr<Program> prog) const;
    unsigned int vaoID = 0;
    std::vector<float> posBuf;

private:

	std::vector<unsigned int> eleBuf;
	
	std::vector<float> norBuf;
	std::vector<float> texBuf;

	unsigned int eleBufID = 0;
	unsigned int posBufID = 0;
	unsigned int norBufID = 0;
	unsigned int texBufID = 0;
	

};

#endif // LAB471_SHAPE_H_INCLUDED
