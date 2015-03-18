#ifndef TIMER_HPP
#define TIMER_HPP



namespace dboost_test
{

class wrong_timeout_exception
{
};

class timer 
{
public:
  virtual ~timer() {};
  virtual long add_timer(const long timeout_ms) throw (dboost_test::wrong_timeout_exception) = 0;
  virtual void remove_timer(const long id) = 0;
};

};

#endif // TIMER_HPP
