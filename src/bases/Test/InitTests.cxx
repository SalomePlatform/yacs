#include "UnitTestsResult.hxx"
#include <stdlib.h>

/*!
 */

int main(int argc, char* argv[])
{
  std::ofstream testFile;
  system("mkdir -p /tmp/${USER}");
  testFile.open(YACS::UnitTestsResult.c_str(), std::ios::out | std::ios::trunc);
  testFile << " --- YACS Unit Tests summary ---" << std::endl;
  testFile.close();
  return 0;
}
