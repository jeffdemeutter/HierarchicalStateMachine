#include <iostream>
#include <thread>

#include "../include/StateMachine.h"

// an enum is mandatory, you need to pass this on to the statemachine
// constructor
enum class State { stand, crouch, prone };

void BasicExample();
void BasicWithUsingExample();
void StateCallBackExamples();
void UpdateMultiThreadedExample();

int main(int argc, char const *argv[]) {
  // uncomment the examples below to test them out yourself

  BasicExample();
  // BasicWithUsingExample();
  // StateCallBackExamples();
  // UpdateMultiThreadedExample();
  return 0;
}

void BasicExample() {
  std::string buttonEntered;

  // You create the statemachine here, and pass on a starting state from an enum
  // Note that only enum values are only valid here
  StateMachine sm{State::stand};

  // then you add your transitions
  // you need to pass on a mathod that returns a bool
  sm.AddTransition(State::stand, State::crouch, [&buttonEntered] {
      return buttonEntered[0] == 'c';
    }).AddTransition(State::crouch, State::stand, [&buttonEntered] {
    return buttonEntered[0] == 's';
  });

  // loop for testing
  for (;;) {
    std::cin >> buttonEntered;
    std::cout << "Typed: " << buttonEntered[0] << "\n";

    sm.Update();
    std::cout << "CurrentState: " << (int)sm.GetCurrentState() << "\n";
  }
}

void BasicWithUsingExample() {
  std::string buttonEntered;

  // you can also define your statemachines like this
  // this can be put together next to your enum definitions
  using MyStateMachine = StateMachine<State>;

  // Here you create your defined statemachine
  MyStateMachine sm{State::stand};

  // then you add your transitions
  // you need to pass on a mathod that returns a bool
  sm.AddTransition(State::stand, State::crouch, [&buttonEntered] {
      return buttonEntered[0] == 'c';
    }).AddTransition(State::crouch, State::stand, [&buttonEntered] {
    return buttonEntered[0] == 's';
  });

  // loop for testing
  for (;;) {
    std::cin >> buttonEntered;
    std::cout << "Typed: " << buttonEntered[0] << "\n";

    sm.Update();
    std::cout << "CurrentState: " << (int)sm.GetCurrentState() << "\n";
  }
}

void StateCallBackExamples() {
  std::string buttonEntered;

  // create statemachine
  StateMachine sm{State::stand};

  // add transitions
  sm.AddTransition(State::stand, State::crouch, [&buttonEntered] {
      return buttonEntered[0] == 'c';
    }).AddTransition(State::crouch, State::stand, [&buttonEntered] {
    return buttonEntered[0] == 's';
  });

  // add state callbacks
  // you also have the onUpdate and onStop callbacks
  sm.AddOnStart(State::stand, [] {
      std::cout << "Stood up!\n";
    }).AddOnStart(State::crouch, [] { std::cout << "crouched!\n"; });

  // loop for testing
  for (;;) {
    std::cin >> buttonEntered;
    std::cout << "Typed: " << buttonEntered[0] << "\n";

    sm.Update();
  }
}

void UpdateMultiThreadedExample() {
  std::string buttonEntered;

  // look at previous examples
  StateMachine sm{State::stand};
  sm.AddStateContext(
        State::stand, [] { std::cout << "Stood up\n"; },
        [] { std::cout << "Standing\n"; },
        [] { std::cout << "Stopped standing\n"; })
      .AddStateContext(
          State::crouch, [] { std::cout << "Crouched\n"; },
          [] { std::cout << "crouching\n"; },
          [] { std::cout << "Stopped crouching\n"; })

      .AddTransition(State::stand, State::crouch,
                     [&buttonEntered] { return buttonEntered[0] == 'c'; })
      .AddTransition(State::crouch, State::stand,
                     [&buttonEntered] { return buttonEntered[0] == 's'; });

  // you can also offload the updating to other threads
  // this will keep updating that statemachine while your main thread can still
  // do other stuff
  std::thread thr{[&sm] {
    while (true) {
      sm.Update();
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  }};
  thr.detach();

  // loop for testing
  for (;;) {
    std::cin >> buttonEntered;
    std::cout << "Typed: " << buttonEntered[0] << "\n";
  }
}