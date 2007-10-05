#include "DrivenCondition.hxx"

#if defined(YACS_PTHREAD)
#include "DrivenConditionPT.cxx"
#else
#error
#endif
