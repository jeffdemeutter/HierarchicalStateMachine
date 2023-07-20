#include <functional>

template <typename E>
concept EnumType = std::is_enum_v<E>;

template <EnumType T>
class StateMachine
{
    struct TransitionContext
    {
        T to;
        std::function<bool()> condition;
    };

    struct StateContext
    {
        std::function<void()> onStart = []{};
        std::function<void()> onUpdate = []{};
        std::function<void()> onStop = []{};

        std::vector<TransitionContext> transitions{};
    };
    
public:
    StateMachine(T startState);
    ~StateMachine() = default;
    StateMachine(const StateMachine&) = delete;
    StateMachine(StateMachine&&) noexcept  = delete;
    StateMachine& operator=(const StateMachine&) = delete;
    StateMachine& operator=(StateMachine&&) noexcept  = delete;

    StateMachine& AddStateContext(T state, std::function<void()> onStart, std::function<void()> onUpdate, std::function<void()> onStop);
    StateMachine& AddTransition(T from, T to, std::function<bool()> condition);

    void Update();

private:
    std::unordered_map<T, StateContext> mStateContexts;
    T mCurrentState;

    void SwitchToState(const TransitionContext& stateContext);
};




template <EnumType T> 
StateMachine<T>::StateMachine(T startState)
    : mCurrentState(startState)
{
}

template <EnumType T>
StateMachine<T>& StateMachine<T>::AddStateContext(T state, std::function<void()> onStart, std::function<void()> onUpdate, std::function<void()> onStop)
{
    mStateContexts[state] = StateContext(onStart, onUpdate, onStop);

    return *this;
}

template <EnumType T>
StateMachine<T>& StateMachine<T>::AddTransition(T from, T to, std::function<bool()> condition)
{
    if (from != to)
        mStateContexts[from].transitions.emplace_back(TransitionContext{to, condition});
    
    return *this;
}

template <EnumType T>
void StateMachine<T>::Update()
{
    for (const auto& stateContext : mStateContexts[mCurrentState].transitions)
    {
        if (mCurrentState == stateContext.to)
            continue;

        if (!stateContext.condition())
            continue;

        SwitchToState(stateContext);
        return;
    }   

    mStateContexts[mCurrentState].onUpdate();
}

template <EnumType T>
void StateMachine<T>::SwitchToState(const TransitionContext& stateContext)
{
    mStateContexts[mCurrentState].onStop();
    mCurrentState = stateContext.to;
    mStateContexts[mCurrentState].onStart();
}