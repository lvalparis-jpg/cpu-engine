#pragma once
#include <iostream>


class Missile_Manager
{
private:


	using entityPair = std::pair<cpu_entity*, cpu_entity*>;
	using vectorPair = std::vector<entityPair>;

	vectorPair todestruct;

public:

	vectorPair& GetToDestruct() { return todestruct; };
};

