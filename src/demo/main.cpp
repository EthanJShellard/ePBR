#include <ePBR/Context.h>
#include <stdexcept>
#include <iostream>

int main(int argc, char* argv[])
{
	ePBR::Context context;
	try 
	{
		context.Test(argv[0]);
	}
	catch (std::runtime_error e) 
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}