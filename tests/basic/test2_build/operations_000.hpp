
//root
template<>
EGCoroutine __interface_root< void >::operator()()
{
  Test t = Test.Start();

SLEEP_UNTIL( t );

  co_return;
}

//root::Test
template<>
template<>
EGCoroutine __interface_root< void >::__interface_Test< void >::operator()()
{
  std::cout << "Hello World" << std::endl;

  co_return;
}
