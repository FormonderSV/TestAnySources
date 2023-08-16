#include <iostream>
#include <modules/TestFile.hpp>

int main(int argc, char **argv) 
{
    const auto result = test_any_sources::Sum(1, 2);
    std::cout << "Sum: " << result << std::endl;
    return 0;
}
