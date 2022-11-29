#include "helper.h"

void Phase1_GreenOn()
{
    OpenOutput(0);
}
void Phase1_GreenOff()
{
    CloseOutput(0);
}

void Phase1_YellowOn()
{
    OpenOutput(4);
}
void Phase1_YellowOff()
{
    CloseOutput(4);
}

void Phase1_RedOn()
{
    OpenOutput(6);
}
void Phase1_RedOff()
{
    CloseOutput(6);
}

void Phase2_GreenOn()
{
    OpenOutput(1);
}
void Phase2_GreenOff()
{
    CloseOutput(1);
}

void Phase2_YellowOn()
{
    OpenOutput(5);
}
void Phase2_YellowOff()
{
    CloseOutput(5);
}

void Phase2_RedOn()
{
    OpenOutput(7);
}
void Phase2_RedOff()
{
    CloseOutput(7);
}
