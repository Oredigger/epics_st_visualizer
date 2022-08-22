#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(void)
{
    std::ifstream fin("seq.st");
    std::stringstream ss;

    ss << fin.rdbuf();
    
    for (auto c : ss.str())
    {
        std::cout << c << std::endl;
    }

    fin.close();
    return 0;
}
