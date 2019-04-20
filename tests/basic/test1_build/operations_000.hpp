
//root
template<>
EGCoroutine __interface_root< void >::operator()()
{
  std::cout << "Hello World" << std::endl;

  co_return;
}
