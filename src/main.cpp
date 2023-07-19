#include <iostream>
#include <Windows.h>

#include "StateMachine.h"
#include "State.h"

int main(int argc, char const *argv[])
{
    std::string buttonEntered;

    StateMachine sm{State::stand};
    sm.AddStateContext(State::stand, []{}, []{}, []{})
      .AddTransition(State::stand , State::prone , [&buttonEntered]{ return buttonEntered[0] == 'x'; })
      .AddTransition(State::stand , State::crouch, [&buttonEntered]{ return buttonEntered[0] == 'c'; });

    sm.AddTransition(State::crouch, State::stand , [&buttonEntered]{ return buttonEntered[0] == 's'; })
      .AddTransition(State::prone , State::stand , [&buttonEntered]{ return buttonEntered[0] == 's'; });
    
    

    for (;;)
    {
        std::cin >> buttonEntered;
        std::cout << "Typed: " << buttonEntered[0] << "\n";

        sm.Update();
    }
    


    return 0;
}
