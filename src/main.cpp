#include <iostream>
#include <Windows.h>
#include <thread>

#include "StateMachine.h"
#include "State.h"


int main(int argc, char const *argv[])
{ 
    std::string buttonEntered;


    StateMachine sm{State::stand};
    sm.AddStateContext(State::stand, []{ std::cout << "Stood up\n"; }, []{ std::cout << "Standing\n"; }, []{ std::cout << "Stopped standing"; })
      .AddTransition(State::stand , State::prone , [&buttonEntered]{ return buttonEntered[0] == 'x'; })
      .AddTransition(State::stand , State::crouch, [&buttonEntered]{ return buttonEntered[0] == 'c'; });

    sm.AddTransition(State::crouch, State::stand , [&buttonEntered]{ return buttonEntered[0] == 's'; })
      .AddTransition(State::prone , State::stand , [&buttonEntered]{ return buttonEntered[0] == 's'; });    
    


    std::thread thr{ [&sm] {
      while(true)
      {
        sm.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
    }};
    thr.detach();



    for (;;)
    {
        std::cin >> buttonEntered;
        std::cout << "Typed: " << buttonEntered[0] << "\n";
    }
    
    

    return 0;
}
